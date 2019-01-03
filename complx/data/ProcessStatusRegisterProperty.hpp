#ifndef PROCESS_STATUS_REGISTER_PROPERTY_HPP
#define PROCESS_STATUS_REGISTER_PROPERTY_HPP

#include <functional>

#include <lc3_all.hpp>

#include <wx/propgrid/propgrid.h> // Needed for because props.h doesn't include everything.
#include <wx/propgrid/props.h>

#include <wx/string.h>

class ProcessStatusRegisterProperty : public wxStringProperty
{
public:
    enum DisplayMode
    {
        Invalid = 0,
        DisplayAsCC = 1,
        DisplayAsPSR = 2,
    };
    ProcessStatusRegisterProperty(std::reference_wrapper<lc3_state> state, unsigned int display_mode = Invalid);
    ~ProcessStatusRegisterProperty() {}
    bool ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const override;
    void UpdateRegisterValue();
    void UpdateDisplay();
    int GetPropertyMaximumLength() const { return mode == DisplayAsCC ? 1 : 5;}
private:
    std::reference_wrapper<lc3_state> state_ref;
    unsigned int mode = Invalid;
};


#endif
