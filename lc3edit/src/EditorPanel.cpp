#include "EditorPanel.hpp"

void EditorPanel::LoadFile(const wxString& file)
{
    editor->LoadFile(file);
}

void EditorPanel::SaveFile(const wxString& file)
{
    editor->SaveFile(file);
}

void EditorPanel::Clear()
{
    editor->ClearAll();
}
