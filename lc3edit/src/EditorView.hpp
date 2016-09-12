#ifndef EDITOR_VIEW_HPP
#define EDITOR_VIEW_HPP

#include <wx/docview.h>
#include "EditorPanel.hpp"
#include <wx/wxFlatNotebook/wxFlatNotebook.h>

class EditorView : public wxView
{
    DECLARE_DYNAMIC_CLASS(EditorView)
public:
    EditorView() : panel(nullptr), notebook(nullptr) {}
    ~EditorView() {}
    void OnInit(wxFlatNotebook* notebook);
    void LoadFile(const wxString& file);
    void SaveFile(const wxString& file);
    void Clear();
    void OnDraw(wxDC* dc) {}
private:
    EditorPanel* panel;
    wxFlatNotebook* notebook;
};

#endif
