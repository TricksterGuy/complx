#ifndef CALL_SUBROUTINE_DIALOG_HPP
#define CALL_SUBROUTINE_DIALOG_HPP

#include "CallSubroutineDialogDecl.h"
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
    const std::string GetSubroutine() const;
    const std::string GetStack() const;
    const std::vector<std::string> GetParams() const;
    bool IsRandomRegisters() const;
    bool IsRandomMemory() const;
private:
    void SetupSubroutines();
};

#endif
