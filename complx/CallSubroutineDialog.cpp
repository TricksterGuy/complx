#include "CallSubroutineDialog.hpp"
#include <lc3_all.hpp>

extern lc3_state state;

const std::string CallSubroutineDialog::GetSubroutine() const
{
    return subroutine->GetStringSelection().ToStdString();
}

const std::string CallSubroutineDialog::GetStack() const
{
    return stack->GetValue().ToStdString();
}

const std::vector<std::string> CallSubroutineDialog::GetParams() const
{
    std::vector<std::string> param_list;
    tokenize(params->GetValue().ToStdString(), param_list, ",");
    return param_list;
}

bool CallSubroutineDialog::IsRandomRegisters() const
{
    return randomRegisters->IsChecked();
}

bool CallSubroutineDialog::IsRandomMemory() const
{
    return randomMemory->IsChecked();
}

void CallSubroutineDialog::SetupSubroutines()
{
    subroutine->Clear();
    for (const auto& symbol_loc : state.symbols)
        subroutine->Append(symbol_loc.first);
}
