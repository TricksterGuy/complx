#include "RegisterPropertyGridItems.hpp"

RegisterPropertyGridItems::RegisterPropertyGridItems(const wxString& property, unsigned short value) : name(property)
{
    main = new wxStringProperty(name, name, "<composed>");
    decimal = new wxUIntProperty("decimal", "decimal", value);
    hexadecimal = new wxStringProperty("hexadecimal", "hexadecimal", wxString::Format("x%04x", value));
    main->AppendChild(decimal);
    main->AppendChild(hexadecimal);
}

void RegisterPropertyGridItems::Add(wxPropertyGridPage* page)
{
    page->Append(main);
    page->Collapse(wxPGPropArg(name));
}
