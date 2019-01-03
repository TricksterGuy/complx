#include "ComplxFrame.hpp"
#include "data/PropertyTypes.hpp"

#include <wx/valnum.h>

#include "logger.hpp"

ComplxFrame::ComplxFrame() : ComplxFrameDecl(nullptr), memory_view_model(new MemoryViewDataModel(std::ref(state)))
{
    EventLog l(__func__);

    state.default_seed = time(NULL);
    InfoLog("Random Seed %u", state.default_seed);
    lc3_init(state);

    memoryView->AssociateModel(memory_view_model.get());
    memoryView->ScrollTo(0x3000);

    // Setup PC Property.
    pc_property = new RegisterProperty("PC", std::ref(reinterpret_cast<short&>(state.pc)), RegisterProperty::Hexadecimal, RegisterProperty::NoBaseProperty | RegisterProperty::AllowHexadecimal);
    statePropGrid->Append(pc_property);
    // xFFFF
    statePropGrid->SetPropertyMaxLength(pc_property, 5);

    // Setup CC property.
    cc_property = new ProcessStatusRegisterProperty(std::ref(state), ProcessStatusRegisterProperty::DisplayAsCC);
    statePropGrid->Append(cc_property);
    statePropGrid->SetPropertyMaxLength(cc_property, cc_property->GetPropertyMaximumLength());

    for (unsigned int i = 0; i < 8; i++)
    {
        auto base = i < 5 ? RegisterProperty::Decimal : RegisterProperty::Hexadecimal;

        auto* property = new RegisterProperty(wxString::Format("R%d", i), std::ref(state.regs[i]), base);
        statePropGrid->Append(property);
        // -32768 is 6 characters.
        statePropGrid->SetPropertyMaxLength(property, 6);
        statePropGrid->Collapse(property);

        register_properties.push_back(property);
    }

    statePropGridManager->GetGrid()->CenterSplitter();
}

ComplxFrame::~ComplxFrame()
{
    EventLog l(__func__);
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
