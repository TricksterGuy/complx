#include "EditorPanel.hpp"
#include <wx/sizer.h>

/**
  *
  * Constructor
  */
EditorPanel::EditorPanel(const wxString& file, wxWindow* parent) : EditorPanelDecl(parent)
{
	DoLoadFile(file);
}

/**
  *
  * Destructor
  */
EditorPanel::~EditorPanel()
{

}

/**
  *
  *
  */
void EditorPanel::DoLoadFile(const wxString& file)
{
    editor->LoadFile(file);
}
