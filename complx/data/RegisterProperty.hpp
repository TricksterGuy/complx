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
    /** Sets default display base for the property*/
    enum Base {
        Decimal,
        Hexadecimal
    };
    enum Flags {
        Invalid = 0,
        /** Don't create a child property for changing the base */
        NoBaseProperty = 1,
        /** Allow editing in hexadecimal*/
        AllowHexadecimal = 2,
        /** Allow editing in decimal */
        AllowDecimal = 4,
        /** Default create a base property, allow both hex and decimal values */
        Default = AllowHexadecimal | AllowDecimal,
    };

    RegisterProperty(const wxString& property, std::reference_wrapper<short> register_value, unsigned int display_base = Decimal, unsigned int flags = Default);
    ~RegisterProperty() {}

    bool ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const override;
    void UpdateDisplayBase();
    void UpdateRegisterValue();

private:
    const wxString name;
    std::reference_wrapper<short> value;
    unsigned int base;
    unsigned int flags;
    wxEnumProperty* base_property = nullptr;
};

#endif
