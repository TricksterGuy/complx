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
#include <wx/docview.h>

class LC3EditFrame;

class LC3EditApp : public wxApp
{
    public:
        virtual bool OnInit();
        virtual void OnInitCmdLine(wxCmdLineParser& parser);
        virtual bool OnCmdLineParsed(wxCmdLineParser& parser);
        int OnRun();
        LC3EditFrame* GetFrame() const {return frame;}
    private:
        LC3EditFrame* frame;
        wxDocManager* manager;
        wxDocTemplate* templ;
};

#endif
