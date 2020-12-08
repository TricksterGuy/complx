#ifndef BWLCD_HPP
#define BWLCD_HPP

#include <lc3.hpp>
#include <lc3_bwlcd/lc3_bwlcd_api.h>

#include <wx/wx.h>

#include "bwlcdgui.h"

#define BWLCD_MAJOR_VERSION 1
#define BWLCD_MINOR_VERSION 6

wxDECLARE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);

class BWLCD : public BWLCDGUI
{
public:
    BWLCD(wxWindow* top, int width, int height, uint16_t startaddr, unsigned int off, unsigned int on);
    void OnUpdate(wxThreadEvent& event);
    void OnPaint(wxPaintEvent& event) override;
private:
    lc3_state* state;
    int width;
    int height;
    uint16_t startaddr;
    unsigned int off;
    unsigned int on;
};

class LC3_BWLCD_API BWLCDPlugin : public wxEvtHandler, public Plugin
{
public:
    BWLCDPlugin(uint16_t width, uint16_t height, uint16_t initaddr, uint16_t startaddr, unsigned int offcolor = 0xa0b0a0, unsigned int oncolor = 0x606860);
    ~BWLCDPlugin();
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
    unsigned int offcolor;
    unsigned int oncolor;
    BWLCD* lcd;
    bool lcd_initializing;

};

extern "C"
{
    LC3_BWLCD_API Plugin* create_plugin(const PluginParams& params);
    LC3_BWLCD_API void destroy_plugin(Plugin* ptr);
}

#endif

