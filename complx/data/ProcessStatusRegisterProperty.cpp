#include "ProcessStatusRegisterProperty.hpp"
#include "../util/ValidationHelper.hpp"

namespace
{

wxString GetAllowedCharacters(unsigned int mode)
{
    wxString ret = "";
    if (mode == ProcessStatusRegisterProperty::DisplayAsCC)
        ret = "NZPnzp";
    else if (mode == ProcessStatusRegisterProperty::DisplayAsPSR)
        ret = "0123456789ABCDEFabcdefx";
    return ret;
}

}


ProcessStatusRegisterProperty::ProcessStatusRegisterProperty(std::reference_wrapper<lc3_state>state, unsigned int display_mode) :
    wxStringProperty(display_mode == DisplayAsCC ? "CC" : "PSR"), state_ref(state), mode(display_mode)
{
    wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
    validator.SetCharIncludes(GetAllowedCharacters(mode));
    SetValidator(validator);

    UpdateDisplay();
}

bool ProcessStatusRegisterProperty::ValidateValue(wxVariant& value, wxPGValidationInfo& validationInfo) const
{
    if (mode == DisplayAsCC)
        return true;
    else if (mode == DisplayAsPSR)
        /// TODO additional validations like not setting multiple cc bits.
        return ValidateHexValue(value.GetString());
    else
        return false;
}

void ProcessStatusRegisterProperty::UpdateRegisterValue()
{
    lc3_state& state = state_ref.get();
    wxString str = GetValueAsString();

    if (mode == DisplayAsCC)
    {
        state.n = str == "N" || str == "n";
        state.z = str == "Z" || str == "z";
        state.p = str == "P" || str == "p";
    }
    else if (mode == DisplayAsPSR)
    {
        unsigned short psr = ParseValueOrDie(str);
        state.privilege = (psr >> 16) & 1;
        state.priority = (psr >> 8) & 7;
        state.n = (psr >> 2) & 1;
        state.z = (psr >> 1) & 1;
        state.p = (psr >> 0) & 1;
    }
}

void ProcessStatusRegisterProperty::UpdateDisplay()
{
    lc3_state& state = state_ref.get();

    if (mode == DisplayAsCC)
    {
        SetValue(state.n ? "N" : (state.z ? "Z" : "P"));
    }
    else if (mode == DisplayAsPSR)
    {
        unsigned short psr = (state.privilege << 15) | (state.priority << 8) | (state.n << 2) | (state.z << 1) | state.p;
        SetValue(wxString::Format("x%04x", psr));
    }
}
