#ifndef CALL_STACK_DIALOG_HPP
#define CALL_STACK_DIALOG_HPP

#include "CallStackDialogDecl.h"
#include <vector>
#include <wx/event.h>
#include <wx/window.h>

#define ID_REWIND 5120
#define ID_GOTOCALL 5121

class CallStackDialog : public CallStackDialogDecl
{
public:
    CallStackDialog(wxWindow* parent = NULL);
    int GetFrame() const;
    int GetRewindId() const;
    int GetViewStackId() const;
private:
    void OnRewindToCall(wxCommandEvent& event);
    void OnViewStackFrame(wxCommandEvent& event);
    void SetupList();
};

#endif
