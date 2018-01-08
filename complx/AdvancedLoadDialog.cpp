#include "AdvancedLoadDialog.hpp"
#include "lc3.hpp"
#include "lc3_expressions.hpp"

extern lc3_state state;

LoadingOptions AdvancedLoadDialog::GetOptions()
{
    LoadingOptions options;
    std::string file = assemblyFile->GetPath().ToStdString();
    if (file.empty())
        return options;

    options.file = file;
    options.tests = xmlFile->GetPath().ToStdString();
    options.console_input = consoleInput->GetValue().ToStdString();
    switch(regInitializer->GetSelection())
    {
        case 0:
            options.registers = RANDOMIZE;
            break;
        case 1:
            options.registers = ZEROED;
            break;
        case 2:
            options.registers = static_cast<ValueInitializationMethod>(regFillValue->GetValue());
            break;
    }
    switch(memInitializer->GetSelection())
    {
        case 0:
            options.memory = RANDOMIZE;
            break;
        case 1:
            options.memory = ZEROED;
            break;
        case 2:
            options.memory = static_cast<ValueInitializationMethod>(memFillValue->GetValue());
            break;
    }
    int ret = -1;
    int error = lc3_calculate(state, pcValue->GetValue().ToStdString(), ret);
    options.pc = ret == -1 ? 0x3000 : ret;
    options.true_traps = trueTraps->IsChecked();
    options.interrupts = interrupts->IsChecked();
    return options;
}

void AdvancedLoadDialog::OnRegChoice(wxCommandEvent& event)
{
    regFillValue->Enable(event.GetSelection() == 2);
}

void AdvancedLoadDialog::OnMemChoice(wxCommandEvent& event)
{
    memFillValue->Enable(event.GetSelection() == 2);
}
