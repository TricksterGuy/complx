/***************************************************************
 * Name:      LC3EditApp.hpp
 * Purpose:   Defines Application Class
 * Author:    Brandon (brandon.whitehead@gatech.edu)
 * Created:   2010-06-28
 * Copyright: Brandon ()
 * License:
 **************************************************************/

#ifndef LC3EDITAPP_HPP
#define LC3EDITAPP_HPP

#include <wx/app.h>

class LC3EditFrame;

class LC3EditApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        int OnRun();

};

extern LC3EditFrame* lc3editframe;

#endif
