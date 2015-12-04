#include "GridCellBinaryRenderer.hpp"
#include <lc3_all.hpp>
#include <wx/settings.h>

extern lc3_state state;

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

RLEColorEntry instrColorings[COLORINGS_SIZE][4] =
{
    {{160, 64, 64, 1}, {64, 64, 160, 1}, {64, 160, 64, 1}, {48, 48, 72, 9}},
    {{0, 0, 0, 1}, {48, 48, 72, 11}},
    {{0, 0, 80, 3}, {0, 0, 0, 9}},
    {{0, 0, 0, 4}, {48, 48, 72, 8}},
    {{96, 0, 0, 3}, {0, 0, 80, 3}, {0, 0, 0, 1}, {0, 0, 80, 5}},
    {{96, 0, 0, 3}, {0, 0, 80, 3}, {0, 0, 0, 3}, {0, 0, 80, 3}},
    {{96, 0, 0, 3}, {0, 0, 80, 3}, {0, 0, 0, 6}},
    {{96, 0, 0, 3}, {48, 48, 72, 9}},
    {{96, 0, 0, 3}, {0, 0, 80, 3}, {48, 48, 72, 6}},
    {{0, 0, 0, 12}},
};

/** GetBestSize
  *
  * Overriden from wxGridCellRenderer
  */
wxSize GridCellBinaryRenderer::GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col)
{
    wxCoord x = 0, y = 0;
    dc.SetFont(attr.GetFont());
    dc.GetTextExtent(grid.GetCellValue(row, col), &x, &y);
    return wxSize(x, y);
}

/** Clone
  *
  * Overriden from wxGridCellRenderer
  */
wxGridCellRenderer* GridCellBinaryRenderer::Clone() const
{
    return new GridCellBinaryRenderer();
}

/** Draw
  *
  * Overriden from wxGridCellStringRenderer
  */
void GridCellBinaryRenderer::Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &srect, int item, int column, bool isSelected)
{
    wxGridCellRenderer::Draw(grid, attr, dc, srect, item, column, isSelected);


    if (!isSelected) InstructionColor(grid, attr, dc, srect, item, column, isSelected);


    dc.SetBackgroundMode( wxTRANSPARENT );

    // different coloured text when the grid is disabled
    if ( grid.IsEnabled() )
    {
        if ( isSelected )
        {
            dc.SetTextBackground( grid.GetSelectionBackground() );
            dc.SetTextForeground( grid.GetSelectionForeground() );
        }
        else
        {
            dc.SetTextBackground( attr.GetBackgroundColour() );
            dc.SetTextForeground( attr.GetTextColour() );
        }
    }
    else
    {
        dc.SetTextBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
        dc.SetTextForeground(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));
    }

    dc.SetFont( attr.GetFont() );

    int hAlign, vAlign;
    wxRect textRect = srect;
    textRect.Inflate(-1);
    attr.GetAlignment(&hAlign, &vAlign);

    grid.DrawTextRectangle(dc, grid.GetCellValue(item, column), textRect, hAlign, vAlign);
}

/** InstructionColor
  *
  * Highlights significant portions of the instruction in binary view
  */
void GridCellBinaryRenderer::InstructionColor(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &srect, int item, int column, bool isSelected)
{
    wxRect rect = srect;
    int width = dc.GetCharWidth(), height = rect.GetHeight();
    rect.Offset(2, 0);

    dc.SetBrush(*wxBLACK);
    dc.DrawRectangle(srect.GetX(), srect.GetY(), srect.GetWidth(), srect.GetHeight());

    unsigned short instr = (unsigned short) grid.GetTable()->GetValueAsLong(item, column);
    int opcode = (instr >> 12) & 0xF;

    if (instr >> 9 == 0 || (instr & (~0xE00)) == 0) return;

    dc.SetBrush(wxColour(0, 96, 96));
    dc.DrawRectangle(rect.GetX() - 2, rect.GetY(), width * 4 + 2, height);

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
            if (state.instructionPlugin)
                colors = state.instructionPlugin->GetInstructionColoring(instr);
            else
                colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);
            break;
        default:
            colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);
            break;
    }

    int colored = 0;
    int colorptr = 0;
    while (colored < 12)
    {
        RLEColorEntry& entry = colors[colorptr];
        dc.SetBrush(wxColour(entry.r, entry.g, entry.b));
        dc.DrawRectangle(rect.GetX() + width * (colored + 4), rect.GetY(), width * entry.length, height);
        colored += entry.length;
        colorptr++;
    }

    //dc.DrawRectangle(rect.GetX() + width * 16, rect.GetY(), rect.GetWidth() - textwidth + 1 - 3, height);
}
