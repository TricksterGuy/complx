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

/** A simple wrapper around lc3_test struct to bind to python. */
class LC3Test
{
public:
    LC3Test() {}
    LC3Test(const lc3_test& ctest) : test(ctest) {}
    const std::string& get_name() const { return test.name; }
    const std::string& get_warning() const { return test.warning; }
    unsigned int get_points() const { return test.points; }
    unsigned int get_max_points() const { return test.max_points; }
    bool get_passed() const { return test.passed; }
    bool get_true_traps() const { return test.true_traps; }
    bool get_interrupt_enabled() const { return test.interrupt_enabled; }
    bool get_disable_plugins() const { return test.disable_plugins; }
    bool get_randomize() const { return test.randomize; }
    bool get_fully_randomize() const { return test.fully_randomize; }
    bool has_max_executions() const { return test.has_max_executions; }
    bool has_halted() const { return test.has_halted; }
    bool has_halted_normally() const { return test.has_halted_normally; }
    long get_random_seed() const { return test.random_seed; }
    int get_fill_value() const { return test.fill_value; }
    unsigned long get_executions() const { return test.executions; }
    unsigned long get_max_executions() const { return test.max_executions; }
    unsigned long get_warnings() const { return test.warnings; }
private:
    lc3_test test;
};

/** A simple wrapper around lc3_test_suite struct to bind to python. */
class LC3TestSuite
{
public:
    LC3TestSuite() {}
    /** loads an xml file */
    bool load(const std::string& xml_file);
    /** @see lc3_run_test_suite */
    void run(const std::string& assembly_file, int run_number = 0) { lc3_run_test_suite(suite, assembly_file, -1, run_number); }
    /** Get LC3Test object */
    LC3Test get_test(unsigned int id) const;
    /** Get number of tests */
    unsigned int get_num_tests() const { return suite.tests.size(); }

    bool get_passed() const { return suite.passed; }
    bool get_points() const { return suite.points; }
    bool get_max_points() const { return suite.max_points; }
private:
    lc3_test_suite suite;
};

#endif
