#ifndef MEMORY_VIEW_BINARY_DATA_RENDERER_HPP
#define MEMORY_VIEW_BINARY_DATA_RENDERER_HPP

#include <wx/dataview.h>

class MemoryViewBinaryDataRenderer : public wxDataViewCustomRenderer
{
public:
    MemoryViewBinaryDataRenderer();
    ~MemoryViewBinaryDataRenderer() {}

    bool SetValue(const wxVariant& value) override;
    bool GetValue(wxVariant& value) const override;

    bool Render(wxRect rect, wxDC* dc, int state) override;
    wxSize GetSize() const override;

    // in-place editing
    bool HasEditorCtrl() const override;
    wxWindow* CreateEditorCtrl(wxWindow* parent, wxRect labelRect, const wxVariant& value) override;
    bool GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value) override;

private:
    wxString FormBinary(unsigned short value);
    void InstructionColor(wxDC& dc, const wxString& binary, wxRect rect) const;

    long value;
};


#endif

