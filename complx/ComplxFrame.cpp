#include "ComplxFrame.hpp"
#include "data/PropertyTypes.hpp"

#include <wx/filedlg.h>
#include <wx/valnum.h>

#include <sstream>

#include "logger.hpp"

namespace
{

wxString AskForAssemblyFile()
{
    std::unique_ptr<wxFileDialog> dialog(new wxFileDialog(NULL, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR));
    if (dialog->ShowModal() == wxID_OK)
        return dialog->GetPath();

    return wxEmptyString;
}

}


ComplxFrame::ComplxFrame() : ComplxFrameDecl(nullptr), state(new lc3_state()), memory_view_model(new MemoryViewDataModel(std::ref(*state)))
{
    EventLog l(__func__);

    InitializeLC3State();
    InitializeMemoryView();
    InitializeStatePropGrid();
}

ComplxFrame::~ComplxFrame()
{
    EventLog l(__func__);
}

void ComplxFrame::OnLoad(wxCommandEvent& event)
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

    // Success failure logs handled in DoLoadFile.
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

void ComplxFrame::OnExit(wxCommandEvent& event)
{
    EventLog l(__func__);
    Destroy();
}

void ComplxFrame::OnStep(wxCommandEvent& event)
{
    EventLog l(__func__);
    InfoLog("Stepping 1 instruction");
    lc3_step(*state);
    PostExecute();
}

void ComplxFrame::OnBack(wxCommandEvent& event)
{
    EventLog l(__func__);
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
    state->default_seed = time(NULL);
    InfoLog("Random Seed %u", state->default_seed);
    lc3_init(*state);
}

void ComplxFrame::InitializeMemoryView()
{
    memoryView->UpdateRef(std::ref(*state));
    memoryView->AssociateModel(memory_view_model.get());
    memoryView->ScrollTo(0x3000);
}

void ComplxFrame::InitializeStatePropGrid()
{
    // Setup PC Property.
    pc_property = new RegisterProperty("PC", std::ref(reinterpret_cast<short&>(state->pc)), RegisterProperty::Hexadecimal, RegisterProperty::NoBaseProperty | RegisterProperty::AllowHexadecimal);
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

bool ComplxFrame::DoLoadFile(const LoadingOptions& opts)
{
    std::unique_ptr<lc3_state> new_state(new lc3_state());

    wxFileName filename(opts.file);
    bool randomize_registers = opts.registers == RANDOMIZE;
    bool randomize_memory = opts.memory == RANDOMIZE;
    short fill_registers = opts.registers;
    short fill_memory = opts.memory;

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
        WarnLog("Assembling file %s failed. Reason: %s", static_cast<const char*>(filename.GetFullName()), e.what().c_str());
        return false;
    }
    catch (const std::vector<LC3AssembleException>& vec)
    {
        WarnLog("Assembling file %s failed. Reasons below.\n-----", static_cast<const char*>(filename.GetFullName()));

        for (const auto& ex : vec)
            WarnLog("%s", ex.what().c_str());

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
    pc_property->UpdateRef(std::ref(reinterpret_cast<short&>(state->pc)));
    for (unsigned int i = 0; i < 8; i++)
        register_properties[i]->UpdateRef(std::ref(state->regs[i]));
    memoryView->Refresh();
    memoryView->ScrollTo(state->pc);
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
