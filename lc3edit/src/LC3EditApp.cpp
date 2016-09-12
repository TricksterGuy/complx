/***************************************************************
 * Name:      LC3EditApp.hpp
 * Purpose:   Code For Application Class
 * Author:    Brandon (brandon.whitehead@gatech.edu)
 * Created:   2010-06-28
 * Copyright: Brandon ()
 * License:
 **************************************************************/

#include "LC3EditApp.hpp"
#include "LC3EditFrame.hpp"
#include "version.h"
#include "AsmFileDocument.hpp"
#include "EditorPanel.hpp"

#include <wx/wxprec.h>
#include <wx/cmdline.h>


static const wxCmdLineEntryDesc cmd_descriptions[] =
{
    { wxCMD_LINE_PARAM,  NULL, NULL, "input file", wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL},
    { wxCMD_LINE_NONE }
};


IMPLEMENT_APP(LC3EditApp);

// Command line
wxArrayString files;
LC3EditFrame* lc3editframe;

 /** OnInit
  *
  * Initializes the program
  */
bool LC3EditApp::OnInit()
{
    if (!wxApp::OnInit()) return false;

    srand(time(NULL));
    manager = new wxDocManager();
    templ = new wxDocTemplate(manager, "asm files", "*.asm", wxEmptyString, "asm", "asmdoc", "asmview", wxCLASSINFO(AsmFileDocument), wxCLASSINFO(EditorPanel));
    frame = new LC3EditFrame(manager, files);

    //wxIcon icon(icon32_xpm);
    //lc3editframe->SetIcon(icon);

    SetTopWindow(frame);
    frame->Centre();
    frame->Show();

    //frame->UpdateStatusBar();

    return true;
}

int LC3EditApp::OnRun()
{
    int exitcode = wxApp::OnRun();
    //wxTheClipboard->Flush();
    return exitcode;
}

/** @brief OnCmdLineParsed
  *
  * @todo: document this function
  */
bool LC3EditApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
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
void LC3EditApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    parser.SetLogo(wxString::Format(_("LC3Edit Version %ld.%ld"), AutoVersion::MAJOR, AutoVersion::MINOR));
    parser.SetDesc(cmd_descriptions);
    parser.SetSwitchChars (_("-"));
}
