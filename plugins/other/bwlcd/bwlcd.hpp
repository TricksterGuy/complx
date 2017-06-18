#ifndef BWLCD_HPP
#define BWLCD_HPP

#include <lc3_all.hpp>
#include <wx/wx.h>
#include "bwlcdgui.h"

#define BWLCD_MAJOR_VERSION 1
#define BWLCD_MINOR_VERSION 4

wxDECLARE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_DISPLAY, wxThreadEvent);

class BWLCD : public BWLCDGUI
{
public:
    BWLCD(wxWindow* top, int width, int height, unsigned short startaddr, unsigned int off, unsigned int on);
    virtual void OnUpdate(wxThreadEvent& event);
    void OnPaint( wxPaintEvent& event );
private:
    lc3_state* state;
    int width;
    int height;
    unsigned short startaddr;
    unsigned int off;
    unsigned int on;
};

class BWLCDPlugin : public wxEvtHandler, public Plugin
{
public:
    BWLCDPlugin(unsigned short width, unsigned short height, unsigned short initaddr, unsigned short startaddr, unsigned int offcolor = 0xa0b0a0, unsigned int oncolor = 0x606860);
    ~BWLCDPlugin();
    virtual void OnMemoryWrite(lc3_state& state, unsigned short address, short value);
    //virtual void OnTock(lc3_state& state);
    void InitDisplay(wxThreadEvent& event);
    void UpdateDisplay(wxThreadEvent& event);
    void DestroyDisplay(wxThreadEvent& event);
    virtual bool AvailableInLC3Test() const {return false;}
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

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

