#ifndef REGISTER_PROPERTY_HPP
#define REGISTER_PROPERTY_HPP

#include <functional>

#include <wx/string.h>

#include <wx/propgrid/propgrid.h> // Needed for because props.h doesn't include everything.
#include <wx/propgrid/props.h>

/** wxPropertyGrid Property class representing a register
  * registers are simply a wxStringProperty with two child properties
  * for editing the register in decimal or hexadecimal.
  */
class RegisterProperty : public wxStringProperty
{
public:
    enum Base {
        Decimal,
        Hexadecimal
    };

    RegisterProperty(const wxString& property, std::reference_wrapper<short> register_value, Base display_base = Decimal);
    ~RegisterProperty() {}

    bool ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const override;
    void UpdateDisplayBase();
    void UpdateRegisterValue();

private:
    const wxString name;
    std::reference_wrapper<short> value;
    unsigned int base = Decimal;
    wxEnumProperty* base_property = nullptr;
};

#endif
