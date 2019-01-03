#include "MemoryView.hpp"
#include "data/MemoryViewDataModel.hpp"
#include "data/MemoryViewBinaryDataRenderer.hpp"
#include "data/MemoryViewInfoDataRenderer.hpp"
#include "util/GuiConstants.hpp"

#include <algorithm>
#include <wx/debug.h>
#include <wx/settings.h>
#include <wx/version.h>

#include "logger.hpp"

MemoryView::MemoryView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
    wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
    EventLog l(__func__);

    const int info_column_width = GuiConstants().GetOffset("INFO_COLUMN_WIDTH");
    const int binary_column_width = GetTextExtent("0").GetWidth() * 16 + GuiConstants().GetOffset("BINARY_COLUMN_ADDITION");

    const unsigned int cell_flags = wxDATAVIEW_COL_RESIZABLE | wxDATAVIEW_COL_REORDERABLE;

    AppendColumn(new wxDataViewColumn("",               new MemoryViewInfoDataRenderer(),   MemoryInfo,     info_column_width,      wxALIGN_NOT,        0));

    AppendTextColumn("Address",         MemoryAddress,          wxDATAVIEW_CELL_INERT,      64,     wxALIGN_RIGHT,  wxDATAVIEW_COL_RESIZABLE);
    AppendTextColumn("Hex",             MemoryHexadecimal,      wxDATAVIEW_CELL_EDITABLE,   44,     wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Decimal",         MemoryDecimal,          wxDATAVIEW_CELL_EDITABLE,   64,     wxALIGN_LEFT,   cell_flags);

    AppendColumn(new wxDataViewColumn("Binary",         new MemoryViewBinaryDataRenderer(), MemoryBinary,   binary_column_width,    wxALIGN_CENTER,     cell_flags));

    AppendTextColumn("Label",           MemoryLabel,            wxDATAVIEW_CELL_INERT,      -1,     wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Instruction",     MemoryInstruction,      wxDATAVIEW_CELL_EDITABLE,   128,    wxALIGN_LEFT,   cell_flags);
    AppendTextColumn("Comment",         MemoryComment,          wxDATAVIEW_CELL_INERT,      -1,     wxALIGN_LEFT,   cell_flags);
}

void MemoryView::ScrollTo(unsigned short address)
{
    InfoLog("Scrolling Memory to x%04x", address);

    auto* model = GetModel();
    auto* vlmodel = dynamic_cast<wxDataViewVirtualListModel*>(model);

    wxASSERT(vlmodel);

#ifdef _WIN32
    /// TODO Delete this section when wx 3.2 is a thing and GetCountPerPage can be used, odd numbered minor versions are experimental releases.
#if wxCHECK_VERSION(3, 1, 0)
    // Apparently Windows uses the Generic wxDataViewCtrl whose implementation is to put the VisibleItem at the bottom.
    address += GetCountPerPage() - 1;
#else
    // GetCountPerPage is not available which makes fixing the above issue much easier...
    // There's also no GetRowHeight so approximate it.
    wxSize size = GetClientSize();
    address += size.y / 23 - 1;
#endif
#endif

    auto item = vlmodel->GetItem(address);

    EnsureVisible(item);
}
