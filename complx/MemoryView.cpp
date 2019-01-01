#include "MemoryView.hpp"
#include "data/MemoryViewDataModel.hpp"
#include "data/MemoryViewBinaryDataRenderer.hpp"

#include <wx/settings.h>

#include "logger.hpp"

MemoryView::MemoryView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
    wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
    EventLog l(__func__);

    int binary_char_size = GetTextExtent("0").GetWidth() * 16 + 8;
    const unsigned int cell_flags = wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE;

    AppendBitmapColumn("",          MemoryInfo,         wxDATAVIEW_CELL_INERT,      32, wxALIGN_RIGHT,  0);
    AppendTextColumn("Address",     MemoryAddress,      wxDATAVIEW_CELL_INERT,      -1, wxALIGN_RIGHT,  wxDATAVIEW_COL_RESIZABLE);
    AppendTextColumn("Hexadecimal", MemoryHexadecimal,  wxDATAVIEW_CELL_EDITABLE,   -1, wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Decimal",     MemoryDecimal,      wxDATAVIEW_CELL_EDITABLE,   -1, wxALIGN_LEFT,   cell_flags);

    AppendColumn(new wxDataViewColumn("Binary",         new MemoryViewBinaryDataRenderer(), MemoryBinary,           binary_char_size,       wxALIGN_LEFT,   cell_flags));

    AppendTextColumn("Label",           MemoryLabel,          wxDATAVIEW_CELL_EDITABLE,   -1, wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Instruction",     MemoryInstruction,    wxDATAVIEW_CELL_EDITABLE,   -1, wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Comment",         MemoryComment,        wxDATAVIEW_CELL_INERT,      -1, wxALIGN_LEFT,   cell_flags);
}
