/***************************************************************
 * Name:      ComplxApp.hpp
 * Purpose:   Code For Application Class
 * Author:    Brandon (brandon.whitehead@gatech.edu)
 * Created:   2010-06-28
 * Copyright: Brandon ()
 * License:
 **************************************************************/

#include "ComplxApp.hpp"
#include "ComplxFrame.hpp"

#ifdef __linux__
#include <glib.h>
#endif

IMPLEMENT_APP(ComplxApp)

bool ComplxApp::OnInit()
{
    if (!wxApp::OnInit())
        return false;

    SetVendorName("Complx");
    SetAppName("Complx");

    srand(time(NULL));

    frame = new ComplxFrame();
    frame->Show();

    return true;
}

int ComplxApp::OnExit()
{
    return 0;
}

