/***************************************************************
 * Name:      ComplxApp.hpp
 * Purpose:   Code For Application Class
 * Author:    Brandon (brandon.whitehead@gatech.edu)
 * Created:   2010-06-28
 * Copyright: Brandon ()
 * License:
 **************************************************************/

#include "icon32.xpm"
#include "ComplxApp.hpp"
#include "ComplxFrame.hpp"
#include "version.h"

#include <wx/wxprec.h>
#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/msgdlg.h>
#include <wx/tipdlg.h>


static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "u", "unsigned", "Display decimal as unsigned 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "d", "disassemble", "Sets up the disassemble level 0 = dumb 1: normal 2: smart (default)",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "s", "stack_size", "Sets the undo stack size default 65536 instructions",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "a", "address", "Sets PC's starting address default 0x3000",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "t", "true_traps", "Enables true traps 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "ie", "interrupts_enable", "Enables interrupts 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "i", "highlight", "Enable instruction highlight 0 = no 1 = yes (default)",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "state", "state", "Loads an lc3 state into the program",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_NONE }
};


IMPLEMENT_APP(ComplxApp);

// Command line
long decimal = 0, disassemble = 2, stack_size = 65536, true_traps = 0, interrupts = 0, highlight = 1;
wxString address_str = wxEmptyString, state_file = wxEmptyString;
wxArrayString files;
ComplxFrame* complxframe;

 /** OnInit
  *
  * Initializes the program
  */
bool ComplxApp::OnInit()
{
    if (!wxApp::OnInit()) return false;

    SetVendorName("Complx");
    SetAppName("Complx");

    wxFileConfig *config = new wxFileConfig("Complx");
    wxConfigBase::Set(config);

    srand(time(NULL));
    complxframe = new ComplxFrame(decimal, disassemble, stack_size, true_traps, interrupts, highlight, address_str, state_file, files);
    wxIcon icon(icon32_xpm);
    complxframe->SetIcon(icon);
    complxframe->Show();

    size_t currentTip = 0;
    bool show = true;
    std::string last_ver = config->Read("/firstrun", "").ToStdString();
    // use our config object...
    if (last_ver.empty()) {
        wxCommandEvent event;
        complxframe->OnFirstTime(event);
        config->Write("/firstrun", AutoVersion::FULLVERSION_STRING);
        config->Flush();
    }
    else
    {
        currentTip = config->Read("/currenttip", 0l);
        config->Read("/showtips", &show);
    }

    if (show)
    {
        wxTipProvider* tip = wxCreateFileTipProvider("/usr/local/share/complx-tools/complx-tips.txt", currentTip);
        show = wxShowTip(complxframe, tip, show);

        config->Write("/showtips", show);
        config->Write("/currenttip", tip->GetCurrentTip());
        config->Flush();
        delete tip;
    }

    return true;
}

int ComplxApp::OnExit()
{
    // clean up: Set() returns the active config object as Get() does, but unlike
    // Get() it doesn't try to create one if there is none (definitely not what
    // we want here!)
    delete wxConfigBase::Get();//(wxConfigBase *) NULL);

    //wxPuts("HELLO");

    return 0;
}

int ComplxApp::OnRun()
{
    int exitcode = wxApp::OnRun();
    //wxTheClipboard->Flush();
    return exitcode;
}

/** @brief OnCmdLineParsed
  *
  * @todo: document this function
  */
bool ComplxApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    parser.Found(_("u"), &decimal);
    parser.Found(_("d"), &disassemble);
    parser.Found(_("s"), &stack_size);
    parser.Found(_("a"), &address_str);
    parser.Found(_("t"), &true_traps);
    parser.Found(_("ie"), &interrupts);
    parser.Found(_("i"), &highlight);
    parser.Found(_("state"), &state_file);

    // get unnamed parameters
    for (unsigned int i = 0; i < parser.GetParamCount(); i++)
    {
        files.Add(parser.GetParam(i));
    }

    return true;
}

/** @brief OnInitCmdLine
  *
  * @todo: document this function
  */
void ComplxApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetLogo(wxString::Format(_("Complx Version %ld.%ld"), AutoVersion::MAJOR, AutoVersion::MINOR));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}

