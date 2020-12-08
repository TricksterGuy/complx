#include "RegisterProperty.hpp"

#include "../util/ValidationHelper.hpp"
#include "PropertyTypes.hpp"

#include <wx/debug.h>
#include <wx/valnum.h>

#include <logging.hpp>


namespace {

const wxChar* bases[] = {
    wxT("Decimal"),
    wxT("Hexadecimal")
};

wxString GetAllowedCharacters(unsigned int flags)
{
    wxString ret = "";
    if ((flags & RegisterProperty::AllowDecimal) && (flags & RegisterProperty::AllowHexadecimal))
        ret = "0123456789ABCDEFabcdefx-";
    else if (flags & RegisterProperty::AllowHexadecimal)
        ret = "0123456789ABCDEFabcdefx";
    else if (flags & RegisterProperty::AllowDecimal)
        ret = "0123456789-";
    return ret;
}

}

RegisterProperty::RegisterProperty(const wxString& property, std::reference_wrapper<int16_t> register_value, unsigned int display_base, unsigned int settings) :
    wxStringProperty(property),
    name(property),
    value(register_value),
    base(display_base),
    flags(settings)
{
    EventLog l(__func__);
    SetClientData(reinterpret_cast<void*>(PropertyType::Register));

    wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
    validator.SetCharIncludes(GetAllowedCharacters(settings));
    SetValidator(validator);

    if (!(flags & NoBaseProperty))
    {
        base_property = new wxEnumProperty("Display As", wxPG_LABEL, bases, nullptr, base);
        base_property->SetClientData(reinterpret_cast<void*>(PropertyType::RegisterDisplayBase));
        AppendChild(base_property);
    }
    RefreshDisplayedValue();
}

void RegisterProperty::RefreshDisplayedValue()
{
    wxString str = (base == Decimal) ? wxString::Format("%d", value.get()) :
                                       wxString::Format("x%04x", static_cast<uint16_t>(value.get()));
    SetValue(str);
}

bool RegisterProperty::ValidateValue(wxVariant& value, wxPGValidationInfo& WXUNUSED(validationInfo)) const
{
    EventLog l(__func__);
    wxString str = value.GetString();

    bool isHex = str[0] == 'x';

    // Reject if flags didn't allow mode.
    if (isHex && !(flags & AllowHexadecimal))
        return false;
    if (!isHex && !(flags & AllowDecimal))
        return false;

    if (isHex)
    {
        if (str.Length() > 5 || !ValidateHexValue(str))
            return false;
    }
    else
    {
        if (!ValidateDecimalValue(str))
            return false;

        // Reject -99999, 99999
        int val = wxAtoi(str);
        if (val < -32768 || val > 32767)
            return false;
    }

    return true;
}

void RegisterProperty::UpdateRegisterValue()
{
    EventLog l(__func__);
    int16_t& reg = value.get();

    int16_t old = reg;

    reg = ParseValueOrDie(GetValueAsString());

    InfoLog("Updated %s from (%d x%04x) to (%d x%04x)", static_cast<const char*>(GetName()), old, static_cast<uint16_t>(old), reg, static_cast<uint16_t>(reg));
}

void RegisterProperty::UpdateDisplayBase()
{
    EventLog l(__func__);

    if (base_property)
        base = static_cast<Base>(base_property->GetChoiceSelection());

    RefreshDisplayedValue();

    InfoLog("Changed display base of %s to %s", static_cast<const char*>(GetName()), base == Decimal ? "Decimal" : "Hexadecimal");
}

void RegisterProperty::UpdateRef(std::reference_wrapper<int16_t> new_value)
{
    value = new_value;
    RefreshDisplayedValue();
}
