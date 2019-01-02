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
    DoRender(*dc, rect);
    return true;
}

void MemoryViewInfoDataRenderer::DoRender(wxDC& dc, wxRect rect) const
{
    // (size / 2, 0, size / 2, size)
    if (state & DRAW_PC)
    {
        wxPoint triangle[3] = {
            {32 - 14, 3},
            {32 - 14, 17},
            {32 , 10}
        };
        triangle[0] += rect.GetTopLeft();
        triangle[1] += rect.GetTopLeft();
        triangle[2] += rect.GetTopLeft();

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
    return wxSize(32, -1);
}
