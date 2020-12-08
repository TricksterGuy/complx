#ifndef COLORLCD_HPP
#define COLORLCD_HPP

#include <lc3.hpp>
#include <lc3_colorlcd/lc3_colorlcd_api.h>

#include <wx/wx.h>
#include <wx/timer.h>

#include "colorlcdgui.h"

#define COLORLCD_MAJOR_VERSION 1
#define COLORLCD_MINOR_VERSION 6

wxDECLARE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);

class ColorLCD : public COLORLCDGUI
{
public:
    ColorLCD(wxWindow* top, int width, int height, uint16_t startaddr, lc3_state* state);
    virtual void OnUpdate(wxTimerEvent& event);
    void OnPaint(wxPaintEvent& event) override;
private:
    wxTimer timer;
    lc3_state* state;
    int width;
    int height;
    uint16_t startaddr;
};

class LC3_COLORLCD_API ColorLCDPlugin : public wxEvtHandler, public Plugin
{
public:
    ColorLCDPlugin(uint16_t width, uint16_t height, uint16_t initaddr, uint16_t startaddr);
    ~ColorLCDPlugin() override;
    void OnWrite(lc3_state& state, uint16_t address, int16_t value) override;
    void InitDisplay(wxThreadEvent& event);
    void UpdateDisplay(wxThreadEvent& event);
    void DestroyDisplay(wxThreadEvent& event);
    bool AvailableInLC3Test() const override {return false;}
private:
    uint16_t width;
    uint16_t height;
    uint16_t initaddr;
    uint16_t startaddr;
    ColorLCD* lcd;
    bool lcd_initializing;

};

extern "C"
{
    LC3_COLORLCD_API Plugin* create_plugin(const PluginParams& params);
    LC3_COLORLCD_API void destroy_plugin(Plugin* ptr);
}

#endif
