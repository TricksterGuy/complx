#ifndef EDITORPANEL_HPP
#define EDITORPANEL_HPP

#include <wx/panel.h>
#include "EditorPanelDecl.hpp"

class EditorPanel : public EditorPanelDecl
{
    public:
        EditorPanel(const wxString& file, wxWindow* parent);
        ~EditorPanel();
    private:
        void DoLoadFile(const wxString& file);
};


#endif
