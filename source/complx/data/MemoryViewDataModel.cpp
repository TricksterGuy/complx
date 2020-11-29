#include "MemoryViewDataModel.hpp"
#include "MemoryViewInfoState.hpp"
#include "Lc3BinaryDisplayData.hpp"
#include "../util/ValidationHelper.hpp"

#include <logging.hpp>

MemoryViewDataModel::MemoryViewDataModel(std::reference_wrapper<lc3_state>state, unsigned int _disassemble_level) :
    wxDataViewVirtualListModel(0x10000), state_ref(state), disassemble_level(_disassemble_level)
{
    EventLog l(__func__);
}

wxString MemoryViewDataModel::GetColumnType(unsigned int col) const
{
    wxString ret;
    switch(col)
    {
        case MemoryInfo:
            // Technically a MemoryViewInfoState
            ret = "long";
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
            ret = "Lc3BinaryDisplayData";
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

    unsigned short addr = row; //ViewToAddress(row;
    short data = state.mem[addr];
    long info = 0;

    switch(column)
    {
        case MemoryInfo:
            if (state.pc == addr)
                info |= DRAW_PC;
            if  (state.breakpoints.find(addr) != state.breakpoints.end())
            {
                info |= DRAW_BREAKPOINT;
                if (!state.breakpoints[addr].enabled)
                    info |= BREAKPOINT_DISABLED;
            }
            if  (state.blackboxes.find(addr) != state.blackboxes.end())
            {
                info |= DRAW_BLACKBOX;
                if (!state.blackboxes[addr].enabled)
                    info |= BLACKBOX_DISABLED;
            }
            if (state.mem_watchpoints.find(addr) != state.mem_watchpoints.end())
            {
                info |= DRAW_WATCHPOINT;
                if (!state.mem_watchpoints[addr].enabled)
                    info |= WATCHPOINT_DISABLED;
            }
            if (state.halted)
                info |= IS_HALTED;
            variant = info;
            return;
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
            ret = lc3_disassemble(state, data, addr + 1, disassemble_level);
            break;
        case MemoryBinary:
            variant << ConstructBinaryDisplayData(static_cast<unsigned short>(data), state.instructionPlugin);
            return;
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

bool MemoryViewDataModel::SetValueByRow(const wxVariant& variant, unsigned int row, unsigned int column)
{
    EventLog l(__func__);
    lc3_state& state = state_ref.get();

    wxASSERT(row <= 0xFFFF);

    wxString value;
    long num;
    Lc3BinaryDisplayData binary_data;

    switch(column)
    {
        case MemoryHexadecimal:
            value = variant.GetString();
            if (!ValidateHexValue(value))
            {
                WarnLog("Invalid hex value given %s.", static_cast<const char*>(value));
                return false;
            }
            num = ParseValueOrDie(value);
            break;
        case MemoryDecimal:
            value = variant.GetString();
            if (!ValidateDecimalValue(value))
            {
                WarnLog("Invalid decimal value given %s.", static_cast<const char*>(value));
                return false;
            }
            num = ParseValueOrDie(value);
            break;
        case MemoryInstruction:
            value = variant.GetString();
            if (value.empty())
                return false;
            try
            {
                num = lc3_assemble_one(state, row, value.ToStdString());
            }
            catch (const LC3AssembleException& e)
            {
                WarnLog("Assembling instruction %s failed. Reason: %s", static_cast<const char*>(value), e.what().c_str());
                return false;
            }
            catch (const std::vector<LC3AssembleException>& vec)
            {
                WarnLog("Assembling instruction %s failed. Reasons below.\n-----", static_cast<const char*>(value));

                for (const auto& ex : vec)
                    WarnLog("%s", ex.what().c_str());

                return false;
            }
            break;
        case MemoryBinary:
            binary_data << variant;
            num = binary_data.instruction;
            break;
        default:
            return false;
    }

    InfoLog("Setting x%04x to %5d|x%04x", row, static_cast<short>(num), static_cast<unsigned short>(num));
    lc3_mem_write(state, row, static_cast<short>(num), true);
    return true;
}

void MemoryViewDataModel::UpdateRef(std::reference_wrapper<lc3_state> new_value)
{
    state_ref = new_value;
    // Do not call Reset here. It results in clear lag in updating the values.
    // Instead call Refresh on each view.
}
