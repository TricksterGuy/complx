#include "MemoryView.hpp"
#include "data/MemoryViewDataModel.hpp"

MemoryView::MemoryView(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name) :
    wxDataViewCtrl(parent, id, pos, size, style, validator, name)
{
    AppendColumn(new wxDataViewColumn("",               new wxDataViewTextRenderer(), MemoryInfo,           32));
    AppendColumn(new wxDataViewColumn("Address",        new wxDataViewTextRenderer(), MemoryAddress));
    AppendColumn(new wxDataViewColumn("Hexadecimal",    new wxDataViewTextRenderer(), MemoryHexadecimal));
    AppendColumn(new wxDataViewColumn("Decimal",        new wxDataViewTextRenderer(), MemoryDecimal));
    AppendColumn(new wxDataViewColumn("Binary",         new wxDataViewTextRenderer(), MemoryBinary,         128));
    AppendColumn(new wxDataViewColumn("Label",          new wxDataViewTextRenderer(), MemoryLabel));
    AppendColumn(new wxDataViewColumn("Instruction",    new wxDataViewTextRenderer(), MemoryInstruction));
    AppendColumn(new wxDataViewColumn("Comment",        new wxDataViewTextRenderer(), MemoryComment));
}
