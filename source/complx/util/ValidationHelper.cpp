#include "ValidationHelper.hpp"

bool ValidateHexValue(const wxString& str)
{
    if (str.IsEmpty())
        return false;

    // Reject strings that don't begin with x
    if (str[0] != 'x')
        return false;

    // Reject x
    if (str.Length() == 1)
        return false;

    // Reject x-34, xxxxx
    for (size_t i = 1; i < str.Length(); i++)
        if (!isxdigit(str[i]))
            return false;

    return true;
}

bool ValidateDecimalValue(const wxString& str)
{
    if (str.IsEmpty())
        return false;

    // Reject a192
    if (str[0] == '-' && str.Length() == 1)
        return false;

    if (str[0] != '-' && !isdigit(str[0]))
        return false;

    for (size_t i = 1; i < str.Length(); i++)
        if (!isdigit(str[i]))
            return false;

    return true;
}

int16_t ParseValueOrDie(const wxString& str)
{
    long convert;
    if (str[0] == 'x')
    {
        bool ret = str.Mid(1).ToCULong(reinterpret_cast<unsigned long*>(&convert), 16);
        wxASSERT(ret);
        return convert;
    }
    else
    {
        bool ret = str.ToCLong(&convert, 10);
        wxASSERT(ret);
        return convert;
    }
}
