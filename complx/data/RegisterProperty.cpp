#include "RegisterProperty.hpp"
#include "PropertyTypes.hpp"

#include <wx/debug.h>
#include <wx/valnum.h>

#include "logger.hpp"

const wxChar* bases[] = {
    wxT("Decimal"),
    wxT("Hexadecimal")
};

RegisterProperty::RegisterProperty(const wxString& property, std::reference_wrapper<short> register_value, Base display_base) :
    wxStringProperty(property),
    name(property),
    value(register_value),
    base(display_base),
    base_property(new wxEnumProperty("Display As", wxPG_LABEL, bases, nullptr, base))
{
    EventLog l(__func__);
    SetClientData(reinterpret_cast<void*>(PropertyType::Register));
    base_property->SetClientData(reinterpret_cast<void*>(PropertyType::RegisterDisplayBase));

    wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
    validator.SetCharIncludes("0123456789ABCDEFabcdefx-");
    SetValidator(validator);

    AppendChild(base_property);
    UpdateDisplayBase();
}

bool RegisterProperty::ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const
{
    EventLog l(__func__);
    wxString str = value.GetString();

    bool isHex = str[0] == 'x';

    // Reject xFFFFF, x
    if (isHex && (str.Length() > 5 || str.Length() == 1))
        return false;

    if (isHex)
    {
        // Reject x-34, xxxxx
        for (size_t i = 1; i < str.Length(); i++)
            if (!isxdigit(str[i]))
                return false;
    }
    else
    {
        // Reject a192
        if (!(str[0] == '-' || isdigit(str[0])))
            return false;

        for (size_t i = 1; i < str.Length(); i++)
            if (!isdigit(str[i]))
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
    short& reg = value.get();
    long convert;
    short old = reg;

    wxString str = GetValueAsString();

    if (str[0] == 'x')
    {
        bool ret = str.Mid(1).ToCULong(reinterpret_cast<unsigned long*>(&convert), 16);
        wxASSERT(ret);
        reg = convert;
    }
    else
    {
        bool ret = str.ToCLong(&convert, 10);
        wxASSERT(ret);
        reg = convert;
    }

    InfoLog("Updated %s from (%d %04x) to (%d %04x)", static_cast<const char*>(GetName()), old, static_cast<unsigned short>(old), reg, static_cast<unsigned short>(reg));
}

void RegisterProperty::UpdateDisplayBase()
{
    EventLog l(__func__);
    base = base_property->GetChoiceSelection();
    wxString str = (base == Decimal) ? wxString::Format("%d", value.get()) :
                                       wxString::Format("x%04x", static_cast<unsigned short>(value.get()));
    SetValue(str);

    InfoLog("Changed display base of %s to %s", static_cast<const char*>(GetName()), base == Decimal ? "Decimal" : "Hexadecimal");
}
