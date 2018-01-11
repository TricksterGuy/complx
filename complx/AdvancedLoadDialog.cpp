#include "AdvancedLoadDialog.hpp"
#include "lc3.hpp"
#include "lc3_expressions.hpp"

extern lc3_state state;

AdvancedLoadDialog::AdvancedLoadDialog(wxWindow* parent, const LoadingOptions& opts) : AdvancedLoadDialogDecl(parent)
{
    assemblyFile->SetPath(opts.file);
    xmlFile->SetPath(opts.tests);
    consoleInput->SetValue(opts.console_input);
    regInitializer->SetSelection(opts.registers == RANDOMIZE ? 0 : (opts.registers == ZEROED ? 1 : 2));
    if (opts.registers != RANDOMIZE && opts.registers != ZEROED)
    {
        regFillValue->SetValue(wxString::Format("x%04x", (unsigned short) opts.registers));
        regFillValue->Enable();
    }
    memInitializer->SetSelection(opts.memory == RANDOMIZE ? 0 : (opts.memory == ZEROED ? 1 : 2));
    if (opts.memory != RANDOMIZE && opts.memory != ZEROED)
    {
        memFillValue->SetValue(wxString::Format("x%04x", (unsigned short) opts.memory));
        memFillValue->Enable();
    }
    pcValue->SetValue(wxString::Format("x%04x", opts.pc));
    trueTraps->SetValue(opts.true_traps);
    interrupts->SetValue(opts.interrupts);
}

LoadingOptions AdvancedLoadDialog::GetOptions()
{
    LoadingOptions options;
    std::string file = assemblyFile->GetPath().ToStdString();
    if (file.empty())
        return options;

    options.file = file;
    options.tests = xmlFile->GetPath().ToStdString();
    options.console_input = consoleInput->GetValue().ToStdString();
    int ret = -1;
    int error;
    switch(regInitializer->GetSelection())
    {
        case 0:
            options.registers = RANDOMIZE;
            break;
        case 1:
            options.registers = ZEROED;
            break;
        case 2:
            error = lc3_calculate(state, regFillValue->GetValue().ToStdString(), ret);
            options.registers = error ? ZEROED : static_cast<ValueInitializationMethod>(ret);
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
            error = lc3_calculate(state, memFillValue->GetValue().ToStdString(), ret);
            options.memory = error ? ZEROED : static_cast<ValueInitializationMethod>(ret);
            break;
    }
    error = lc3_calculate(state, pcValue->GetValue().ToStdString(), ret);
    options.pc = error != 0 ? 0x3000 : ret;
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
