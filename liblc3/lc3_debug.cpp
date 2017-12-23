#include "lc3_debug.hpp"
#include "lc3_symbol.hpp"
#include "lc3_execute.hpp"
#include "lc3_expressions.hpp"
#include <cassert>

bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name, int num_params, const std::vector<std::string>& params);

bool lc3_add_break(lc3_state& state, const std::string& symbol, const std::string& label, const std::string& condition, int times)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_add_break(state, addr, label, condition, times);
}

bool lc3_add_break(lc3_state& state, unsigned short addr, const std::string& label, const std::string& condition, int times)
{
    if (state.breakpoints.find(addr) != state.breakpoints.end()) return true;

    lc3_breakpoint_info info;
    info.enabled = true;
    info.addr = addr;
    info.max_hits = times;
    info.hit_count = 0;
    info.label = label;
    info.condition = condition;

    state.breakpoints[addr] = info;

    return false;
}

bool lc3_add_blackbox(lc3_state& state, const std::string& symbol, const std::string& label, const std::string& condition)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_add_blackbox(state, addr, label, condition);
}

bool lc3_add_blackbox(lc3_state& state, unsigned short addr, const std::string& label, const std::string& condition)
{
    if (state.blackboxes.find(addr) != state.blackboxes.end()) return true;

    lc3_blackbox_info info;
    info.enabled = true;
    info.addr = addr;
    info.hit_count = 0;
    info.label = label;
    info.condition = condition;

    state.blackboxes[addr] = info;

    return false;
}

bool lc3_add_watch(lc3_state& state, const std::string& symbol, const std::string& condition, const std::string& label, int times)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_add_watch(state, false, addr, condition, label, times);
}

bool lc3_add_watch(lc3_state& state, bool is_reg, unsigned short data, const std::string& condition, const std::string& label, int times)
{
    if (is_reg)
    {
        assert(data <= 7);
        if (state.reg_watchpoints.find(data) != state.reg_watchpoints.end()) return true;
    }
    else
    {
        assert(data <= 0xFFFF);
        if (state.mem_watchpoints.find(data) != state.mem_watchpoints.end()) return true;
    }

    lc3_watchpoint_info info;
    info.enabled = true;
    info.is_reg = is_reg;
    info.data = data;
    info.max_hits = times;
    info.hit_count = 0;
    info.label = label;
    info.condition = condition;

    if (is_reg)
        state.reg_watchpoints[data] = info;
    else
        state.mem_watchpoints[data] = info;

    return false;
}

bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name, const std::vector<std::string>& params)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;
    return lc3_add_subroutine(state, addr, name, params.size(), params);
}

bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name, int num_params)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;
    std::vector<std::string> params;
    return lc3_add_subroutine(state, addr, name, num_params, params);
}

bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name, const std::vector<std::string>& params)
{
    return lc3_add_subroutine(state, address, name, params.size(), params);
}

bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name, int num_params)
{
    std::vector<std::string> params;
    return lc3_add_subroutine(state, address, name, num_params, params);
}


bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name, int num_params, const std::vector<std::string>& params)
{
    if (state.subroutines.find(address) != state.subroutines.end()) return true;

    lc3_subroutine_info info;
    info.address = address;
    info.name = name;
    info.num_params = num_params;
    info.params = params;
    state.subroutines[address] = info;

    return false;
}

bool lc3_remove_break(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_remove_break(state, addr);
}

bool lc3_remove_break(lc3_state& state, unsigned short addr)
{
    if (state.breakpoints.find(addr) == state.breakpoints.end()) return true;

    state.breakpoints.erase(addr);

    return false;
}

bool lc3_remove_blackbox(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    return lc3_remove_blackbox(state, addr);
}

bool lc3_remove_blackbox(lc3_state& state, unsigned short addr)
{
    if (state.blackboxes.find(addr) == state.blackboxes.end()) return true;

    state.blackboxes.erase(addr);

    return false;
}

bool lc3_remove_watch(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    if (addr == -1) return true;

    lc3_remove_watch(state, false, addr);
    return false;
}

bool lc3_remove_watch(lc3_state& state, bool is_reg, unsigned short data)
{
    if (is_reg)
    {
        if (state.reg_watchpoints.find(data) == state.reg_watchpoints.end()) return true;
        state.reg_watchpoints.erase(data);
    }
    else
    {
        if (state.mem_watchpoints.find(data) == state.mem_watchpoints.end()) return true;
        state.mem_watchpoints.erase(data);
    }

    return false;
}

bool lc3_break_test(lc3_state& state, const lc3_state_change* changes)
{
    // Test for breakpoints
    if (state.breakpoints.find(state.pc) != state.breakpoints.end())
    {
        lc3_breakpoint_info& info = state.breakpoints[state.pc];
        if (info.enabled)
        {
            int boolean = 0;
            if (lc3_calculate(state, info.condition, boolean))
            {
                // Expression has an error...
                // TODO Restrict condition to 128 chars
                char buf[256];
                sprintf(buf, "Breakpoint: %s Error in Expression: %s", info.label.c_str(), info.condition.c_str());
                lc3_warning(state, buf);
            }
            else if (boolean)
            {
                // Halt
                state.halted = 1;
                info.hit_count++;
                // If we are concerned about the number of times and we have surpassed the max.
                if (info.max_hits >= 0 && info.hit_count >= info.max_hits)
                    // Time to delete
                    lc3_remove_break(state, state.pc);
            }
        }
    }

    // Test for watchpoints
    bool r7changed = state.regs[7] != changes->r7;
    // Have we modified a register or a memory address?
    if (changes->changes == LC3_REGISTER_CHANGE)
    {
        int reg = changes->location;
        if (state.reg_watchpoints.find(reg) != state.reg_watchpoints.end())
        {
            lc3_watchpoint_info& info = state.reg_watchpoints[reg];
            if (info.enabled)
            {
                int boolean = 0;
                if (lc3_calculate(state, info.condition, boolean))
                {
                    // Expression has an error...
                    // TODO Restrict condition to 128 chars and label to 32 chars
                    char buf[256];
                    sprintf(buf, "Watchpoint: %s Error in Expression: %s", info.label.c_str(), info.condition.c_str());
                    lc3_warning(state, buf);
                }
                else if (boolean)
                {
                    // Halt
                    state.halted = 1;
                    info.hit_count++;
                    // If we are concerned about the number of times and we have surpassed the max.
                    if (info.max_hits >= 0 && info.hit_count >= info.max_hits)
                        // Time to delete
                        lc3_remove_watch(state, true, reg);
                }
            }
        }
    }
    else if (changes->changes == LC3_MEMORY_CHANGE)
    {
        int mem = changes->location;
        if (state.mem_watchpoints.find(mem) != state.mem_watchpoints.end())
        {
            lc3_watchpoint_info& info = state.mem_watchpoints[mem];
            if (info.enabled)
            {
                int boolean = 0;
                if (lc3_calculate(state, info.condition, boolean))
                {
                    // Expression has an error...
                    // TODO Restrict condition to 128 chars and label to 32 chars
                    char buf[256];
                    sprintf(buf, "Watchpoint: %s Error in Expression: %s", info.label.c_str(), info.condition.c_str());
                    lc3_warning(state, buf);
                }
                else if (boolean)
                {
                    // Halt
                    state.halted = 1;
                    info.hit_count++;
                    // If we are concerned about the number of times and we have surpassed the max.
                    if (info.max_hits >= 0 && info.hit_count >= info.max_hits)
                        // Time to delete
                        lc3_remove_watch(state, false, mem);
                }
            }
        }
    }

    // Also handle r7 changes.
    if (r7changed)
    {
        int reg = 7;
        if (state.reg_watchpoints.find(reg) != state.reg_watchpoints.end())
        {
            lc3_watchpoint_info& info = state.reg_watchpoints[reg];
            if (info.enabled)
            {
                int boolean = 0;
                if (lc3_calculate(state, info.condition, boolean))
                {
                    // Expression has an error...
                    // TODO Restrict condition to 128 chars and label to 32 chars
                    char buf[256];
                    sprintf(buf, "Watchpoint: %s Error in Expression: %s", info.label.c_str(), info.condition.c_str());
                    lc3_warning(state, buf);
                }
                else if (boolean)
                {
                    // Halt
                    state.halted = 1;
                    info.hit_count++;
                    // If we are concerned about the number of times and we have surpassed the max.
                    if (info.max_hits >= 0 && info.hit_count >= info.max_hits)
                        // Time to delete
                        lc3_remove_watch(state, true, reg);
                }
            }
        }
    }

    return state.halted;
}

bool lc3_blackbox_test(lc3_state& state)
{
    bool blackboxing = false;
    // Test for blackboxes
    if (state.blackboxes.find(state.pc) != state.blackboxes.end())
    {
        lc3_blackbox_info& info = state.blackboxes[state.pc];
        if (info.enabled)
        {
            int boolean = 0;
            if (lc3_calculate(state, info.condition, boolean))
            {
                // Expression has an error...
                // TODO Restrict condition to 128 chars
                char buf[256];
                sprintf(buf, "Blackbox: %s Error in Expression: %s", info.label.c_str(), info.condition.c_str());
                lc3_warning(state, buf);
            }
            else if (boolean)
            {
                // Signal blackbox
                blackboxing = true;
                info.hit_count++;
            }
        }
    }

    return blackboxing;
}
