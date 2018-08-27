#ifndef PYLC3_HPP
#define PYLC3_HPP

#include <lc3_all.hpp>

/** A simple wrapper around lc3_state struct to bind to python. */
class LC3State
{
public:
    LC3State() { lc3_init(state); };
    /** @see lc3_init */
    void init(bool randomize_registers = true, bool randomize_memory = true, short register_fill_value = 0, short memory_fill_value = 0)
    {
        lc3_init(state, randomize_registers, randomize_memory, register_fill_value, memory_fill_value);
    }
    /** @see lc3_assemble 
        @param filename Full path of the file to load.
        @param testing_mode If we are using this state for testing.  Additional metrics are captured such as the stack frame.
    */
    bool load(const std::string& filename, bool testing_mode = false);
    /** @see lc3_run */
    void run(unsigned int num) { lc3_run(state, num); }
    /** @see lc3_step */
    void step() { lc3_step(state); }
    /** @see lc3_back */
    void back() { lc3_back(state); }
    /** @see lc3_rewind */
    void rewind() { lc3_rewind(state); }
    /** @see lc3_finish */
    void finish() { lc3_finish(state); }
    /** @see lc3_next_line */
    void next_line() { lc3_next_line(state); }
    /** @see lc3_prev_line */
    void previous_line() { lc3_prev_line(state); }
    /** @see lc3_mem_read */
    short memory_read(unsigned short address) { return lc3_mem_read(state, address); }
    /** @see lc3_mem_write */
    void memory_write(unsigned short address, short value) { lc3_mem_write(state, address, value); }
    /** @see lc3_sym_lookup */
    short lookup(const std::string symbol) { return lc3_sym_lookup(state, symbol); }

    /** Gets value at address, note that the difference between this and memory_read is that memory_read will trigger plugins and devices */
    short get_memory(unsigned short address) const { return state.mem[address]; }
    /** Sets value at address, note that the difference between this and memory_write is that memory_write will trigger plugins and devices */
    void set_memory(unsigned short address, short value) { state.mem[address] = value; }

    /** @see lc3_add_break */
    bool add_breakpoint(unsigned short address) { return lc3_add_break(state, address); }
    /** @see lc3_remove_break */
    bool remove_breakpoint(unsigned short address) { return lc3_remove_break(state, address); }

    short get_r0() const { return state.regs[0]; }
    void set_r0(short r0) { state.regs[0] = r0; }
    short get_r1() const { return state.regs[1]; }
    void set_r1(short r1) { state.regs[1] = r1; }
    short get_r2() const { return state.regs[2]; }
    void set_r2(short r2) { state.regs[2] = r2; }
    short get_r3() const { return state.regs[3]; }
    void set_r3(short r3) { state.regs[3] = r3; }
    short get_r4() const { return state.regs[4]; }
    void set_r4(short r4) { state.regs[4] = r4; }
    short get_r5() const { return state.regs[5]; }
    void set_r5(short r5) { state.regs[5] = r5; }
    short get_r6() const { return state.regs[6]; }
    void set_r6(short r6) { state.regs[6] = r6; }
    short get_r7() const { return state.regs[7]; }
    void set_r7(short r7) { state.regs[7] = r7; }
    short get_register(int reg_num) const { return reg_num >= 0 && reg_num <= 7 ? state.regs[reg_num] : -1; }
    void set_register(int reg_num, short value) { if (reg_num >= 0 && reg_num <= 7) state.regs[reg_num] = value; }
    bool get_n() const { return state.n; }
    bool get_z() const { return state.z; }
    bool get_p() const { return state.p; }
    unsigned short get_pc() const { return state.pc; }
    void set_pc(unsigned short pc) { state.pc = pc; }
    bool has_halted() const { return state.halted; }

private:
    lc3_state state;
};

#endif
