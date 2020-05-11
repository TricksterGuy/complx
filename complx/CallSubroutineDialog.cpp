#include "CallSubroutineDialog.hpp"
#include <lc3_all.hpp>

extern lc3_state state;

std::string CallSubroutineDialog::GetSubroutine() const
{
    return subroutine->GetStringSelection().ToStdString();
}

std::vector<std::string> CallSubroutineDialog::GetParams() const
{
    std::vector<std::string> param_list;
    if (IsCallingConvention())
    {
        tokenize(params->GetValue().ToStdString(), param_list, ",");
    }
    else
    {
        param_list.push_back(r0Value->GetValue().ToStdString());
        param_list.push_back(r1Value->GetValue().ToStdString());
        param_list.push_back(r2Value->GetValue().ToStdString());
        param_list.push_back(r3Value->GetValue().ToStdString());
        param_list.push_back(r4Value->GetValue().ToStdString());
        param_list.push_back(r5Value->GetValue().ToStdString());
        param_list.push_back(r6Value->GetValue().ToStdString());
        param_list.push_back(r7Value->GetValue().ToStdString());
    }
    return param_list;
}

std::map<std::string, std::string> CallSubroutineDialog::GetEnvironment() const
{
    std::map<std::string, std::string> env;
    env["STACK"] = stack->GetValue().ToStdString();
    env["R5"] = r5ValueLCC->GetValue().ToStdString();
    env["R7"] = r7ValueLCC->GetValue().ToStdString();
    return env;
}

bool CallSubroutineDialog::IsCallingConvention() const
{
    return subroutineMode->GetSelection() == 0;
}

void CallSubroutineDialog::OnSubroutineModeChange(wxCommandEvent& event)
{
    if (subroutineMode->GetSelection() == 0)
    {
        passByRegistersPanel->Show(false);
        callingConventionPanel->Show(true);
    }
    else
    {
        passByRegistersPanel->Show(true);
        callingConventionPanel->Show(false);
    }
    passByRegistersPanel->Layout();
    Layout();
}

void CallSubroutineDialog::SetupSubroutines()
{
    subroutine->Clear();
    for (const auto& symbol_loc : state.symbols)
        subroutine->Append(symbol_loc.first);
}
