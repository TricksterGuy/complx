#ifndef REGISTER_PROPERTY_GRID_ITEMS_HPP
#define REGISTER_PROPERTY_GRID_ITEMS_HPP

#include <wx/string.h>
#include <wx/propgrid/manager.h>

class RegisterPropertyGridItems
{
public:
    RegisterPropertyGridItems(const wxString& name, unsigned short value);
    ~RegisterPropertyGridItems() {}
    void Add(wxPropertyGridPage* page);
private:
    wxStringProperty* main = nullptr;
    wxUIntProperty* decimal = nullptr;
    wxStringProperty* hexadecimal = nullptr;
    const wxString name;
};


#endif
