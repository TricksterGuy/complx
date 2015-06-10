#include <string>
#include <sstream>
#include <bitset>
#include <wx/msgdlg.h>
#include "MemoryView.hpp"
#include "BreakpointDialog.hpp"
#include "lc3_assemble.hpp"

/** MemoryView
  *
  * Constructor
  */
MemoryView::MemoryView()
{
    unsigned_mode = false;
    disassemble_level = 2;
}

/** ~MemoryView
  *
  * Destructor
  */
MemoryView::~MemoryView()
{
}

/** ShowAllAddresses
  *
  * Modifies memory view to show all addresses.
  */
void MemoryView::ShowAllAddresses()
{
    defaultView = ViewAction::SHOW;
    ranges.clear();
    ExpandRanges();
}

/** ModifyAddresses
  *
  * Modifies the visibility of the addresses passed in
  */
void MemoryView::ModifyAddresses(const std::vector<ViewRange>& addresses, bool expand_too)
{
    for (const auto& view_range : addresses)
    {
        ranges.insert(view_range);
    }
    if (expand_too) ExpandRanges();
}

/** SetDefaultVisibility
  *
  * Sets the default visibility of an address.
  */
void MemoryView::SetDefaultVisibility(ViewAction action)
{
    defaultView = action;
}

/** GetNumberCols
  *
  * Gets the number of columns (required function)
  */
int MemoryView::GetNumberCols()
{
    return MemorySize;
}

/** GetNumberRows
  *
  * Gets the number of rows (required function)
  */
int MemoryView::GetNumberRows()
{
    return ranges.empty() ? 0x10000 : viewTable.size();
}

/** GetValue
  *
  * Gets a value to place in a cell.
  */
wxString MemoryView::GetValue(int item, int column)
{
    unsigned short addr = ranges.empty() ? item : viewTable[item].address;
    short data = state.mem[addr];

    wxString ret;
    unsigned short pc = state.pc;
    std::stringstream string;
    std::bitset<16> b = data;
    std::string instruction;

    switch(column)
    {
        case MemoryAddress:
            ret = wxString::Format(_("%04X:"), addr);
            break;
        case MemoryHexadecimal:
            ret = wxString::Format(_("x%04X"), (unsigned short)data);
            break;
        case MemoryDecimal:
            if (unsigned_mode)
                ret = wxString::Format(_("%d"), (unsigned short)data);
            else
                ret = wxString::Format(_("%d"), (short)data);
            break;
        case MemoryLabel:
            ret = wxString::FromUTF8(lc3_sym_rev_lookup(state, addr).c_str());
            break;
        case MemoryInstruction:
            // Change the pc temporarily...
            state.pc = (unsigned short) (addr + 1);
            if (disassemble_level == 0)
                instruction = lc3_basic_disassemble(state, data);
            else if (disassemble_level == 1)
                instruction = lc3_disassemble(state, data);
            else
                instruction = lc3_smart_disassemble(state, data);
            ret = wxString::FromUTF8(instruction.c_str());
            state.pc = pc;
            break;
        case MemoryBinary:
            string << b;
            ret = wxString::FromUTF8(string.str().c_str());
            break;
        default:
            ret = wxEmptyString;
    }
    return ret;
}

/** SetValue
  *
  * Sets a value at a cell
  */
void MemoryView::SetValue(int item, int col, const wxString &value)
{
    unsigned short addr = ranges.empty() ? item : viewTable[item].address;
    int data = 0;
    wxString effvalue = value;
    std::string strdata;
    std::string newsym;
    char *errstr;

    switch(col)
    {
        case MemoryHexadecimal:
            if (value.StartsWith(_("x")) || value.StartsWith(_("X")))
                effvalue = _("0") + value;
            else if (!(value.StartsWith(_("0x")) && value.StartsWith(_("0X"))))
                effvalue = _("0x") + value;

            strdata = effvalue.ToStdString();

            data = (int)strtol(strdata.c_str(), &errstr, 16);
            if (*errstr) return;

            lc3_mem_write(state, addr, (short)data, true);
            break;
        case MemoryDecimal:
            if (value.StartsWith(_("#")))
                effvalue = value.Mid(1);

            strdata = effvalue.ToStdString();
            data = (int)strtol(strdata.c_str(), &errstr, 10);
            if (*errstr) return;

            lc3_mem_write(state, addr, (short)data, true);
            break;
        case MemoryLabel:
            strdata = lc3_sym_rev_lookup(state, addr);
            newsym = value.ToStdString();
            if (!strdata.empty()) lc3_sym_delete(state, strdata);
            data = lc3_sym_lookup(state, newsym);
            if (data == -1)
                lc3_sym_add(state, newsym, addr);
            else
                wxMessageBox(wxString::Format("BAD STUDENT! The symbol %s already exists at address 0x%04x",
                                              newsym, data), _("ERROR"));
            break;
        case MemoryBinary:
            if (value.StartsWith(_("0b")))
                effvalue = value.Mid(2);

            strdata = effvalue.ToStdString();
            data = (int)strtol(strdata.c_str(), &errstr, 2);
            if (*errstr) return;

            lc3_mem_write(state, addr, (short)data, true);
            break;
        case MemoryInstruction:
            try
            {
                strdata = effvalue.ToStdString();
                if (!strdata.empty())
                    lc3_mem_write(state, addr, (short)lc3_assemble_one(state, addr, strdata), true);
                else
                    lc3_mem_write(state, addr, 0, true);
            }
            catch (LC3AssembleException e)
            {
                effvalue = e.what();
                wxMessageBox(wxString::Format("BAD STUDENT! %s", effvalue), _("Assemble Error"));
            }
            catch (std::vector<LC3AssembleException> e)
            {
                effvalue = e[0].what();
                wxMessageBox(wxString::Format("BAD STUDENT! %s", effvalue), _("Assemble Error"));
            }
            break;
        default:
            break;
    }
}

/** GetColLabelValue
  *
  * Gets a Columns Label String (required function)
  */
wxString MemoryView::GetColLabelValue(int col)
{
    wxString ret;
    switch(col)
    {
        case MemoryInfo:
            ret = wxEmptyString;
            break;
        case MemoryAddress:
            ret = _("Addr");
            break;
        case MemoryHexadecimal:
            ret = _("Hex");
            break;
        case MemoryDecimal:
            ret = _("Decimal");
            break;
        case MemoryBinary:
            ret = _("Binary");
            break;
        case MemoryLabel:
            ret = _("Label");
            break;
        case MemoryInstruction:
            ret = _("Instruction");
            break;
        default:
            ret = wxEmptyString;
            break;
    }
    return ret;
}

/** GetValueAsLong
  *
  * Gets a value as a long
  */
long MemoryView::GetValueAsLong(int item, int col)
{
    unsigned short addr = ranges.empty() ? item : viewTable[item].address;
    return state.mem[addr];
}

/** SetDisassembleLevel
  *
  * Sets the disassemble level
  */
void MemoryView::SetDisassembleLevel(int level)
{
    disassemble_level = level;
}

/** SetUnsignedMode
  *
  * Sets the decimal display mode.
  */
void MemoryView::SetUnsignedMode(bool mode)
{
    unsigned_mode = mode;
}

/** ExpandRanges
  *
  * Expands view table ranges
  */
void MemoryView::ExpandRanges()
{
    viewTable.clear();
    viewTable_rev.clear();
    if (ranges.empty()) return;

    unsigned int old_rows = GetNumberRows();

    std::set<unsigned short> temp_set;
    if (defaultView == ViewAction::SHOW)
    {
        for (unsigned int i = 0; i <= 65535; i++)
            temp_set.insert(i);
    }
    for (const auto& range : ranges)
    {
        if (range.action == ViewAction::HIDE)
        {
            for (unsigned short current = range.start; current <= range.end; current++)
                temp_set.erase(current);
        }
        else
        {
            for (unsigned short current = range.start; current <= range.end; current++)
                temp_set.insert(current);
        }
    }

    int id = 0;
    for (const auto& address : temp_set)
    {
        viewTable.push_back(ViewTableEntry(id, address));
        viewTable_rev[address] = id;
        id++;
    }

    if (GetView())
    {
        unsigned int new_rows = GetNumberRows();
        if (old_rows > new_rows)
        {
            wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_DELETED, 0, old_rows - new_rows);
            GetView()->ProcessTableMessage(msg);
        }
        else if (new_rows > old_rows)
        {
            wxGridTableMessage msg(this, wxGRIDTABLE_NOTIFY_ROWS_APPENDED, new_rows - old_rows);
            GetView()->ProcessTableMessage(msg);
        }
    }
}

int MemoryView::AddressToView(unsigned short address) const
{
    if (viewTable_rev.empty()) return address;
    if (viewTable_rev.find(address) == viewTable_rev.end())
    {
        auto upper = viewTable_rev.upper_bound(address);
        auto lower = upper;
        lower--;
        if (upper == viewTable_rev.end())
        {
            return lower->second;
        }
        if (upper == viewTable_rev.begin())
        {
            return upper->second;
        }
        if (abs(upper->first - address) > abs(lower->first - address))
            return lower->second;
        else
            return upper->second;
    }
    else
    {
        return viewTable_rev.at(address);
    }
}
