#ifndef COLORLCD_HPP
#define COLORLCD_HPP

#include <lc3_plugin.hpp>
#include <lc3_parser.hpp>
#include <wx/wx.h>
#include "colorlcdgui.h"

#define COLORLCD_MAJOR_VERSION 1
#define COLORLCD_MINOR_VERSION 3

wxDECLARE_EVENT(wxEVT_COMMAND_CREATE_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_DESTROY_DISPLAY, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_UPDATE_DISPLAY, wxThreadEvent);

class ColorLCD : public COLORLCDGUI
{
    public:
        ColorLCD(wxWindow* top, int width, int height, unsigned short startaddr);
        ~ColorLCD();
		virtual void OnUpdate(wxThreadEvent& event);
		void OnPaint( wxPaintEvent& event );
    private:
        lc3_state* state;
        int width;
        int height;
        unsigned short startaddr;
};

class ColorLCDPlugin : public wxEvtHandler, public Plugin
{
    public:
        ColorLCDPlugin(unsigned short width, unsigned short height, unsigned short initaddr, unsigned short startaddr);
        ~ColorLCDPlugin();
        virtual void OnMemoryWrite(lc3_state& state, unsigned short address, short value);
        void InitDisplay(wxThreadEvent& event);
        void UpdateDisplay(wxThreadEvent& event);
        void DestroyDisplay(wxThreadEvent& event);
    private:
        unsigned short width;
        unsigned short height;
        unsigned short initaddr;
        unsigned short startaddr;
        ColorLCD* lcd;
        bool lcd_initializing;

};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

