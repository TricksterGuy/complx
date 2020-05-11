#ifndef CALL_SUBROUTINE_DIALOG_HPP
#define CALL_SUBROUTINE_DIALOG_HPP

#include "CallSubroutineDialogDecl.h"
#include <map>
#include <string>
#include <vector>
#include <wx/event.h>
#include <wx/window.h>

class CallSubroutineDialog : public CallSubroutineDialogDecl
{
public:
    CallSubroutineDialog(wxWindow* parent = NULL)  : CallSubroutineDialogDecl(parent)
    {
        SetupSubroutines();
    }
    std::string GetSubroutine() const;
    // Only for calling convention.
    std::map<std::string, std::string> GetEnvironment() const;
    std::vector<std::string> GetParams() const;
    bool IsCallingConvention() const;
    void OnSubroutineModeChange(wxCommandEvent& event) override;
private:
    void SetupSubroutines();
};

#endif
