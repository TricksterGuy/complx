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

#ifdef __linux__
#include <glib.h>
#endif

static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    {
        wxCMD_LINE_SWITCH, "h", "help", "Displays help on command line parameters",
        wxCMD_LINE_VAL_NONE, wxCMD_LINE_OPTION_HELP
    },
    {
        wxCMD_LINE_OPTION, "d", "disassemble", "Sets up the disassemble level 0 = basic 1: normal (default) 2: smart",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "s", "stack_size", "Sets the undo stack size default 65536 instructions",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "c", "call_stack_size", "Sets the call stack size default 10000 JSR/Trap calls",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "a", "address", "Sets PC's starting address default 0x3000",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "t", "true_traps", "Enables true traps 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "ie", "interrupts_enable", "Enables interrupts 0 = no (default) 1 = yes",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "i", "highlight", "Enable instruction highlight 0 = no 1 = yes (default)",
        wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "r", "fill_registers", "Registers fill value default random",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL
    },
    {
        wxCMD_LINE_OPTION, "m", "fill_memory", "Memory fill value default random",
        wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL
    },
    { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_NONE }
};


IMPLEMENT_APP(ComplxApp)

// Command line
long disassemble = 1, stack_size = 65536, call_stack_size = 10000, true_traps = 0, interrupts = 0, highlight = 1, pc = 0x3000, memory_fill = 0, register_fill = 0;
wxString address_str = wxEmptyString, register_fill_str = "random", memory_fill_str = "random";
wxArrayString files;
ComplxFrame* complxframe;

wxConfig* CreateConfig(void)
{
#ifdef __linux__
    /// TODO remove when wxWidgets 3.2 trickles itself down to sweet Ubuntu (3.1 has xdg support).
    wxString config_dir = wxString::FromUTF8(g_build_filename(g_get_user_config_dir(), "complx", NULL));
    wxString config_file = config_dir + wxFILE_SEP_PATH + "complx.cfg";
    if (!wxFileName::Mkdir(config_dir, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL))
    {
        fprintf(stderr, "Couldn't create config file directory. Home folder is not writable?\n");
        config_file = wxEmptyString;
    }
    wxFileConfig* config = new wxFileConfig("complx", wxEmptyString, config_file);
#else
    wxFileConfig* config = new wxFileConfig("complx");
#endif
    return config;
}

/** OnInit
 *
 * Initializes the program
 */
bool ComplxApp::OnInit()
{
    if (!wxApp::OnInit()) return false;

    SetVendorName("Complx");
    SetAppName("Complx");


    wxConfig* config = CreateConfig();
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
    opts.highlight = highlight != 0;
    opts.width = 800;
    opts.height = 600;

    opts.loading_options.true_traps = true_traps != 0;
    opts.loading_options.interrupts = interrupts != 0;
    if (address_str.ToCLong(&pc, 16))
        opts.loading_options.pc = pc & 0xFFFF;
    if (memory_fill_str.ToCLong(&memory_fill, 0))
        opts.loading_options.memory = (ValueInitializationMethod) memory_fill;
    if (register_fill_str.ToCLong(&register_fill, 0))
        opts.loading_options.registers = (ValueInitializationMethod) register_fill;
    if (memory_fill_str == "random")
        opts.loading_options.memory = RANDOMIZE;
    if (register_fill_str == "random")
        opts.loading_options.registers = RANDOMIZE;

    if (!files.empty())
        opts.loading_options.file = files[0];

    std::string last_ver = config->Read("/firstrun", "").ToStdString();
    std::string column_size_str = config->Read("/columnsizes", "").ToStdString();
    std::string width_str = config->Read("/appwidth", "800").ToStdString();
    std::string height_str = config->Read("/appheight", "600").ToStdString();

    bool firstrun = false;
    opts.exact_column_sizing = true;

    if (last_ver.empty())
    {
        firstrun = true;
    }
    else if (last_ver != Version::FULLVERSION_STRING)
    {
        config->Write("/firstrun", Version::FULLVERSION_STRING);
        config->Flush();
    }
    else
    {
        // Operation that takes 2 seconds don't want to do it if it is not the first run.
        opts.exact_column_sizing = false;
        std::vector<std::string> columns;
        tokenize(column_size_str, columns, ",");
        if (columns.size() == MemorySize)
        {
            for (const auto& str : columns)
                opts.column_sizes.push_back(wxAtoi(str));
        }
        if (!width_str.empty())
            opts.width = wxAtoi(width_str);
        if (!height_str.empty())
            opts.height = wxAtoi(height_str);
    }

    complxframe = new ComplxFrame(opts);
    wxIcon icon(icon32_xpm);
    complxframe->SetIcon(icon);
    complxframe->Show();

    // use our config object...
    if (firstrun)
    {
        wxCommandEvent event;
        complxframe->OnFirstTime(event);
        config->Write("/firstrun", Version::FULLVERSION_STRING);
        config->Flush();
    }

    // To anyone who is looking at these messages will be removed once I have more test coverage.  This is my compromise for now.
    // These messages have been added due to numerous complaints about the native mac version.
    // The reason why I am adding this dialog is to remind users to file bugs and to remind them that this isn't officially supported by me at this time.
    // Background: there are numerous flags when setting up wxWidgets components, and certain flags do not work on certain ports of wxWidgets.
    // I would need some time to investigate this behavior on different platforms.
    // While it is my goal to have this program be cross platform, I would like for the look and feel and behavior to be consistent. I take pride in my work in this way.
    // However, I'm not up for the additional headache caused by having to support more OSes at this time.
    // The good thing is that support for additional OSes will come, but on my own terms and time
    // If you want to speed up this process then lets improve the test coverage should we can verify that things are working as intended.
    // Please do not remove from distributions of this binary on other platforms, I will find out if it is removed, I will not be happy.
#ifdef __APPLE__
    wxMessageBox("It appears you are running complx on a Mac OSX system.\n\n"
                 "Note that while the TAs have made available instructions for installing this program on Macs, "
                 "understand that at the same time this program has not been fully tested on Mac systems and thus I, Brandon, "
                 "can not fully support running this program on Mac systems.\n\n"
                 "This program was originally designed by me with Linux in mind as that is what is required for CS2110. "
                 "There have been numerous bug reports about things broken in the Mac version of complx. "
                 "While I would like to fix these issues and have the user experience consistent across platforms, "
                 "I do not have the time to address them, if bugs are found they may not be fixed until December of this year. "
                 "As it stands right now the user experience is inferior to that of the native linux version.\n\n"
                 "Remember that you should have the Linux version (via virtual machine or dual boot) handy in case something breaks. "
                 "If you find bugs with this version please report them via Help > Create Bug Report so that I am made aware.\n\n"
                 "I do also support pull requests, what is needed is more test automation (preferred) and manual testing to ensure regressions don't happen.\n",
                 "Please note (Message from Brandon)");
#endif
// Its rather hard to produce a native binary of complx on Windows systems, but just in case...
#ifdef __WINDOWS__
    wxMessageBox("It appears you are running complx on a Windows system\n\n"
                 "Please understand that this program is not fully tested on Windows systems and thus I, Brandon, do not support running this program on Windows. "
                 "This program was originally designed by me with Linux in mind as that is what is required for CS2110.\n\n"
                 "As far as I know complx does not work well natively on windows based systems. "
                 "This is due to Windows having a completely different threading model than Linux. "
                 "You may notice a lot of things don't work well or at all.\n\n"
                 "If you find bugs with this version please report them via Help > Create Bug Report so that I am made aware of them.\n\n"
                 "I do also support pull requests, what is needed is more test automation (preferred) and manual testing to ensure regressions don't happen.\n",
                 "Please note (Message from Brandon)");
#endif

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
    parser.Found(_("r"),  &register_fill_str);
    parser.Found(_("m"),  &memory_fill_str);
    parser.Found(_("t"),  &true_traps);
    parser.Found(_("ie"), &interrupts);
    parser.Found(_("i"),  &highlight);

    register_fill_str.MakeLower();
    memory_fill_str.MakeLower();

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
    parser.SetLogo(wxString::Format(_("Complx Version %s"), Version::FULLVERSION_STRING));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}
