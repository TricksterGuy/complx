#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>
#include <lc3_all.hpp>

// Execution
/** Runs until it halts */
void do_run(void);
/** If times is given runs for that many instructions else steps 1 instruction */
void do_step(unsigned int times = 1);
/** If times is given back steps for that many instructions else back steps 1 instruction */
void do_back(unsigned int times = 1);
/** If times is given steps that that many lines (not stepping into any subroutines / traps) else steps 1 line */
void do_next(unsigned int times = 1);
/** If times is given goes back that many lines (not stepping into any subroutines / traps) else back steps 1 line */
void do_prev(unsigned int times = 1);
/** Steps out of the current subroutine */
void do_finish(void);
/** Backsteps until undo stack empty */
void do_rewind(void);

// Debugging
/** Puts a temporary breakpoint at addr (this is equivalent to break addr, name, 1, 1) */
void do_tempbreak(unsigned short address);
void do_tempbreak(const std::string& symbol);
/** Puts breakpoint at addr with condition cond, times number of hits before becoming inactive, and name name */
void do_break(unsigned short address, const std::string& name = "", const std::string& condition = "", int times = -1);
void do_break(const std::string& symbol, const std::string& name = "", const std::string& condition = "", int times = -1);
/** Sets a watchpoint on target with condition cond, times number of hits before becoming inactive, and name name */
void do_watch(const std::string& symbol, const std::string& condition, const std::string& name = "", int times = -1);
void do_watch(unsigned char reg, const std::string& condition, const std::string& name = "", int times = -1);
void do_watch(unsigned short address, const std::string& condition, const std::string& name = "", int times = -1);
/** Marks address as a blackbox with name name and condition to activate. An address marked as a blackbox will never be stepped into */
void do_blackbox(unsigned short address, const std::string& name = "", const std::string& condition = "1");
void do_blackbox(const std::string& symbol, const std::string& name = "", const std::string& condition = "1");
/** Sets size of undo stack */
void do_undostack(unsigned int num);
/** Sets size of call stack */
void do_callstack(unsigned int num);
/** Deletes all breakpoints/watchpoints/blackboxes with symbol or address */
void do_delete(const std::string& name);
void do_delete(unsigned short address);
void do_delete(unsigned char reg);

// Manipulation
/** Sets register/cc/pc/address to value */
void do_set(unsigned char reg, short value);
void do_set(unsigned char reg, const std::string& expr);
void do_set(unsigned short address, short value);
void do_set(unsigned short address, const std::string& expr);
void do_set(const std::string& symbol, short value);
void do_set(const std::string& symbol, const std::string& expr);
/** Fills memory with value */
void do_fillmem(short value);
/** Sets source for console input cin to reset */
void do_input(const std::string& source);
/** Sets source for console output cout to reset*/
void do_output(const std::string& sink);
/** Resets lc3 state */
void do_reset(void);
/** Randomizes lc3 state */
void do_randomize(void);
/** Turns on or off the true traps setting.  Will step into traps if set */
void do_truetraps(bool set);
/** Toggles interrupts if no param given else true sets interrupts false unsets interrupts */
void do_interrupt(bool set);

// Display
/** Displays the address given or tries to display a range of addresses */
void do_list(unsigned short start, int level = LC3_NORMAL_DISASSEMBLE);
void do_list(const std::string& start, int level = LC3_NORMAL_DISASSEMBLE);
/** Displays all registers cc and pc in both decimal and hex.  Also displays total number of instructions executed */
void do_info(void);

// File
/** Reinitializes and loads filename */
void do_load(const std::string& filename);
/** Reinitializes and Reloads last file if there was a file already loaded */
void do_reload(void);
/** Loads filename over current state */
void do_loadover(const std::string& filename);
/** Reloads last filename over current state */
void do_reloadover(void);
/** Quits the program */
void do_quit(void);
void do_exit(void);

#endif
