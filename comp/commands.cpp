#include "commands.hpp"
#include <lc3_all.hpp>
#include <logger.hpp>

extern lc3_state state;

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

}

void do_list(unsigned short start)
{

}

void do_list(unsigned short start, unsigned short end, int level)
{

}

void do_dump(unsigned short start)
{

}

void do_dump(unsigned short start, unsigned short end)
{

}

void do_debug_info(void)
{

}

void do_debug_info(const std::string& name)
{

}

void do_debug_info(unsigned short address)
{

}

void do_info(void)
{

}

void do_load(const std::string& filename)
{

}

void do_reload(const std::string& filename)
{

}

void do_loadover(const std::string& filename)
{

}

void do_reloadover(const std::string& filename)
{

}

void do_quit(void)
{

}

void do_exit(void)
{

}
