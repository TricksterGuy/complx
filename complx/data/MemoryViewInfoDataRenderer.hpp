#ifndef MEMORY_VIEW_INFO_DATA_RENDERER_HPP
#define MEMORY_VIEW_INFO_DATA_RENDERER_HPP

#include <wx/dataview.h>

class MemoryViewInfoDataRenderer : public wxDataViewCustomRenderer
{
public:
    MemoryViewInfoDataRenderer();
    ~MemoryViewInfoDataRenderer() {}

    bool SetValue(const wxVariant& value) override;
    bool GetValue(wxVariant& value) const override;

    bool Render(wxRect rect, wxDC* dc, int state) override;
    wxSize GetSize() const override;

private:
    void DoRender(wxDC& dc, wxSize size) const;
    long state;
};


#endif
