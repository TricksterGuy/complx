#include "commands.hpp"
#include <logger.hpp>
#include <bitset>

extern lc3_state state;
std::string current_filename;

#define NUM_LIST_DEFAULT 16

std::string binary_instruction_string(unsigned short data);

void do_run(void)
{
    lc3_run(state);
}

void do_step(unsigned int times)
{
    lc3_run(state, times);
}

void do_back(unsigned int times)
{
    lc3_rewind(state, times);
}

void do_next(unsigned int times)
{
    for (unsigned int i = 0; i < times; i++)
        lc3_next_line(state);
}

void do_prev(unsigned int times)
{
    for (unsigned int i = 0; i < times; i++)
        lc3_prev_line(state);
}

void do_finish(void)
{
    lc3_finish(state);
}

void do_rewind(void)
{
    lc3_rewind(state);
}

void do_tempbreak(unsigned short address)
{
    lc3_add_break(state, address);
}

void do_tempbreak(const std::string& symbol)
{
    lc3_add_break(state, symbol);
}

void do_break(unsigned short address, const std::string& name, const std::string& condition, int times)
{
    lc3_add_break(state, address, name, condition, times);
}

void do_watch(const std::string& symbol, const std::string& condition, const std::string& name, int times)
{
    lc3_add_watch(state, symbol, condition, name, times);
}

void do_watch(unsigned char reg, const std::string& condition, const std::string& name, int times)
{
    lc3_add_watch(state, true, reg, condition, name, times);
}

void do_watch(unsigned short address, const std::string& condition, const std::string& name, int times)
{
    lc3_add_watch(state, false, address, condition, name, times);
}

void do_blackbox(unsigned short address, const std::string& name, const std::string& condition)
{
    lc3_add_blackbox(state, address, name, condition);
}

void do_blackbox(const std::string& symbol, const std::string& name, const std::string& condition)
{
    lc3_add_blackbox(state, symbol, name, condition);
}

void do_undo_stack(unsigned short num)
{
    state.max_stack_size = num;
}

void do_call_stack(unsigned short num)
{
    state.max_call_stack_size = num;
}

void do_delete(const std::string& symbol)
{
    lc3_remove_blackbox(state, symbol);
    lc3_remove_break(state, symbol);
    lc3_remove_watch(state, symbol);
}

void do_delete(unsigned short address)
{
    lc3_remove_blackbox(state, address);
    lc3_remove_break(state, address);
    lc3_remove_watch(state, false, address);
}

void do_delete(unsigned char reg)
{
    lc3_remove_watch(state, true, reg);
}

void do_set(unsigned char reg, short value)
{
    state.regs[reg] = value;
    ///TODO Handle PC,CC
}

void do_set(unsigned char reg, const std::string& expr)
{
    int value_calc;
    if (lc3_calculate(state, expr, value_calc))
        return;
    do_set(reg, (short)value_calc);
}

void do_set(unsigned short address, short value)
{
    state.mem[address] = value;
}

void do_set(unsigned short address, const std::string& expr)
{
    int value_calc;
    if (lc3_calculate(state, expr, value_calc))
        return;
    do_set(address, (short)value_calc);
}

void do_set(const std::string& symbol, short value)
{
    int lookup = lc3_sym_lookup(state, symbol);
    if (lookup == -1)
        return;
    do_set((unsigned short)lookup, value);
}

void do_set(const std::string& symbol, const std::string& expr)
{
    int lookup = lc3_sym_lookup(state, symbol);
    if (lookup == -1)
        return;
    do_set((unsigned short)lookup, expr);
}

void do_input(const std::string& source)
{
    std::istream* newsource = source == "cin" ? &std::cin : new std::ifstream(source.c_str());
    if (!newsource->good())
    {
        if (newsource != &std::cin)
            delete newsource;
        return;
    }

    if (state.input != &std::cin)
        delete state.input;

    state.input = newsource;
}

void do_output(const std::string& sink)
{
    std::ostream* newsink = sink == "cout" ? &std::cout : new std::ofstream(sink.c_str());
    if (!newsink->good())
    {
        if (newsink != &std::cout)
            delete newsink;
        return;
    }

    if (state.output != &std::cout)
        delete state.output;

    state.output = newsink;
}

void do_reset(void)
{
    lc3_init(state);
}

void do_randomize(void)
{
    lc3_init(state, true, true);
}

void do_true_traps(bool set)
{
    lc3_set_true_traps(state, set);
}

void do_interrupt(bool set)
{
    state.interrupt_enabled = set;
}

void do_print(unsigned char reg)
{
    ///TODO better formatting
    short value = state.regs[reg];
    printf("R%d\t%hd\tx%04hx\n", reg, value, value);
}

void do_print(unsigned short address)
{
    short value = state.mem[address];
    printf("MEM[x%04hx]\t%hd\tx%04hx\n", address, value, value);
}

void do_print(const std::string& symbol)
{
    int lookup = lc3_sym_lookup(state, symbol);
    if (lookup == -1)
        return;
    short value = state.mem[(unsigned short)lookup];
    printf("MEM[%s]\t%hd\tx%04hx\n", symbol.c_str(), value, value);
}

void do_list(void)
{
    int size = NUM_LIST_DEFAULT / 2;
    int pc = state.pc;
    unsigned short start, end;
    if (pc - size < 0)
    {
        start = 0;
        end = NUM_LIST_DEFAULT + 1;
    }
    else if (pc + size > 0xffff)
    {
        start = 0xffff - NUM_LIST_DEFAULT - 1;
        end = 0xffff;
    }
    else
    {
        start = pc - size;
        end = pc + size;
    }
    do_list(start, end);
}

void do_list(unsigned short start)
{
    do_list(start, start);
}

void do_list(unsigned short start, unsigned short end, int level)
{
    for (unsigned int i = start; i <= end; i++)
    {
        unsigned short data = state.mem[i];
        std::string output;
        switch(level)
        {
            case LC3_BASIC_DISASSEMBLE:
                output = lc3_basic_disassemble(state, data);
            case LC3_NORMAL_DISASSEMBLE:
                output = lc3_disassemble(state, data);
            case LC3_ADVANCED_DISASSEMBLE:
                output = lc3_smart_disassemble(state, data);
            default:
                return;
        }
        std::string binstr = binary_instruction_string(data);
        const auto& comment = state.comments.find(i);
        printf("x%04hx : %s %s %s\n", i, binstr.c_str(), output.c_str(), comment == state.comments.end() ? "" : comment->second.c_str());
    }
}

void do_dump(unsigned short start)
{
    do_dump(start, start);
}

void do_dump(unsigned short start, unsigned short end)
{
    for (unsigned int i = start; i <= end; i++)
    {
        short data = state.mem[i];
        printf("x%04hx : %hd x%04hx\n", i, data, data);
    }
}

void do_debug_info(void)
{
    for (const auto& addr_break : state.breakpoints)
    {
        const auto& breakp = addr_break.second;
        const std::string symbol = lc3_sym_rev_lookup(state, breakp.addr);
        printf("Breakpoint %s ", breakp.label.c_str());
        if (symbol.empty())
            printf("at x%04hx", breakp.addr);
        else
            printf("at %s", symbol.c_str());
        printf(" - Condition: %s Times: %d Times Triggered: %d\n", breakp.condition.c_str(), breakp.max_hits, breakp.hit_count);
    }
    for (const auto& addr_watch : state.mem_watchpoints)
    {
        const auto& watch = addr_watch.second;
        const std::string symbol = lc3_sym_rev_lookup(state, watch.data);
        printf("Watchpoint %s ", watch.label.c_str());
        if (symbol.empty())
            printf("targeting MEM[x%04hx]", watch.data);
        else
            printf("targeting %s", symbol.c_str());
        printf(" - Condition: %s Times: %d Times Triggered: %d\n", watch.condition.c_str(), watch.max_hits, watch.hit_count);
    }
    for (const auto& reg_watch : state.reg_watchpoints)
    {
        const auto& watch = reg_watch.second;
        printf("Watchpoint %s targeting R%d - Condition: %s Times: %d Times Triggered: %d\n", watch.label.c_str(), watch.data, watch.condition.c_str(), watch.max_hits, watch.hit_count);
    }
    for (const auto& addr_bbox : state.blackboxes)
    {
        const auto& bbox = addr_bbox.second;
        const std::string symbol = lc3_sym_rev_lookup(state, bbox.addr);
        printf("Blackbox %s ", bbox.label.c_str());
        if (symbol.empty())
            printf("at MEM[x%04hx]", bbox.addr);
        else
            printf("at %s", symbol.c_str());
        printf(" - Condition: %s Times Triggered: %d\n", bbox.condition.c_str(), bbox.hit_count);
    }
}

void do_debug_info(const std::string& symbol)
{
    int lookup = lc3_sym_lookup(state, symbol);
    if (lookup == -1)
        return;
    do_debug_info((unsigned short)lookup);
}

void do_debug_info(unsigned short address)
{
    const auto& breakp_it = state.breakpoints.find(address);
    const auto& mwatch_it = state.mem_watchpoints.find(address);
    const auto& rwatch_it = state.reg_watchpoints.find(address);
    const auto& bbox_it = state.blackboxes.find(address);

    if (breakp_it != state.breakpoints.end())
    {
        const auto& breakp = breakp_it->second;
        const std::string symbol = lc3_sym_rev_lookup(state, breakp.addr);
        printf("Breakpoint %s ", breakp.label.c_str());
        if (symbol.empty())
            printf("at x%04hx", breakp.addr);
        else
            printf("at %s", symbol.c_str());
        printf(" - Condition: %s Times: %d Times Triggered: %d\n", breakp.condition.c_str(), breakp.max_hits, breakp.hit_count);
    }
    if (mwatch_it != state.mem_watchpoints.end())
    {
        const auto& watch = mwatch_it->second;
        const std::string symbol = lc3_sym_rev_lookup(state, watch.data);
        printf("Watchpoint %s ", watch.label.c_str());
        if (symbol.empty())
            printf("targeting MEM[x%04hx]", watch.data);
        else
            printf("targeting %s", symbol.c_str());
        printf(" - Condition: %s Times: %d Times Triggered: %d\n", watch.condition.c_str(), watch.max_hits, watch.hit_count);
    }
    if (rwatch_it != state.reg_watchpoints.end())
    {
        const auto& watch = rwatch_it->second;
        printf("Watchpoint %s targeting R%d - Condition: %s Times: %d Times Triggered: %d\n", watch.label.c_str(), watch.data, watch.condition.c_str(), watch.max_hits, watch.hit_count);
    }
    if (bbox_it != state.blackboxes.end())
    {
        const auto& bbox = bbox_it->second;
        const std::string symbol = lc3_sym_rev_lookup(state, bbox.addr);
        printf("Blackbox %s ", bbox.label.c_str());
        if (symbol.empty())
            printf("at MEM[x%04hx]", bbox.addr);
        else
            printf("at %s", symbol.c_str());
        printf(" - Condition: %s Times Triggered: %d\n", bbox.condition.c_str(), bbox.hit_count);
    }
}

void do_info(void)
{
    printf("x%04hx: %s\n", state.pc, lc3_disassemble(state, state.mem[state.pc]).c_str());
    printf("R0 %6hd|x%04hx\tR1 %6hd|x%04hx\tR2 %6hd|x%04hx\tR3 %6hd|x%04hx\nR4 %6hd|x%04hx\tR5 %6hd|x%04hx\tR6 %6hd|x%04hx\tR7 %6hd|x%04hx\nCC: %s\n",
           state.regs[0], state.regs[0], state.regs[1], state.regs[1], state.regs[2], state.regs[2],
           state.regs[3], state.regs[3], state.regs[4], state.regs[4], state.regs[5], state.regs[5],
           state.regs[6], state.regs[6], state.regs[7], state.regs[7], (state.n ? "N" : (state.z ? "Z" : "P")));
}

void do_load(const std::string& filename)
{
    lc3_init(state);
    try
    {
        lc3_assemble(state, filename);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "[ERROR] %s failed to assemble. %s\n", filename.c_str(), e.what().c_str());
        return;
    }
    current_filename = filename;
}

void do_reload()
{
    if (current_filename.empty())
        return;

    lc3_init(state);
    try
    {
        lc3_assemble(state, current_filename);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "[ERROR] %s failed to assemble. %s\n", current_filename.c_str(), e.what().c_str());
        return;
    }
}

void do_loadover(const std::string& filename)
{
    try
    {
        lc3_assemble(state, filename);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "[ERROR] %s failed to assemble. %s\n", filename.c_str(), e.what().c_str());
        return;
    }
    current_filename = filename;
}

void do_reloadover(const std::string& filename)
{
    if (current_filename.empty())
        return;

    try
    {
        lc3_assemble(state, current_filename);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "[ERROR] %s failed to assemble. %s\n", current_filename.c_str(), e.what().c_str());
        return;
    }
}

void do_quit(void)
{
    exit(EXIT_SUCCESS);
}

void do_exit(void)
{
    exit(EXIT_SUCCESS);
}


std::string binary_instruction_string(unsigned short data)
{
    std::bitset<16> b = data;
    std::stringstream stringstr;
    stringstr << b;
    return stringstr.str();
}
