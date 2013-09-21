#include "MemoryViewFrame.hpp"
#include "MemoryGrid.hpp"
#include <wx/textdlg.h>
#include <wx/msgdlg.h>

void PrintError(int error);

/** MemoryViewFrame
  *
  * Constructor
  */
MemoryViewFrame::MemoryViewFrame(wxWindow* parent, MemoryView* view) : MemoryViewFrameDecl(parent)
{
    memory->SetView(view);
}

/** ~MemoryViewFrame
  *
  * Destructor
  */
MemoryViewFrame::~MemoryViewFrame()
{

}

/** InitGridSizes
  *
  * Initializes Column Sizes
  */
void MemoryViewFrame::InitGridSizes(void)
{
    memory->InitGridSizes();
}

/** OnGoto
  *
  * Jumps to a memory address
  */
void MemoryViewFrame::OnGoto(wxCommandEvent& event)
{
    wxString wxaddress = wxGetTextFromUser(_("Input Address"), _("Goto Address"), wxEmptyString, this);
    std::string address = (std::string) wxaddress.mb_str();
    int addr;
    int error = lc3_calculate(state, address, addr);

    if (error)
    {
        PrintError(error);
        return;
    }

    memory->SelectLocation(0);
    memory->SelectLocation((unsigned short) addr);
    memory->Refresh();
}
