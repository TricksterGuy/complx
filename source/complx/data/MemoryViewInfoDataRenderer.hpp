#ifndef MEMORY_VIEW_INFO_DATA_RENDERER_HPP
#define MEMORY_VIEW_INFO_DATA_RENDERER_HPP

#include <wx/dataview.h>

class MemoryViewInfoDataRenderer : public wxDataViewCustomRenderer
{
public:
    MemoryViewInfoDataRenderer();
    ~MemoryViewInfoDataRenderer() {}

    bool SetValue(const wxVariant& variant) override;
    bool GetValue(wxVariant& variant) const override;

    bool Render(wxRect rect, wxDC* dc, int state) override;
    wxSize GetSize() const override;

private:
    void DoRender(wxDC& dc, wxRect rect) const;
    // See MemoryViewInfoState enum
    long state;
};


#endif
