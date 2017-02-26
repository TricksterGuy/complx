#ifndef LC3CONSOLE_HPP
#define LC3CONSOLE_HPP

#include <wx/event.h>
#include <wx/textctrl.h>
#include <wx/window.h>
#include <iostream>
#include <sstream>

#include "LC3ConsoleDecl.h"

class LC3Console : public LC3ConsoleDecl
{
public:
    LC3Console(wxWindow* parent);
    ~LC3Console();
    std::stringstream inputStream;
    void OnReadChar();
    void Update();
    void Clear();
    void ClearInput();
private:
    wxStreamToTextRedirector* stotxt;
    //void OnChar(wxKeyEvent& event);
    void OnText(wxCommandEvent& event);
    void OnEnterKey(wxCommandEvent& event);
};

#endif
