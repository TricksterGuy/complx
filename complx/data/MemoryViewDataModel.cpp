#include "MemoryViewDataModel.hpp"
#include <sstream>
#include <bitset>

MemoryViewDataModel::MemoryViewDataModel(std::reference_wrapper<lc3_state>state, unsigned int _disassemble_level) :
    wxDataViewVirtualListModel(0x10000), state_ref(state), disassemble_level(_disassemble_level)
{

}

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

void MemoryViewDataModel::GetValueByRow(wxVariant& variant, unsigned int row, unsigned int column) const
{
    lc3_state& state = state_ref.get();

    wxString ret = wxEmptyString;

    unsigned short pc = state.pc;
    unsigned short addr = row; //ViewToAddress(row;
    short data = state.mem[addr];

    std::stringstream binary;
    std::bitset<16> b;

    switch(column)
    {
        case MemoryAddress:
            ret = wxString::Format("%04X:", addr);
            break;
        case MemoryHexadecimal:
            ret = wxString::Format("x%04X", static_cast<unsigned short>(data));
            break;
        case MemoryDecimal:
            ret = wxString::Format("%d", data);
            break;
        case MemoryLabel:
            ret = lc3_sym_rev_lookup(state, addr);
            break;
        case MemoryInstruction:
            // Change the pc temporarily...
            state.pc = addr + 1;
            ret = lc3_disassemble(state, data, disassemble_level);
            state.pc = pc;
            break;
        case MemoryBinary:
            b = static_cast<unsigned short>(data);
            binary << b;
            ret = binary.str();
            break;
        case MemoryComment:
            if (state.comments.find(addr) != state.comments.end())
            {
                ret = state.comments[addr];
                ret.Replace("\n", " ", true);
            }
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
