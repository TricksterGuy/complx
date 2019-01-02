#include "MemoryViewBinaryDataRenderer.hpp"
#include "../util/GuiConstants.hpp"

#include <bitset>
#include <sstream>

#include <lc3_all.hpp>

#include <wx/dc.h>
#include <wx/debug.h>
#include <wx/settings.h>
#include <wx/valtext.h>

enum Colorings
{
    BR_TYPE,
    JSR_TYPE,
    JUMP_REG_TYPE,
    TRAP_TYPE,
    ARITH_IMM_TYPE,
    ARITH_REG_TYPE,
    NOT_TYPE,
    PCOFFSET_TYPE,
    BASE_OFFSET_TYPE,
    NULL_TYPE,
    COLORINGS_SIZE
};

RLEColorEntry defaultColoring = {"UNUSED_BITS_COLOR", 0, 0, 0, 12};
RLEColorEntry opcodeColoring = {"OPCODE_COLOR", 0, 96, 96, 4};

RLEColorEntry instrColorings[COLORINGS_SIZE][4] =
{
    {{"BRN_COLOR", 160, 64, 64, 1},     {"BRZ_COLOR", 64, 64, 160, 1},      {"BRP_COLOR", 64, 160, 64, 1},      {"PCOFFSET_COLOR", 48, 48, 72, 9}   },
    {{"UNUSED_BITS_COLOR", 0, 0, 0, 1}, {"PCOFFSET_COLOR", 48, 48, 72, 11}                                                                          },
    {{"UNUSED_BITS_COLOR", 0, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 6}                                       },
    {{"UNUSED_BITS_COLOR", 0, 0, 0, 4}, {"VECTOR_COLOR", 48, 48, 72, 8}                                                                             },
    {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 1},  {"IMM_COLOR", 0, 0, 80, 5}          },
    {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 3},  {"SR_COLOR", 0, 0, 80, 3}           },
    {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 6}                                       },
    {{"DR_COLOR", 96, 0, 0, 3},         {"PCOFFSET_COLOR", 48, 48, 72, 9}                                                                           },
    {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"OFFSET_COLOR", 48, 48, 72, 6}                                         },
    {{"UNUSED_BITS_COLOR", 0, 0, 0, 12}                                                                                                             },
};

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

wxString FormBinary(unsigned short value)
{
    std::stringstream binary;
    std::bitset<16> b = value;
    binary << b;
    return binary.str();
}

}

MemoryViewBinaryDataRenderer::MemoryViewBinaryDataRenderer() :
    wxDataViewCustomRenderer("long", wxDATAVIEW_CELL_EDITABLE)
{

}


bool MemoryViewBinaryDataRenderer::SetValue(const wxVariant& variant)
{
    value = variant.GetLong();
    return true;
}

bool MemoryViewBinaryDataRenderer::GetValue(wxVariant& variant) const
{
    variant = value;
    return true;
}

bool MemoryViewBinaryDataRenderer::Render(wxRect rect, wxDC* dc, int state)
{
    wxString binary_value = FormBinary(static_cast<unsigned short>(value));

    if (state & wxDATAVIEW_CELL_SELECTED)
        dc->DrawText(binary_value, rect.GetTopLeft());
    else
        InstructionColor(*dc, binary_value, rect);
    return true;
}

void MemoryViewBinaryDataRenderer::InstructionColor(wxDC& dc, const wxString& binary, wxRect rect) const
{
    const wxCoord char_width = dc.GetTextExtent("0").GetWidth();
    const wxCoord char_height = rect.GetHeight();
    const wxPoint start = rect.GetTopLeft();

    dc.SetPen(*wxTRANSPARENT_PEN);
    dc.SetTextForeground(*wxWHITE);

    unsigned short instr = static_cast<unsigned short>(value);
    int opcode = (instr >> 12) & 0xF;

    // If data just draw it all in black.
    if (instr >> 9 == 0 || (instr & (~0xE00)) == 0)
    {
        dc.SetBrush(GetBrush(defaultColoring));
        dc.DrawRectangle(start, wxSize(char_width * 16, char_height));
        dc.DrawText(binary, start);
        return;
    }

    // Draw Opcode
    dc.SetBrush(GetBrush(opcodeColoring));
    dc.DrawRectangle(start, wxSize(char_width * 4, char_height));
    dc.DrawText(binary.Mid(0, 4), start);

    std::vector<RLEColorEntry> colors;
    switch (opcode)
    {
        case BR_INSTR:
            colors.assign(instrColorings[BR_TYPE], instrColorings[BR_TYPE] + 4);
            break;
        case ADD_INSTR:
        case AND_INSTR:
            if (instr >> 5 & 0x1)
                colors.assign(instrColorings[ARITH_IMM_TYPE], instrColorings[ARITH_IMM_TYPE] + 4);
            else
                colors.assign(instrColorings[ARITH_REG_TYPE], instrColorings[ARITH_REG_TYPE] + 4);
            break;
        case NOT_INSTR:
            colors.assign(instrColorings[NOT_TYPE], instrColorings[NOT_TYPE] + 4);
            break;
        case LD_INSTR:
        case ST_INSTR:
        case LDI_INSTR:
        case STI_INSTR:
        case LEA_INSTR:
            colors.assign(instrColorings[PCOFFSET_TYPE], instrColorings[PCOFFSET_TYPE] + 4);
            break;
        case JSR_INSTR:
            if ((instr >> 11) & 1)
                colors.assign(instrColorings[JSR_TYPE], instrColorings[JSR_TYPE] + 4);
            else
                colors.assign(instrColorings[JUMP_REG_TYPE], instrColorings[JUMP_REG_TYPE] + 4);
            break;
        case LDR_INSTR:
        case STR_INSTR:
            colors.assign(instrColorings[BASE_OFFSET_TYPE], instrColorings[BASE_OFFSET_TYPE] + 4);
            break;
        case JMP_INSTR:
            colors.assign(instrColorings[JUMP_REG_TYPE], instrColorings[JUMP_REG_TYPE] + 4);
            break;
        case TRAP_INSTR:
            colors.assign(instrColorings[TRAP_TYPE], instrColorings[TRAP_TYPE] + 4);
            break;
        case ERROR_INSTR:
            //if (state.instructionPlugin)
                //colors = state.instructionPlugin->GetInstructionColoring(instr);
            //else
                colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);
            break;
        default:
            colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);
            break;
    }

    // Defend against bad instruction plugin that didn't implement GetInstructionColoring correctly.
    if (colors.empty())
        colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);

    unsigned int colored = 4;
    for (const auto& entry : colors)
    {
        dc.SetBrush(GetBrush(entry));

        wxPoint loc = start + wxSize((colored) * char_width, 0);
        wxSize rect_size(entry.length * char_width, char_height);
        dc.DrawRectangle(loc, rect_size);
        dc.DrawText(binary.Mid(colored, entry.length), loc);
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
    wxString binary_value = FormBinary(static_cast<unsigned short>(variant.GetLong()));

    wxTextCtrl* ctrl = new wxTextCtrl(parent, wxID_ANY, binary_value,
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

    value = binary;

    return true;
}
