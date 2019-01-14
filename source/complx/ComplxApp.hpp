/***************************************************************
 * Name:      ComplxApp.hpp
 * Purpose:   Defines Application Class
 * Author:    Brandon (brandon.whitehead@gatech.edu)
 * Created:   2010-06-28
 * Copyright: Brandon ()
 * License:
 **************************************************************/

#ifndef COMPLX_APP_HPP
#define COMPLX_APP_HPP

#include <wx/app.h>
#include <wx/frame.h>

class ComplxApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;
private:
    wxFrame* frame = nullptr;
};

#endif
