#include "ComplxFrame.hpp"
#include "data/PropertyTypes.hpp"

#include <sstream>

#include <wx/filedlg.h>
#include <wx/valnum.h>

#include <logging.hpp>

namespace
{

wxString AskForAssemblyFile()
{
    std::unique_ptr<wxFileDialog> dialog(new wxFileDialog(nullptr, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR));
    if (dialog->ShowModal() == wxID_OK)
        return dialog->GetPath();
    return wxEmptyString;
}

}

ComplxFrame::ComplxFrame() : ComplxFrameDecl(nullptr), state(new lc3_state()), memory_view_model(new MemoryViewDataModel(std::ref(*state)))
{
    EventLog l(__func__);

    InitializeOutput();
    InitializeLC3State();
    InitializeMemoryView();
    InitializeStatePropGrid();
}

ComplxFrame::~ComplxFrame()
{
    EventLog l(__func__);
    logger->SetLogTarget(std::cerr);
}

void ComplxFrame::OnLoad(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    wxString file = AskForAssemblyFile();
    if (file.IsEmpty())
    {
        WarnLog("No file loaded.");
        return;
    }

    InfoLog("File selected: %s", static_cast<const char*>(file));

    LoadingOptions options;
    options.file = file;

    // Success/failure logs handled in DoLoadFile.
    if (DoLoadFile(options))
        reload_options = options;
}

void ComplxFrame::OnReload(wxCommandEvent& event)
{
    EventLog l(__func__);
    if (reload_options.file.IsEmpty())
    {
        OnLoad(event);
        return;
    }

    InfoLog("Reloading: %s", static_cast<const char*>(reload_options.file));
    DoLoadFile(reload_options);
}

void ComplxFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    Destroy();
}

void ComplxFrame::OnStep(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    PreExecute();
    InfoLog("Stepping 1 instruction");
    lc3_step(*state);
    PostExecute();
}

void ComplxFrame::OnBack(wxCommandEvent& WXUNUSED(event))
{
    EventLog l(__func__);
    PreExecute();
    InfoLog("Stepping back 1 instruction");
    lc3_back(*state);
    PostExecute();
}

void ComplxFrame::OnStateChange(wxPropertyGridEvent& event)
{
    EventLog l(__func__);
    auto* property = event.GetProperty();
    auto property_type = reinterpret_cast<uintptr_t>(property->GetClientData());

    if (property_type == PropertyType::Register)
    {
        auto* register_property = dynamic_cast<RegisterProperty*>(property);
        wxASSERT(register_property);
        register_property->UpdateRegisterValue();
    }

    if (property_type == PropertyType::RegisterDisplayBase)
    {
        auto* register_property = dynamic_cast<RegisterProperty*>(property->GetParent());
        wxASSERT(register_property);
        register_property->UpdateDisplayBase();
    }

    // This means they changed the value of the PC.
    if (property == pc_property)
        memoryView->Refresh();

    if (property == cc_property)
        cc_property->UpdateRegisterValue();
}

void ComplxFrame::InitializeLC3State()
{
    state->default_seed = time(nullptr);

    InfoLog("Random Seed %u", state->default_seed);

    lc3_init(*state);

    state->output = output.get();
    state->warning = warning.get();
    state->trace = trace.get();
    state->reader = std::bind(&ComplxFrame::ConsoleRead, this, std::placeholders::_1, std::placeholders::_2);
    state->peek = std::bind(&ComplxFrame::ConsolePeek, this, std::placeholders::_1, std::placeholders::_2);
}

void ComplxFrame::InitializeMemoryView()
{
    memoryView->UpdateRef(std::ref(*state));
    memoryView->AssociateModel(memory_view_model.get());
    memoryView->ScrollTo(state->pc);
}

void ComplxFrame::InitializeStatePropGrid()
{
    // Setup PC Property.
    pc_property = new RegisterProperty("PC", std::ref(reinterpret_cast<int16_t&>(state->pc)), RegisterProperty::Hexadecimal, RegisterProperty::NoBaseProperty | RegisterProperty::AllowHexadecimal);
    statePropGrid->Append(pc_property);
    // xFFFF
    statePropGrid->SetPropertyMaxLength(pc_property, 5);

    // Setup CC property.
    cc_property = new ProcessStatusRegisterProperty(std::ref(*state), ProcessStatusRegisterProperty::DisplayAsCC);
    statePropGrid->Append(cc_property);
    statePropGrid->SetPropertyMaxLength(cc_property, cc_property->GetPropertyMaximumLength());

    for (unsigned int i = 0; i < 8; i++)
    {
        auto base = i < 5 ? RegisterProperty::Decimal : RegisterProperty::Hexadecimal;

        auto* property = new RegisterProperty(wxString::Format("R%d", i), std::ref(state->regs[i]), base);
        statePropGrid->Append(property);
        // -32768 is 6 characters.
        statePropGrid->SetPropertyMaxLength(property, 6);
        statePropGrid->Collapse(property);

        register_properties[i] = property;
    }

    statePropGridManager->GetGrid()->CenterSplitter();
}

void ComplxFrame::InitializeOutput()
{
    output = std::make_unique<std::ostream>(consoleText);
    warning = std::make_unique<std::ostream>(warningText);
    trace = std::make_unique<std::ostream>(traceText);
    logging = std::make_unique<std::ostream>(loggingText);

    // Copy initial logs to the logging textctrl. And switch to logging to the textctrl.
    const auto& old = dynamic_cast<const std::stringstream&>(logger->GetLogTarget());
    *logging << (old.str());
    logger->SetLogTarget(*logging);
}

bool ComplxFrame::DoLoadFile(const LoadingOptions& opts)
{
    std::unique_ptr<lc3_state> new_state(new lc3_state());

    wxFileName filename(opts.file);
    bool randomize_registers = opts.registers == RANDOMIZE;
    bool randomize_memory = opts.memory == RANDOMIZE;
    int16_t fill_registers = opts.registers;
    int16_t fill_memory = opts.memory;

    new_state->default_seed = (opts.has_random_seed) ? opts.random_seed : state->default_seed;
    lc3_init(*new_state, randomize_registers, randomize_memory, fill_registers, fill_memory);
    new_state->pc = opts.pc;

    try
    {
        std::vector<code_range> ranges;
        LC3AssembleOptions options;
        options.multiple_errors = true;
        options.warnings_as_errors = false;
        options.process_debug_comments = true;
        options.enable_warnings = false;
        options.disable_plugins = false;
        lc3_assemble(*new_state, filename.GetFullPath().ToStdString(), ranges, options);
    }
    catch (const LC3AssembleException& e)
    {
        WarnLog("Assembling file %s failed. Reason: %s", static_cast<const char*>(filename.GetFullName()), e.what());
        return false;
    }

    InfoLog("Successfully loaded file: %s", static_cast<const char*>(opts.file));

    /// TODO should really have an lc3_destroy function.
    // Without this plugins will be dangling.
    lc3_init(*state);

    state = std::move(new_state);
    PostLoadFile();
    reload_options.file_modification_time = filename.GetModificationTime();

    return true;
}

void ComplxFrame::PostLoadFile()
{
    memoryView->UpdateRef(std::ref(*state));
    cc_property->UpdateRef(std::ref(*state));
    pc_property->UpdateRef(std::ref(reinterpret_cast<int16_t&>(state->pc)));
    for (unsigned int i = 0; i < 8; i++)
        register_properties[i]->UpdateRef(std::ref(state->regs[i]));
    memoryView->Refresh();
    memoryView->ScrollTo(state->pc);
}

void ComplxFrame::PreExecute()
{
    TransferDataFromWindow();
}

void ComplxFrame::PostExecute()
{
    memoryView->Refresh();
    memoryView->ScrollTo(state->pc);

    for (auto& property : register_properties)
        property->RefreshDisplayedValue();
    pc_property->RefreshDisplayedValue();
    cc_property->RefreshDisplayedValue();
}

int ComplxFrame::ConsoleRead(lc3_state& state, std::istream&)
{
    if (consoleInput.IsEmpty())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0, 0);
        state.pc--;
        state.halted = true;
        return -1;
    }
    int ret = consoleInput[0];
    consoleInput = consoleInput.Mid(1);
    InfoLog("Read Character %c value: %d", static_cast<char>(ret), ret);
    TransferDataToWindow();
    return ret;
}

int ComplxFrame::ConsolePeek(lc3_state& state, std::istream&)
{
    if (consoleInput.IsEmpty())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0, 0);
        state.pc--;
        state.halted = true;
        return -1;
    }
    InfoLog("Peek Character %c value: %d", static_cast<char>(consoleInput[0]), consoleInput[0]);
    return consoleInput[0];
}

