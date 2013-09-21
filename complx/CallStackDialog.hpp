#ifndef CALL_STACK_DIALOG_HPP
#define CALL_STACK_DIALOG_HPP

#include <wx/event.h>
#include <wx/window.h>
#include <vector>
#include "CallStackDialogDecl.h"
#include "lc3.hpp"

extern lc3_state state;

#define ID_REWIND 5120
#define ID_GOTOCALL 5121

class CallStackDialog : public CallStackDialogDecl
{
    public:
        CallStackDialog(wxWindow* parent = NULL);
        ~CallStackDialog();
        int GetFrame() const;
        int GetRewindId() const;
        int GetViewStackId() const;
    private:
		void OnRewindToCall(wxCommandEvent& event);
		void OnViewStackFrame(wxCommandEvent& event);
		void SetupList();
};

#endif
