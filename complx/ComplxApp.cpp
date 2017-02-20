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

#include <wx/cmdline.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/msgdlg.h>
#include <wx/tipdlg.h>
#include <wx/wxprec.h>

static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    { wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP },
    { wxCMD_LINE_OPTION, "d", "disassemble", "Sets up the disassemble level 0 = basic 1: normal (default) 2: smart",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "s", "stack_size", "Sets the undo stack size default 65536 instructions",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "c", "call_stack_size", "Sets the call stack size default 10000 JSR/Trap calls",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "a", "address", "Sets PC's starting address default 0x3000",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "t", "true_traps", "Enables true traps 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "ie", "interrupts_enable", "Enables interrupts 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_OPTION, "i", "highlight", "Enable instruction highlight 0 = no 1 = yes (default)",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_NONE }
};


IMPLEMENT_APP(ComplxApp)

// Command line
long disassemble = 1, stack_size = 65536, call_stack_size = 10000, true_traps = 0, interrupts = 0, highlight = 1;
wxString address_str = wxEmptyString;
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
    // Sleep here for a fix for a bug with a previous Ubuntu version where there was a deadlock with wxWidgets apps not
    // starting due to a GTK issue.
    ///TODO see if I can remove this.
    wxMilliSleep(50);

	ComplxFrame::Options opts;
	opts.title = wxString::Format("%s v%s", "Complx LC-3 Simulator", Version::FULLVERSION_STRING);
	opts.disassemble = disassemble > 2 ? 2 : disassemble;
	opts.stack_size = stack_size;
	opts.call_stack_size = call_stack_size;
	opts.true_traps = true_traps != 0;
	opts.interrupts = interrupts != 0;
	opts.highlight = highlight != 0;
	opts.pc = address_str;
	if (!files.empty())
		opts.file = files[0];

    complxframe = new ComplxFrame(opts);
    wxIcon icon(icon32_xpm);
    complxframe->SetIcon(icon);
    complxframe->Show();

    std::string last_ver = config->Read("/firstrun", "").ToStdString();
    // use our config object...
    if (last_ver.empty()) {
        wxCommandEvent event;
        complxframe->OnFirstTime(event);
        config->Write("/firstrun", Version::FULLVERSION_STRING);
        config->Flush();
    }

    return true;
}

int ComplxApp::OnExit()
{
    delete wxConfigBase::Get();
    return 0;
}

/** @brief OnCmdLineParsed
  *
  * @todo: document this function
  */
bool ComplxApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    parser.Found(_("d"),  &disassemble);
    parser.Found(_("s"),  &stack_size);
    parser.Found(_("a"),  &address_str);
    parser.Found(_("t"),  &true_traps);
    parser.Found(_("ie"), &interrupts);
    parser.Found(_("i"),  &highlight);

    // get unnamed parameters
    for (unsigned int i = 0; i < parser.GetParamCount(); i++)
        files.Add(parser.GetParam(i));

    return true;
}

/** @brief OnInitCmdLine
  *
  * @todo: document this function
  */
void ComplxApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetLogo(wxString::Format(_("Complx Version %ld.%ld"), Version::MAJOR, Version::MINOR));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}
