#ifndef VALIDATION_HELPER_HPP
#define VALIDATION_HELPER_HPP

#include <wx/string.h>

bool ValidateHexValue(const wxString& str);
bool ValidateDecimalValue(const wxString& str);

short ParseValueOrDie(const wxString& str);

#endif
