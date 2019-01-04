#include "MemoryViewBinaryDataRenderer.hpp"
#include "../util/GuiConstants.hpp"

#include <lc3_all.hpp>

#include <wx/dc.h>
#include <wx/debug.h>
#include <wx/settings.h>
#include <wx/valtext.h>

namespace {

wxColour GetColor(const RLEColorEntry& rle)
{
    if (GuiConstants().HasColor(rle.name))
        return GuiConstants().GetColor(rle.name);
    else
        return wxColour(rle.r, rle.g, rle.b);
}

wxBrush GetBrush(const RLEColorEntry& rle)
{
    return wxBrush(GetColor(rle));
}

}

MemoryViewBinaryDataRenderer::MemoryViewBinaryDataRenderer() :
    wxDataViewCustomRenderer("Lc3BinaryDisplayData", wxDATAVIEW_CELL_EDITABLE)
{

}


bool MemoryViewBinaryDataRenderer::SetValue(const wxVariant& variant)
{
    value << variant;
    return true;
}

bool MemoryViewBinaryDataRenderer::GetValue(wxVariant& variant) const
{
    variant << value;
    return true;
}

bool MemoryViewBinaryDataRenderer::Render(wxRect rect, wxDC* dc, int state)
{
    if (state & wxDATAVIEW_CELL_SELECTED)
        dc->DrawText(value.binary, rect.GetTopLeft());
    else
        InstructionColor(*dc, rect);
    return true;
}

void MemoryViewBinaryDataRenderer::InstructionColor(wxDC& dc, wxRect rect) const
{
    const wxCoord char_width = dc.GetTextExtent("0").GetWidth();
    const wxCoord char_height = rect.GetHeight();
    const wxPoint start = rect.GetTopLeft();

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetTextForeground(*wxWHITE);

    unsigned int colored = 0;
    for (const auto& entry : value.colors)
    {
        dc.SetBrush(GetBrush(entry));

        wxPoint loc = start + wxSize((colored) * char_width, 0);
        wxSize rect_size(entry.length * char_width, char_height);
        dc.DrawRectangle(loc, rect_size);
        dc.DrawText(value.binary.Mid(colored, entry.length), loc);
        colored += entry.length;
    }
}

wxSize MemoryViewBinaryDataRenderer::GetSize() const
{
    return GetTextExtent("0000000000000000");
}

bool MemoryViewBinaryDataRenderer::HasEditorCtrl() const
{
    return true;
}

wxWindow* MemoryViewBinaryDataRenderer::CreateEditorCtrl(wxWindow* parent, wxRect labelRect, const wxVariant& variant)
{
    Lc3BinaryDisplayData data;
    data << variant;

    wxTextCtrl* ctrl = new wxTextCtrl(parent, wxID_ANY, data.binary,
                                      labelRect.GetPosition(),
                                      labelRect.GetSize(),
                                      wxTE_PROCESS_ENTER);

    wxTextValidator validator(wxFILTER_INCLUDE_CHAR_LIST);
    validator.SetCharIncludes("01");
    ctrl->SetValidator(validator);
    ctrl->SetMaxLength(16);

    // Adjust size of wxTextCtrl editor to fit text, even if it means being
    // wider than the corresponding column (this is how Explorer behaves).
    const int fitting = ctrl->GetSizeFromTextSize(ctrl->GetTextExtent(ctrl->GetValue())).x;
    const int current = ctrl->GetSize().x;
    const int maxwidth = ctrl->GetParent()->GetSize().x - ctrl->GetPosition().x;

    // Adjust size so that it fits all content. Don't change anything if the
    // allocated space is already larger than needed and don't extend wxDVC's
    // boundaries.
    int width = wxMin(wxMax(current, fitting), maxwidth);

    if ( width != current )
        ctrl->SetSize(wxSize(width, -1));

    // select the text in the control an place the cursor at the end
    ctrl->SetInsertionPointEnd();
    ctrl->SelectAll();

    return ctrl;
}

bool MemoryViewBinaryDataRenderer::GetValueFromEditorCtrl(wxWindow* editor, wxVariant& value)
{
    long binary;

    wxTextCtrl* text = dynamic_cast<wxTextCtrl*>(editor);
    wxASSERT(text);

    bool ret = text->GetValue().ToCLong(&binary, 2);
    wxASSERT(ret);

    Lc3BinaryDisplayData data(static_cast<unsigned short>(binary), wxEmptyString, std::list<RLEColorEntry>());

    value << data;

    return true;
}
