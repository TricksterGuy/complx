#include "ComplxFrame.hpp"
#include "data/PropertyTypes.hpp"

#include <wx/valnum.h>

ComplxFrame::ComplxFrame() : ComplxFrameDecl(nullptr), memoryViewModel(new MemoryViewDataModel(std::ref(state)))
{
    lc3_init(state);
    memoryView->AssociateModel(memoryViewModel.get());

    for (unsigned int i = 0; i < 8; i++)
    {
        auto base = i < 5 ? RegisterProperty::Decimal : RegisterProperty::Hexadecimal;
        auto* property = new RegisterProperty(wxString::Format("R%d", i), std::ref(state.regs[i]), base);
        registerProperties.push_back(property);
        statePropGrid->Append(property);
        // -32768 is 6 characters.
        statePropGrid->SetPropertyMaxLength(property, 6);
    }
    statePropGridManager->GetGrid()->CenterSplitter();
}

ComplxFrame::~ComplxFrame()
{

}

void ComplxFrame::OnStateChange(wxPropertyGridEvent& event)
{
    auto* property = event.GetProperty();
    auto property_type = reinterpret_cast<uintptr_t>(property->GetClientData());

    if (property_type == PropertyType::Register)
    {
        auto* register_property = dynamic_cast<RegisterProperty*>(property);
        wxASSERT(register_property);
        register_property->UpdateRegisterValue();
        return;
    }

    if (property_type == PropertyType::RegisterDisplayBase)
    {
        auto* register_property = dynamic_cast<RegisterProperty*>(property->GetParent());
        wxASSERT(register_property);
        register_property->UpdateDisplayBase();
        return;
    }
}
