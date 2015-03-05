#include <cstdlib>
#include "bwlcd.hpp"

wxDEFINE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_UPDATE_DISPLAY, wxThreadEvent);

static Plugin* instance = NULL;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance != NULL)
        return instance;

    unsigned short widthaddr, heightaddr, startaddr, initaddr;
    unsigned int oncolor = 0x606860, offcolor = 0xa0b0a0, maxsize = 500;

    if (lc3_params_read_ushort(params, "widthaddr", widthaddr) == false)
    {
        fprintf(stderr, "Width param (widthaddr) not given or in incorrect format: %s\n", lc3_params_get_value(params, "widthaddr").c_str());
        return NULL;
    }

    if (lc3_params_read_ushort(params, "heightaddr", heightaddr) == false)
    {
        fprintf(stderr, "Height param (heightaddr) not given or in incorrect format: %s\n", lc3_params_get_value(params, "heightaddr").c_str());
        return NULL;
    }

    if (lc3_params_read_ushort(params, "startaddr", startaddr) == false)
    {
        fprintf(stderr, "Start Address param (startaddr) not given or in incorrect format: %s\n", lc3_params_get_value(params, "startaddr").c_str());
        return NULL;
    }

    if (lc3_params_read_ushort(params, "initaddr", initaddr) == false)
    {
        fprintf(stderr, "Initial Address param (initaddr) not given or in incorrect format: %s\n", lc3_params_get_value(params, "initaddr").c_str());
        return NULL;
    }

    lc3_params_read_uint(params, "oncolor", oncolor);
    lc3_params_read_uint(params, "offcolor", offcolor);
    lc3_params_read_uint(params, "maxsize", maxsize);

    /*if (wxTheApp == NULL)
    {
        fprintf(stderr, "[ERROR] Using this plugin with the command line version of the simulator is not supported at this time\n");
        return NULL;
    }*/

    instance = new BWLCDPlugin(widthaddr, heightaddr, initaddr, startaddr, offcolor, oncolor, maxsize);
    return instance;
}

void destroy_plugin(Plugin* ptr = NULL)
{
    if (ptr == instance)
    {
        delete instance;
        instance = NULL;
    }
}

/** @brief BWLCDPlugin
  *
  * @todo: document this function
  */
 BWLCDPlugin::BWLCDPlugin(unsigned short _widthaddr, unsigned short _heightaddr, unsigned short _initaddr,
                          unsigned short _startaddr, unsigned int _offcolor, unsigned int _oncolor, unsigned int _maxsize) :
    Plugin(BWLCD_MAJOR_VERSION, BWLCD_MINOR_VERSION, LC3_OTHER, "Black & White LCD Display"), widthaddr(_widthaddr),
    heightaddr(_heightaddr), initaddr(_initaddr), startaddr(_startaddr), offcolor(_offcolor), oncolor(_oncolor), maxsize(_maxsize), lcd(NULL),
    lcd_initializing(false)
{
    width = height = 0;
    Connect(wxID_ANY, wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEventHandler(BWLCDPlugin::InitDisplay));
    Connect(wxID_ANY, wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEventHandler(BWLCDPlugin::DestroyDisplay));
}

/** @brief ~BWLCDPlugin
  *
  * @todo: document this function
  */
 BWLCDPlugin::~BWLCDPlugin()
{
    if (lcd)
        delete lcd;
}

/** @brief InitDisplay
  *
  * @todo: document this function
  */
void BWLCDPlugin::InitDisplay(wxThreadEvent& event)
{
    lcd = new BWLCD(wxTheApp->GetTopWindow(), width, height, startaddr, offcolor, oncolor, maxsize);
    lcd_initializing = false;
    lcd->Show();
    lcd->Connect(wxID_ANY, wxEVT_COMMAND_UPDATE_DISPLAY, wxThreadEventHandler(BWLCD::OnUpdate));

    wxThreadEvent* evt = new wxThreadEvent(wxEVT_COMMAND_UPDATE_DISPLAY);
    evt->SetPayload<lc3_state*>(event.GetPayload<lc3_state*>());
    wxQueueEvent(lcd, evt);
}

/** @brief DestroyDisplay
  *
  * @todo: document this function
  */
void BWLCDPlugin::DestroyDisplay(wxThreadEvent& event)
{
    delete lcd;
    lcd = NULL;
}

/** @brief OnMemoryWrite
  *
  * @todo: document this function
  */
void BWLCDPlugin::OnMemoryWrite(lc3_state& state, unsigned short address, short value)
{
    if (address == initaddr)
    {
        unsigned short data = value;
        if (data == 0x8000U && lcd == NULL)
        {
	    if (state.mem[widthaddr] && state.mem[heightaddr])
	    {
                width = state.mem[widthaddr];
	        height = state.mem[heightaddr];
                wxThreadEvent* evt = new wxThreadEvent(wxEVT_COMMAND_CREATE_DISPLAY);
                evt->SetPayload<lc3_state*>(&state);
                wxQueueEvent(this, evt);
                lcd_initializing = true;
	    }
	    else
	    {
                lc3_warning(state, "BWLCD must have non-zero dimensions set before initialization!");
	    }
        }
        else if (data == 0x8000U && (lcd != NULL || lcd_initializing))
        {
            lc3_warning(state, "BWLCD already initialized!");
        }
        else if (value == 0 && lcd == NULL)
        {
            lc3_warning(state, "BWLCD is destroyed already!");
        }
        else if (value == 0 && (lcd != NULL || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else
        {
            lc3_warning(state, "Incorrect value written to BWLCD");
        }
    }
    else if (address >= startaddr && address < startaddr + width * height)
    {
        if (lcd == NULL)
        {
            lc3_warning(state, "Writing to LCD while its not initialized!");
        }
        else
        {
            wxThreadEvent* evt = new wxThreadEvent(wxEVT_COMMAND_UPDATE_DISPLAY);
            evt->SetPayload<lc3_state*>(&state);
            wxQueueEvent(lcd, evt);
        }
    }
}

/** @brief BWLCD
  *
  * @todo: document this function
  */
 BWLCD::BWLCD(wxWindow* top, int _width, int _height, unsigned short _startaddr, unsigned int _off, unsigned int _on, unsigned int _maxsize) :
 BWLCDGUI(top), width(_width), height(_height), startaddr(_startaddr), off(_off), on(_on), maxsize(_maxsize)
{
    state = NULL;
    unsigned int sw = _width * TILE_SIZE, sh = _height * TILE_SIZE;
    if(sw < MIN_SIZE || sh < MIN_SIZE)
    {
        if(sw < sh)
	{
            sh = sh * MIN_SIZE / sw;
	    sw = MIN_SIZE;
	}
	else
	{
            sw = sw * MIN_SIZE / sh;
	    sh = MIN_SIZE;
	}
    }
    else if(sw > _maxsize || sh > _maxsize)
    {
        if(sw > sh)
	{
            sh = sh * _maxsize / sw;
	    sw = _maxsize;
	}
	else
	{
            sw = sw * _maxsize / sh;
	    sh = _maxsize;
	}
    }
    SetClientSize(sw, sh);
    int x, y;
    GetParent()->GetScreenPosition(&x, &y);
    Move(x - GetSize().GetX(), y);
}

/** @brief ~BWLCD
  *
  * @todo: document this function
  */
 BWLCD::~BWLCD()
{

}

/** @brief OnUpdate
  *
  * @todo: document this function
  */
void BWLCD::OnUpdate(wxThreadEvent& event)
{
    /// @todo there has to be a better way of doing this than just storing the lc3_state object!
    state = event.GetPayload<lc3_state*>();
    Refresh();
}

void BWLCD::OnPaint( wxPaintEvent& event )
{
    if (state == NULL) return;

    wxPaintDC dc(displayPanel);
    dc.SetPen(wxNullPen);

    wxColour oncolor(on);
    wxColour offcolor(off);
    wxBrush onBrush(oncolor);
    wxBrush offBrush(offcolor);

    int cw, ch;
    displayPanel->GetClientSize(&cw, &ch);

    for (int i = 0; i < height; i++)
    {
        int ty = ch * i / height;
	int th = ch * (i + 1) / height - ty;
        for (int j = 0; j < width; j++)
        {
	    int tx = cw * j / width;
	    int tw = cw * (j + 1) / width - tx;
	    dc.SetBrush(state->mem[startaddr + j + i * width] ? onBrush : offBrush);
            dc.DrawRectangle(tx, ty, tw, th);
        }
    }
}

