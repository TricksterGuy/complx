#include <cstdlib>
#include "colorlcd.hpp"

wxDEFINE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);

static Plugin* instance = NULL;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance != NULL)
        return instance;

    unsigned short width, height, startaddr, initaddr;

    if (lc3_params_read_ushort(params, "width", width) == false)
    {
        fprintf(stderr, "Width param (width) not given or in incorrect format: %s\n", lc3_params_get_value(params, "width").c_str());
        return NULL;
    }

    if (lc3_params_read_ushort(params, "height", height) == false)
    {
        fprintf(stderr, "Height param (height) not given or in incorrect format: %s\n", lc3_params_get_value(params, "height").c_str());
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

    /*if (wxTheApp == NULL)
    {
        fprintf(stderr, "[ERROR] Using this plugin with the command line version of the simulator is not supported at this time\n");
        return NULL;
    }*/

    instance = new ColorLCDPlugin(width, height, initaddr, startaddr);
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

ColorLCDPlugin::ColorLCDPlugin(unsigned short _width, unsigned short _height, unsigned short _initaddr, unsigned short _startaddr) :
    Plugin(COLORLCD_MAJOR_VERSION, COLORLCD_MINOR_VERSION, LC3_OTHER, "Color LCD Display"), width(_width),
    height(_height), initaddr(_initaddr), startaddr(_startaddr), lcd(NULL),
    lcd_initializing(false)
{
    BindAddress(initaddr);
    BindNAddresses(startaddr, width * height);
    Connect(wxID_ANY, wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEventHandler(ColorLCDPlugin::InitDisplay));
    Connect(wxID_ANY, wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEventHandler(ColorLCDPlugin::DestroyDisplay));
}

ColorLCDPlugin::~ColorLCDPlugin()
{
    if (lcd)
        delete lcd;
}

void ColorLCDPlugin::InitDisplay(wxThreadEvent& event)
{
    lcd = new ColorLCD(wxTheApp->GetTopWindow(), width, height, startaddr, event.GetPayload<lc3_state*>());
    lcd_initializing = false;
    lcd->Show();
}

void ColorLCDPlugin::DestroyDisplay(wxThreadEvent& event)
{
    delete lcd;
    lcd = NULL;
}

void ColorLCDPlugin::OnWrite(lc3_state& state, unsigned short address, short value)
{
    if (address == initaddr)
    {
        unsigned short data = value;
        if (data == 0x8000U && lcd == NULL)
        {
            wxThreadEvent* evt = new wxThreadEvent(wxEVT_COMMAND_CREATE_DISPLAY);
            evt->SetPayload<lc3_state*>(&state);
            wxQueueEvent(this, evt);
            lcd_initializing = true;
        }
        else if (data == 0x8000U && (lcd != NULL || lcd_initializing))
        {
            lc3_warning(state, "ColorLCD already initialized!");
        }
        else if (data == 0 && lcd == NULL)
        {
            lc3_warning(state, "ColorLCD is destroyed already!");
        }
        else if (data == 0 && (lcd != NULL || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else if (static_cast<unsigned short>(state.mem[address]) == 0x8000U && data != 0x8000U && (lcd != NULL || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else
        {
            lc3_warning(state, "Incorrect value written to ColorLCD");
        }
    }
    else if (address >= startaddr && address < startaddr + width * height && !lcd_initializing)
    {
        if (lcd == NULL)
            lc3_warning(state, "Writing to LCD while its not initialized!");
    }

    state.mem[address] = value;
}

ColorLCD::ColorLCD(wxWindow* top, int _width, int _height, unsigned short _startaddr, lc3_state* s) :
    COLORLCDGUI(top), timer(this), state(s), width(_width), height(_height), startaddr(_startaddr)
{
    //Centre();
    // 60 fps.
    timer.Start(1000.0 / 60);
    Connect(timer.GetId(), wxEVT_TIMER, wxTimerEventHandler(ColorLCD::OnUpdate), NULL, this);
    int x, y;
    GetParent()->GetScreenPosition(&x, &y);
    Move(x - GetSize().GetX(), y);
}

void ColorLCD::OnUpdate(wxTimerEvent& event)
{
    Refresh();
}

void ColorLCD::OnPaint( wxPaintEvent& event )
{
    /// @note see bwlcd.cpp
    if (state == NULL) return;

    wxPaintDC dc(displayPanel);
    dc.SetPen(wxNullPen);

    int cw, ch;
    displayPanel->GetSize(&cw, &ch);

    int tw = cw / width;
    int th = ch / height;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned short val = state->mem[startaddr + j + i * width];

            unsigned char r = (val >> 10) & 0x1f;
            unsigned char g = (val >> 5) & 0x1f;
            unsigned char b = val & 0x1f;

            wxColour color(r * 255 / 31, g * 255 / 31, b * 255 / 31);
            wxBrush brush(color);
            dc.SetBrush(brush);

            dc.DrawRectangle(j * tw, i * th, tw, th);
        }
    }
}
