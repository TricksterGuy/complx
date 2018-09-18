#ifndef PYLC3_HPP
#define PYLC3_HPP

#include <sstream>
#include <lc3_all.hpp>

/** A simple wrapper around lc3_state struct to bind to python. */
class LC3State
{
public:
    /** Constructor
      *
      * @param testing_mode Collect extra metrics and disable stdin and stdout.
      */
    explicit LC3State(bool testing_mode = false) : testing(testing_mode)
    { 
        lc3_init(state);
    };
    /** @see lc3_init 
      *  @param randomize Enable randomizing of both memory and registers.
      *  @param fill_value If randomize is false the value for every single memory address and register.
      */
    void init(bool randomize = true, short fill_value = 0)
    {
        lc3_init(state, randomize, randomize, fill_value, fill_value);
    }
    /** @see lc3_assemble 
      * @param filename Full path of the file to load.
      * @param disable_plugins True to disable lc3 plugins.
      * @param process_debug_comments True to enable processing of @ statements in comments.
      * @param multiple_errors Assembling doesn't end with the first error message.
      * @param enable_warnings Enable assembler warnings.
      * @param warnings_as_errors Treat assembler warnings as errors.
      */
    bool load(const std::string& filename, bool disable_plugins = false, bool process_debug_comments = true, bool multiple_errors = true, bool enable_warnings = false, bool warnings_as_errors = false);
    /** Test only */
    bool loadCode(const std::string& lc3_code);
    /** @see lc3_run */
    void run() { state.halted = 0 ; lc3_run(state); }
    /** @see lc3_run */
    void run(unsigned int num) { state.halted = 0 ; lc3_run(state, num); }
    /** @see lc3_step */
    void step() { state.halted = 0 ; lc3_step(state); }
    /** @see lc3_back */
    void back() { state.halted = 0 ; lc3_back(state); }
    /** @see lc3_rewind */
    void rewind() { state.halted = 0 ; lc3_rewind(state); }
    /** @see lc3_rewind */
    void rewind(unsigned int num) { state.halted = 0 ; lc3_rewind(state, num); }
    /** @see lc3_finish */
    void finish() { state.halted = 0 ; lc3_finish(state); }
    /** @see lc3_next_line */
    void next_line(unsigned int num = 0) 
    { 
        state.halted = 0;
        for (unsigned int i = 0; i < num; i++)
            lc3_next_line(state);
    }
    /** @see lc3_prev_line */
    void previous_line(unsigned int num = 0)
    {
        state.halted = 0;
        for (unsigned int i = 0; i < num; i++)
            lc3_prev_line(state);
    }
    /** @see lc3_mem_read */
    int memory_read(unsigned short address) { return lc3_mem_read(state, address); }
    /** @see lc3_mem_write */
    void memory_write(unsigned short address, int value) { lc3_mem_write(state, address, value); }
    /** @see lc3_sym_lookup */
    int lookup(const std::string& symbol) { return lc3_sym_lookup(state, symbol); }
    /** @see lc3_sym_rev_lookup */
    const std::string reverse_lookup(unsigned short address) { return lc3_sym_rev_lookup(state, address); }
    

    /** Gets value at address, note that the difference between this and memory_read is that memory_read will trigger plugins and devices */
    int get_memory(unsigned short address) const { return state.mem[address]; }
    /** Sets value at address, note that the difference between this and memory_write is that memory_write will trigger plugins and devices */
    void set_memory(unsigned short address, int value) { state.mem[address] = value; }

    /** @see lc3_disassemble */
    const std::string disassemble(unsigned short address, int level) 
    { 
        ///TODO remove disassemble's accessing of state.pc it should assume the address where it is located is the "pc"
        unsigned short pc = state.pc;
        state.pc = address + 1;
        const std::string ret = lc3_disassemble(state, state.mem[address], level); 
        state.pc = pc;
        return ret;
    }
    /** @see lc3_disassemble */
    const std::string disassemble_data(unsigned short data, int level) 
    { 
        return lc3_disassemble(state, data, level); 
    }
    /** @see lc3_add_break */
    bool add_breakpoint(unsigned short address, const std::string& condition = "1", int times = -1, const std::string& label = "") { return lc3_add_break(state, address, label, condition, times); }
    /** @see lc3_add_break */
    bool add_breakpoint(const std::string& symbol, const std::string& condition = "1", int times = -1, const std::string& label = "") { return lc3_add_break(state, symbol, label, condition, times); }
    /** @see lc3_add_watch */
    bool add_watchpoint(bool is_reg, unsigned short data, const std::string& condition, int times = -1, const std::string& label = "") { return lc3_add_watch(state, is_reg, data, condition, label, times); }
    /** @see lc3_add_watch */
    bool add_watchpoint(const std::string& symbol, const std::string& condition, int times = -1, const std::string& label = "") { return lc3_add_watch(state, symbol, condition, label, times); }
    /** @see lc3_add_blackbox */
    bool add_blackbox(const std::string& symbol, const std::string& condition = "1", const std::string& label = "") { return lc3_add_blackbox(state, symbol, label, condition); }
    /** @see lc3_add_blackbox */
    bool add_blackbox(unsigned short addr, const std::string& condition = "1", const std::string& label = "") { return lc3_add_blackbox(state, addr, label, condition); }
    /** @see lc3_remove_break */
    bool remove_breakpoint(unsigned short address) { return lc3_remove_break(state, address); }
    /** @see lc3_remove_break */
    bool remove_breakpoint(const std::string& symbol) { return lc3_remove_break(state, symbol); }
    /** @see lc3_remove_watch */
    bool remove_watchpoint(bool is_reg, unsigned short data) { return lc3_remove_watch(state, is_reg, data); }
    /** @see lc3_remove_watch */
    bool remove_watchpoint(const std::string& symbol) { return lc3_remove_watch(state, symbol); }
    /** @see lc3_remove_blackbox */
    bool remove_blackbox(unsigned short addr) { return lc3_remove_blackbox(state, addr); }
    /** @see lc3_remove_blackbox */
    bool remove_blackbox(const std::string& symbol) { return lc3_remove_blackbox(state, symbol); }

    /** Adds metadata for the subroutine specified */
    bool add_subroutine_info(const std::string& subroutine, int num_params)
    {
        int addr = lookup(subroutine);
        if (addr == -1) return false;
        lc3_subroutine_info info;
        info.address = lookup(subroutine);
        info.name = subroutine;
        info.num_params = num_params;
        
        state.subroutines[info.address] = info;
        return true;
    }

    /** @see srand */
    void seed(unsigned int seed) { srand(seed); }

    /** @see lc3_random */
    unsigned int random() { return lc3_random(); }

    int get_r0() const { return state.regs[0]; }
    void set_r0(int r0) { state.regs[0] = r0; }
    int get_r1() const { return state.regs[1]; }
    void set_r1(int r1) { state.regs[1] = r1; }
    int get_r2() const { return state.regs[2]; }
    void set_r2(int r2) { state.regs[2] = r2; }
    int get_r3() const { return state.regs[3]; }
    void set_r3(int r3) { state.regs[3] = r3; }
    int get_r4() const { return state.regs[4]; }
    void set_r4(int r4) { state.regs[4] = r4; }
    int get_r5() const { return state.regs[5]; }
    void set_r5(int r5) { state.regs[5] = r5; }
    int get_r6() const { return state.regs[6]; }
    void set_r6(int r6) { state.regs[6] = r6; }
    int get_r7() const { return state.regs[7]; }
    void set_r7(int r7) { state.regs[7] = r7; }
    int get_register(int reg_num) const { return reg_num >= 0 && reg_num <= 7 ? state.regs[reg_num] : -1; }
    void set_register(int reg_num, int value) { if (reg_num >= 0 && reg_num <= 7) state.regs[reg_num] = value; }
    bool get_n() const { return state.n; }
    bool get_z() const { return state.z; }
    bool get_p() const { return state.p; }
    unsigned short get_pc() const { return state.pc; }
    void set_pc(unsigned short pc) { state.pc = pc; }
    bool has_halted() const { return state.halted; }

    /** @see lc3_state.memory_ops */
    const std::map<unsigned short, lc3_memory_stats>& get_memory_ops() const { return state.memory_ops; }
    /** @see lc3_state.comments */
    const std::string comment(unsigned short address) const 
    {
        if (state.comments.find(address) == state.comments.end())
            return "";
        return state.comments.at(address);
    }
    const std::map<unsigned short, lc3_breakpoint_info>& get_breakpoints() const { return state.breakpoints; }
    const std::map<unsigned short, lc3_blackbox_info>& get_blackboxes() const { return state.blackboxes; }
    const std::map<unsigned short, lc3_watchpoint_info>& get_memory_watchpoints() const { return state.mem_watchpoints; }
    const std::map<unsigned short, lc3_watchpoint_info>& get_register_watchpoints() const { return state.reg_watchpoints; }

    unsigned int get_max_undo_stack_size() const { return state.max_stack_size; }
    void set_max_undo_stack_size(unsigned int size) { state.max_stack_size = size; }

    unsigned int get_max_call_stack_size() const { return state.max_call_stack_size; }
    void set_max_call_stack_size(unsigned int size) { state.max_call_stack_size = size; }

    bool get_true_traps() const { return state.true_traps; }
    void set_true_traps(bool setting) { lc3_set_true_traps(state, setting); }
    bool get_interrupts() const { return state.interrupt_enabled; }
    void set_interrupts(bool setting) { state.interrupt_enabled = setting; }
    bool get_strict_execution() const { return state.strict_execution; }
    void set_strict_execution(bool setting) { state.strict_execution = setting; }

    /** The following accessors are only meaningful if testing_mode was set */
    std::string get_input() const { return in.str(); }
    void set_input(std::string input) { in.str(input); }

    std::string get_output() const { return out.str(); }
    void set_output(std::string output) { out.str(output); }

    std::string get_warnings() const { return warning.str(); }
    void set_warnings(std::string warn_str) { warning.str(warn_str); }

    const std::vector<lc3_subroutine_call_info>& first_level_calls() const {return state.first_level_calls;}
    const std::vector<lc3_trap_call_info>& first_level_traps() const {return state.first_level_traps;}

private:
    lc3_state state;
    std::stringstream in;
    std::stringstream out;
    std::stringstream warning;
    bool testing;
};

#endif
