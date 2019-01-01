#include "MemoryViewInfoDataRenderer.hpp"
#include "MemoryViewInfoState.hpp"

#include <wx/dc.h>

MemoryViewInfoDataRenderer::MemoryViewInfoDataRenderer() :
    wxDataViewCustomRenderer("long", wxDATAVIEW_CELL_INERT)
{

}

bool MemoryViewInfoDataRenderer::SetValue(const wxVariant& variant)
{
    state = variant.GetLong();
    return true;
}

bool MemoryViewInfoDataRenderer::GetValue(wxVariant& variant) const
{
    variant = state;
    return true;
}

bool MemoryViewInfoDataRenderer::Render(wxRect rect, wxDC* dc, int state)
{
    wxCoord ox, oy;
    dc->GetDeviceOrigin(&ox, &oy);
    dc->SetDeviceOrigin(rect.GetLeft(), rect.GetTop());
    DoRender(*dc, rect.GetSize());
    dc->SetDeviceOrigin(ox, oy);

    return true;
}

void MemoryViewInfoDataRenderer::DoRender(wxDC& dc, wxSize size) const
{
    // (size / 2, 0, size / 2, size)
    if (state & DRAW_PC)
    {
        const wxPoint triangle[3] = {
            {16, 8},
            {16, 24},
            {32, 16}
        };

        dc.SetPen(*wxBLACK_PEN);
        if (state & IS_HALTED)
            dc.SetBrush(*wxGREY_BRUSH);
        else
            dc.SetBrush(wxBrush(wxColor(48, 176, 112)));

        dc.DrawPolygon(3, triangle);
    }


    if (state & DRAW_BREAKPOINT)
    {

    }
}

wxSize MemoryViewInfoDataRenderer::GetSize() const
{
    return wxSize(32, 32);
}
