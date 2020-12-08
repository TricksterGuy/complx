#include "bwlcd.hpp"

#include <cstdlib>
#include <memory>
#include <sstream>

wxDEFINE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    uint16_t width, height, startaddr, initaddr;
    unsigned int oncolor = 0x606860, offcolor = 0xa0b0a0;

    width = params.read_uint16_t_required("width");
    height = params.read_uint16_t_required("height");
    startaddr = params.read_uint16_t_required("startaddr");
    initaddr = params.read_uint16_t_required("initaddr");
    params.read_uint("oncolor", oncolor);
    params.read_uint("offcolor", offcolor);

    /*if (wxTheApp == nullptr)
    {
        fprintf(stderr, "[ERROR] Using this plugin with the command line version of the simulator is not supported at this time\n");
        return nullptr;
    }*/

    instance = std::make_unique<BWLCDPlugin>(width, height, initaddr, startaddr, offcolor, oncolor);

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

BWLCDPlugin::BWLCDPlugin(uint16_t _width, uint16_t _height, uint16_t _initaddr,
                         uint16_t _startaddr, unsigned int _offcolor, unsigned int _oncolor) :
    Plugin(BWLCD_MAJOR_VERSION, BWLCD_MINOR_VERSION, LC3_OTHER, "Black & White LCD Display"), width(_width),
    height(_height), initaddr(_initaddr), startaddr(_startaddr), offcolor(_offcolor), oncolor(_oncolor), lcd(nullptr),
    lcd_initializing(false)
{
    BindAddress(initaddr);
    BindNAddresses(startaddr, width * height);
    Connect(wxID_ANY, wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEventHandler(BWLCDPlugin::InitDisplay));
    Connect(wxID_ANY, wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEventHandler(BWLCDPlugin::DestroyDisplay));
}

BWLCDPlugin::~BWLCDPlugin()
{
    if (lcd)
        delete lcd;
}

void BWLCDPlugin::InitDisplay(wxThreadEvent& WXUNUSED(event))
{
    lcd = new BWLCD(wxTheApp->GetTopWindow(), width, height, startaddr, offcolor, oncolor);
    lcd_initializing = false;
    lcd->Show();
}

void BWLCDPlugin::DestroyDisplay(wxThreadEvent& WXUNUSED(event))
{
    delete lcd;
    lcd = nullptr;
}

void BWLCDPlugin::OnWrite(lc3_state& state, uint16_t address, int16_t value)
{
    if (address == initaddr)
    {
        uint16_t data = value;
        if (data == 0x8000U && lcd == nullptr)
        {
            wxThreadEvent* evt = new wxThreadEvent(wxEVT_COMMAND_CREATE_DISPLAY);
            evt->SetPayload<lc3_state*>(&state);
            wxQueueEvent(this, evt);
            lcd_initializing = true;
        }
        else if (data == 0x8000U && (lcd != nullptr || lcd_initializing))
        {
            lc3_warning(state, "BWLCD already initialized!");
        }
        else if (data == 0 && lcd == nullptr)
        {
            lc3_warning(state, "BWLCD is destroyed already!");
        }
        else if (data == 0 && (lcd != nullptr || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else if (static_cast<uint16_t>(state.mem[address]) == 0x8000U && data != 0x8000U && (lcd != nullptr || lcd_initializing))
        {
            wxQueueEvent(this, new wxThreadEvent(wxEVT_COMMAND_DESTROY_DISPLAY));
        }
        else
        {
            lc3_warning(state, "Incorrect value written to BWLCD");
        }
    }
    else if (address >= startaddr && address < startaddr + width * height && !lcd_initializing)
    {
        if (lcd == nullptr)
            lc3_warning(state, "Writing to LCD while its not initialized!");
    }

    state.mem[address] = value;
}


BWLCD::BWLCD(wxWindow* top, int _width, int _height, uint16_t _startaddr, unsigned int _off, unsigned int _on) :
    BWLCDGUI(top), state(nullptr), width(_width), height(_height), startaddr(_startaddr), off(_off), on(_on)
{
    int x, y;
    GetParent()->GetScreenPosition(&x, &y);
    Move(x - GetSize().GetX(), y);
}


void BWLCD::OnUpdate(wxThreadEvent& event)
{
    /// @todo there has to be a better way of doing this than just storing the lc3_state object!
    state = event.GetPayload<lc3_state*>();
    Refresh();
}

void BWLCD::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    if (state == nullptr) return;

    wxPaintDC dc(displayPanel);
    dc.SetPen(wxNullPen);

    wxColour oncolor(on);
    wxColour offcolor(off);
    wxBrush onBrush(oncolor);
    wxBrush offBrush(offcolor);


    int cw, ch;
    displayPanel->GetSize(&cw, &ch);

    int tw = cw / width;
    int th = ch / height;

    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (state->mem[startaddr + j + i * width])
                dc.SetBrush(onBrush);
            else
                dc.SetBrush(offBrush);
            dc.DrawRectangle(j * tw, i * th, tw, th);
        }
    }
}
