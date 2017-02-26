/***************************************************************
 * Name:      ComplxApp.hpp
 * Purpose:   Defines Application Class
 * Author:    Brandon (brandon.whitehead@gatech.edu)
 * Created:   2010-06-28
 * Copyright: Brandon ()
 * License:
 **************************************************************/

#ifndef COMPLXAPP_HPP
#define COMPLXAPP_HPP

#include <wx/app.h>

class ComplxFrame;

class ComplxApp : public wxApp
{
public:
    virtual bool OnInit();
    virtual int OnExit();
    virtual void OnInitCmdLine(wxCmdLineParser& parser);
    virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
};

extern ComplxFrame* complxframe;

#endif
