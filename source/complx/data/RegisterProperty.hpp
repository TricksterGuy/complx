#ifndef REGISTER_PROPERTY_HPP
#define REGISTER_PROPERTY_HPP

#include <functional>

#include <wx/propgrid/propgrid.h> // Needed for because props.h doesn't include everything.
#include <wx/propgrid/props.h>

#include <wx/string.h>

/** wxPropertyGrid Property class representing a register
  * registers are simply a wxStringProperty with two child properties
  * for editing the register in decimal or hexadecimal.
  */
class RegisterProperty : public wxStringProperty
{
public:
    /** Sets default display base for the property. */
    enum Base
    {
        Decimal,
        Hexadecimal
    };
    enum Flags
    {
        Invalid = 0,
        /** Don't create a child property for changing the base. */
        NoBaseProperty = 1,
        /** Allow editing in hexadecimal. */
        AllowHexadecimal = 2,
        /** Allow editing in decimal. */
        AllowDecimal = 4,
        /** Default create a base property, allow both hex and decimal values. */
        Default = AllowHexadecimal | AllowDecimal,
    };

    RegisterProperty(const wxString& property, std::reference_wrapper<short> register_value, unsigned int display_base = Decimal, unsigned int flags = Default);
    ~RegisterProperty() {}

    /** Refreshes the Property Grid Property's value. */
    void RefreshDisplayedValue();
    /** @see wxPGProperty::ValidateValue */
    bool ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const override;
    /** Updates the base the registers value is displayed in. */
    void UpdateDisplayBase();
    /** Updates the LC3 State's register value via member variable value. */
    void UpdateRegisterValue();
    /** Updates the reference wrapper. Usually done after loading a new assembly file. */
    void UpdateRef(std::reference_wrapper<short> new_value);

private:
    /** Name of the register. */
    const wxString name;
    /** Reference to the register's value. */
    std::reference_wrapper<short> value;
    /** @see Base Display base of the register. */
    unsigned int base;
    /** @see Flags Flags for this RegisterProperty. */
    unsigned int flags;
    /** Base property, an optional child property for controlling the displayed base of the register. */
    wxEnumProperty* base_property = nullptr;
};

#endif
