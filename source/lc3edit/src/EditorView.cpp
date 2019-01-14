#include "EditorView.hpp"

IMPLEMENT_DYNAMIC_CLASS(EditorView, wxView)

void EditorView::OnInit(wxFlatNotebook* n)
{
    notebook = n;
    wxString title = "Untitled.asm";
    wxDocument* doc = GetDocument();
    panel = new EditorPanel(notebook);
    if (doc)
    {
        title = doc->GetTitle();
        panel->LoadFile(doc->GetFilename());
    }
    notebook->AddPage(panel, title, false);
}

void EditorView::LoadFile(const wxString& file)
{
    if (panel)
        panel->LoadFile(file);
}

void EditorView::SaveFile(const wxString& file)
{
    if (panel)
        panel->SaveFile(file);
}

void EditorView::Clear()
{
    if (panel)
        panel->Clear();
}
