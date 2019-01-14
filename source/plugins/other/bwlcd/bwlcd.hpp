#ifndef BWLCD_HPP
#define BWLCD_HPP

#include <lc3.hpp>

#include <wx/wx.h>

#include "bwlcdgui.h"

#define BWLCD_MAJOR_VERSION 1
#define BWLCD_MINOR_VERSION 6

wxDECLARE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);

class BWLCD : public BWLCDGUI
{
public:
    BWLCD(wxWindow* top, int width, int height, unsigned short startaddr, unsigned int off, unsigned int on);
    void OnUpdate(wxThreadEvent& event);
    void OnPaint(wxPaintEvent& event) override;
private:
    lc3_state* state;
    int width;
    int height;
    unsigned short startaddr;
    unsigned int off;
    unsigned int on;
};

class LC3_API BWLCDPlugin : public wxEvtHandler, public Plugin
{
public:
    BWLCDPlugin(unsigned short width, unsigned short height, unsigned short initaddr, unsigned short startaddr, unsigned int offcolor = 0xa0b0a0, unsigned int oncolor = 0x606860);
    ~BWLCDPlugin();
    void OnWrite(lc3_state& state, unsigned short address, short value) override;
    void InitDisplay(wxThreadEvent& event);
    void UpdateDisplay(wxThreadEvent& event);
    void DestroyDisplay(wxThreadEvent& event);
    bool AvailableInLC3Test() const override {return false;}
private:
    unsigned short width;
    unsigned short height;
    unsigned short initaddr;
    unsigned short startaddr;
    unsigned int offcolor;
    unsigned int oncolor;
    BWLCD* lcd;
    bool lcd_initializing;

};

extern "C" LC3_API Plugin* create_plugin(const PluginParams& params);
extern "C" LC3_API void destroy_plugin(Plugin* ptr);

#endif

