#include "commands.hpp"
#include "windows.hpp"
#include <logger.hpp>
#include <ncurses.h>

extern lc3_state state;
extern std::unique_ptr<MemoryWindow> memory;
extern std::unique_ptr<RegisterWindow> registers;
std::string current_filename;

#define NUM_LIST_DEFAULT 16

std::string binary_instruction_string(unsigned short data);

void do_run(void)
{
    state.halted = 0;
    lc3_run(state);
    memory->Update();
    registers->Update();
}

void do_step(unsigned int times)
{
    state.halted = 0;
    lc3_run(state, times);
    memory->Update();
    registers->Update();
}

void do_back(unsigned int times)
{
    lc3_rewind(state, times);
    memory->Update();
    registers->Update();
}

void do_next(unsigned int times)
{
    state.halted = 0;
    for (unsigned int i = 0; i < times; i++)
        lc3_next_line(state);
    memory->Update();
    registers->Update();
}

void do_prev(unsigned int times)
{
    for (unsigned int i = 0; i < times; i++)
        lc3_prev_line(state);
    memory->Update();
    registers->Update();
}

void do_finish(void)
{
    state.halted = 0;
    lc3_finish(state);
    memory->Update();
    registers->Update();
}

void do_rewind(void)
{
    lc3_rewind(state);
    memory->Update();
    registers->Update();
}

void do_tempbreak(unsigned short address)
{
    lc3_add_break(state, address, "", "1", 1);
    memory->Update();
}

void do_tempbreak(const std::string& symbol)
{
    lc3_add_break(state, symbol, "", "1", 1);
    memory->Update();
}

void do_break(unsigned short address, const std::string& name, const std::string& condition, int times)
{
    lc3_add_break(state, address, name, condition, times);
    memory->Update();
}

void do_break(const std::string& symbol, const std::string& name, const std::string& condition, int times)
{
    lc3_add_break(state, symbol, name, condition, times);
    memory->Update();
}

void do_watch(const std::string& symbol, const std::string& condition, const std::string& name, int times)
{
    lc3_add_watch(state, symbol, condition, name, times);
    memory->Update();
}

void do_watch(unsigned char reg, const std::string& condition, const std::string& name, int times)
{
    lc3_add_watch(state, true, reg, condition, name, times);
    registers->Update();
}

void do_watch(unsigned short address, const std::string& condition, const std::string& name, int times)
{
    lc3_add_watch(state, false, address, condition, name, times);
    memory->Update();
}

void do_blackbox(unsigned short address, const std::string& name, const std::string& condition)
{
    lc3_add_blackbox(state, address, name, condition);
    memory->Update();
}

void do_blackbox(const std::string& symbol, const std::string& name, const std::string& condition)
{
    lc3_add_blackbox(state, symbol, name, condition);
    memory->Update();
}

void do_undostack(unsigned int num)
{
    state.max_stack_size = num;
}

void do_callstack(unsigned int num)
{
    state.max_call_stack_size = num;
}

void do_delete(const std::string& symbol)
{
    lc3_remove_blackbox(state, symbol);
    lc3_remove_break(state, symbol);
    lc3_remove_watch(state, symbol);
    memory->Update();
}

void do_delete(unsigned short address)
{
    lc3_remove_blackbox(state, address);
    lc3_remove_break(state, address);
    lc3_remove_watch(state, false, address);
    memory->Update();
}

void do_delete(unsigned char reg)
{
    lc3_remove_watch(state, true, reg);
    registers->Update();
}

void do_set(unsigned char reg, short value)
{
    if (reg <= 7)
        state.regs[reg] = value;
    if (reg == 8)
        state.pc = value;
    if (reg == 9)
    {
        state.n = value < 0;
        state.z = value == 0;
        state.p = value > 0;
    }
    registers->Update();
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
    memory->Display(address, address, LC3_NORMAL_DISASSEMBLE);
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

void do_fillmem(short value)
{
    for (int i = 0; i < 0x10000; i++)
        state.mem[i] = value;
    memory->Update();
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
    registers->Update();
    memory->Update();
}

void do_randomize(void)
{
    lc3_init(state, true, true);
    registers->Update();
    memory->Update();
}

void do_truetraps(bool set)
{
    lc3_set_true_traps(state, set);
}

void do_interrupt(bool set)
{
    state.interrupt_enabled = set;
}

void do_list(unsigned short start)
{
    do_list(start, start);
}

void do_list(unsigned short start, unsigned short end, int level)
{
    memory->Display(start, end, level);
}

void do_list(const std::string& start)
{
    int lookup = lc3_sym_lookup(state, start);
    if (lookup == -1)
        return;
    do_list(lookup, lookup);
}

void do_list(const std::string& start, const std::string& end, int level)
{
    int slookup = lc3_sym_lookup(state, start);
    int elookup = lc3_sym_lookup(state, end);
    if (slookup == -1 || elookup == -1)
        return;
    do_list(slookup, elookup, level);
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
    memory->Update();
}

void do_reload(void)
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
    memory->Update();
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
    memory->Update();
}

void do_reloadover(void)
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
    memory->Update();
}

void do_quit(void)
{
    exit(EXIT_SUCCESS);
}

void do_exit(void)
{
    exit(EXIT_SUCCESS);
}
