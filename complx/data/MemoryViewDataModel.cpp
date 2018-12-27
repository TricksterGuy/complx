#include "MemoryViewDataModel.hpp"
#include <sstream>
#include <bitset>

wxString MemoryViewDataModel::GetColumnType(unsigned int col) const
{
    wxString ret;
    switch(col)
    {
        case MemoryInfo:
            ret = "null";
            break;
        case MemoryAddress:
            ret = "string";
            break;
        case MemoryHexadecimal:
            ret = "string";
            break;
        case MemoryDecimal:
            ret = "string";
            break;
        case MemoryBinary:
            ret = "string";
            break;
        case MemoryLabel:
            ret = "string";
            break;
        case MemoryInstruction:
            ret = "string";
            break;
        case MemoryComment:
            ret = "string";
            break;
        default:
            ret = "null";
    }
    return ret;
}

void MemoryViewDataModel::GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const
{
    //unsigned short addr = ViewToAddress(item);
    //short data = state.mem[addr];

    unsigned short addr = row;
    unsigned short data = row;
    wxString ret = wxEmptyString;
    std::stringstream binary;
    std::bitset<16> b = data;
    std::string instruction;

    switch(col)
    {
        case MemoryAddress:
            ret = wxString::Format(_("%04X:"), addr);
            break;
        case MemoryHexadecimal:
            ret = wxString::Format(_("x%04X"), (unsigned short)data);
            break;
        case MemoryDecimal:
            ret = wxString::Format(_("%d"), (short)data);
            break;
        case MemoryLabel:
            ret = "don't label me";//wxString::FromUTF8(lc3_sym_rev_lookup(state, addr).c_str());
            break;
        case MemoryInstruction:
            ret = "ADD R0, R0, R0";
            break;
        case MemoryBinary:
            binary << b;
            ret = wxString::FromUTF8(binary.str().c_str());
            break;
        case MemoryComment:
            ret = "Yeah test";
            break;
        default:
            ret = wxEmptyString;
    }

    variant = ret;
}

bool MemoryViewDataModel::SetValueByRow(const wxVariant& variant, unsigned int row, unsigned int col)
{
    return false;
}
