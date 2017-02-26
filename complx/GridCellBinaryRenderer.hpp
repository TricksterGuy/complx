#ifndef GRID_CELL_BINARY_RENDERER_HPP
#define GRID_CELL_BINARY_RENDERER_HPP

#include <wx/dc.h>
#include <wx/gdicmn.h>
#include <wx/grid.h>

class GridCellBinaryRenderer : public wxGridCellRenderer
{
public:
    GridCellBinaryRenderer() {}
    void Draw(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &rect, int row, int col, bool isSelected);
    wxSize GetBestSize(wxGrid& grid, wxGridCellAttr& attr, wxDC& dc, int row, int col);
    wxGridCellRenderer* Clone() const;
    void InstructionColor(wxGrid &grid, wxGridCellAttr &attr, wxDC &dc, const wxRect &srect, int item, int column, bool isSelected);
};

#endif
