#include "AsmFileDocument.hpp"
#include "EditorPanel.hpp"

IMPLEMENT_DYNAMIC_CLASS(AsmFileDocument, wxDocument)

bool AsmFileDocument::DeleteContents()
{
    m_documentTitle = "Untitled.asm";
    m_documentFile = wxEmptyString;
    filename.Clear();
    EditorPanel* view = dynamic_cast<EditorPanel*>(GetFirstView());
    if (view)
        view->Clear();
    return true;
}

bool AsmFileDocument::DoSaveDocument(const wxString& file)
{
    filename = file;
    m_documentTitle = filename.GetFullName();
    m_documentFile = filename.GetFullPath();
    EditorPanel* view = dynamic_cast<EditorPanel*>(GetFirstView());
    if (view)
        view->SaveFile(m_documentFile);
    return true;
}

bool AsmFileDocument::DoOpenDocument(const wxString& file)
{
    filename = file;
    m_documentTitle = filename.GetFullName();
    m_documentFile = filename.GetFullPath();
    EditorPanel* view = dynamic_cast<EditorPanel*>(GetFirstView());
    if (view)
        view->LoadFile(m_documentFile);
    return true;
}
