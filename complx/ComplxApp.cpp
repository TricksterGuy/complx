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
#include "logger.hpp"

#ifdef __linux__
#include <glib.h>
#endif

IMPLEMENT_APP(ComplxApp)

bool ComplxApp::OnInit()
{
    logger->SetLogLevel(LogLevel::INFO);

    EventLog l(__func__);

    if (!wxApp::OnInit())
        return false;

    SetVendorName("Complx");
    SetAppName("Complx");

    frame = new ComplxFrame();
    frame->Show();

    return true;
}

int ComplxApp::OnExit()
{
    EventLog l(__func__);
    return 0;
}

