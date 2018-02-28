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
    /** @see lc3_assemble */
    bool load(const std::string& filename);
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

    /** Gets value at address, note that the difference between this and memory_read is that memory_read will trigger plugins and devices */
    short get_memory(unsigned short address) const { return state.mem[address]; }
    /** Sets value at address, note that the difference between this and memory_write is that memory_write will trigger plugins and devices */
    void set_memory(unsigned short address, short value) { state.mem[address] = value; }

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
