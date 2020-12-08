#ifndef MEMORY_VIEW_BINARY_DATA_RENDERER_HPP
#define MEMORY_VIEW_BINARY_DATA_RENDERER_HPP

#include <wx/dataview.h>

#include "Lc3BinaryDisplayData.hpp"

class MemoryViewBinaryDataRenderer : public wxDataViewCustomRenderer
{
public:
    MemoryViewBinaryDataRenderer();
    ~MemoryViewBinaryDataRenderer() {}

    bool SetValue(const wxVariant& variant) override;
    bool GetValue(wxVariant& variant) const override;

    bool Render(wxRect rect, wxDC* dc, int state) override;
    wxSize GetSize() const override;

    // in-place editing
    bool HasEditorCtrl() const override;
    wxWindow* CreateEditorCtrl(wxWindow* parent, wxRect labelRect, const wxVariant& variant) override;
    bool GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value) override;

private:
    void InstructionColor(wxDC& dc, wxRect rect) const;
    Lc3BinaryDisplayData value;
};


#endif

