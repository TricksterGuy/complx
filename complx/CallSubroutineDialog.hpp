#ifndef CALL_SUBROUTINE_DIALOG_HPP
#define CALL_SUBROUTINE_DIALOG_HPP

#include "CallSubroutineDialogDecl.h"
#include <wx/event.h>
#include <wx/window.h>
#include <string>
#include <vector>
#include <lc3.hpp>

extern lc3_state state;

class CallSubroutineDialog : public CallSubroutineDialogDecl
{
    public:
        CallSubroutineDialog(wxWindow* parent = NULL)  : CallSubroutineDialogDecl(parent) {SetupSubroutines();}
        ~CallSubroutineDialog() {}
        const std::string GetSubroutine() const;
        const std::string GetStack() const;
        const std::vector<std::string> GetParams() const;
        bool IsRandomRegisters() const;
        bool IsRandomMemory() const;
    private:
        void SetupSubroutines();
};

#endif
