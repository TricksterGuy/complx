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
    return 0x10000;
}

/** GetValue
  *
  * Gets a value to place in a cell.
  */
wxString MemoryView::GetValue(int item, int column)
{
    short data = state.mem[item];
    //short data = lc3_mem_read(state, item, true);
    wxString ret;
    unsigned short pc = state.pc;
    std::stringstream string;
    std::bitset<16> b = data;
    std::string instruction;

    switch(column)
    {
        case MemoryAddress:
            ret = wxString::Format(_("%04X:"), (unsigned short)item);
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
            ret = wxString::FromUTF8(lc3_sym_rev_lookup(state, item).c_str());
            break;
        case MemoryInstruction:
            // Change the pc temporarily...
            state.pc = (unsigned short) (item + 1);
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
void MemoryView::SetValue(int row, int col, const wxString &value)
{
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

            strdata = (std::string)effvalue.mb_str();

            data = (int)strtol(strdata.c_str(), &errstr, 16);
            if (*errstr) return;

            lc3_mem_write(state, row, (short) data, true);
            break;
        case MemoryDecimal:
            if (value.StartsWith(_("#")))
                effvalue = value.Mid(1);

            strdata = (std::string)effvalue.mb_str();
            data = (int)strtol(strdata.c_str(), &errstr, 10);
            if (*errstr) return;

            lc3_mem_write(state, row, (short) data, true);
            break;
        case MemoryLabel:
            strdata = lc3_sym_rev_lookup(state, row);
            newsym = (std::string)value.mb_str();
            if (!strdata.empty()) lc3_sym_delete(state, strdata);
            data = lc3_sym_lookup(state, newsym);
            if (data == -1)
                lc3_sym_add(state, newsym, row);
            else
                wxMessageBox(wxString::Format("BAD STUDENT! The symbol %s already exists at address 0x%04x",
                                              newsym, data), _("ERROR"));
            break;
        case MemoryBinary:
            if (value.StartsWith(_("0b")))
                effvalue = value.Mid(2);

            strdata = (std::string)effvalue.mb_str();
            data = (int)strtol(strdata.c_str(), &errstr, 2);
            if (*errstr) return;

            lc3_mem_write(state, row, (short) data, true);
            break;
        case MemoryInstruction:
            try
            {
                strdata = (std::string)effvalue.mb_str();
                if (!strdata.empty())
                    lc3_mem_write(state, row, (short) lc3_assemble_one(state, row, strdata), true);
                else
                    lc3_mem_write(state, row, 0, true);
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
long MemoryView::GetValueAsLong(int row, int col)
{
    return state.mem[row];
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
