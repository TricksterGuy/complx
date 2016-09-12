#ifndef EDITORPANEL_HPP
#define EDITORPANEL_HPP

#include <wx/docview.h>
#include <wx/panel.h>
#include "EditorPanelDecl.hpp"

class EditorPanel : public EditorPanelDecl
{
    public:
        EditorPanel(wxWindow* parent) : EditorPanelDecl(parent) {}
        ~EditorPanel() {}
        void LoadFile(const wxString& file);
        void SaveFile(const wxString& file);
        void Clear();
};


#endif
