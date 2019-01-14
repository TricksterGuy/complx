#ifndef PROCESS_STATUS_REGISTER_PROPERTY_HPP
#define PROCESS_STATUS_REGISTER_PROPERTY_HPP

#include <functional>

#include <lc3.hpp>

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
    /** Refreshes the Property Grid Property's value. */
    void RefreshDisplayedValue();
    /** Updates the LC3 state's CC or PSR value */
    void UpdateRegisterValue();
    /** Updates the reference wrapper. Usually done after loading a new assembly file */
    void UpdateRef(std::reference_wrapper<lc3_state> new_value);
    /** Gets the Maximum Length for editing. Not to be confused with GetMaxLength */
    int GetPropertyMaximumLength() const { return mode == DisplayAsCC ? 1 : 5; }
private:
    std::reference_wrapper<lc3_state> state_ref;
    unsigned int mode = Invalid;
};


#endif
