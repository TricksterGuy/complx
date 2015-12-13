#ifndef COMMANDS_HPP
#define COMMANDS_HPP

#include <string>

// Execution
/** Runs until it halts */
void do_run(void);
/** If times is given runs for that many instructions else steps 1 instruction */
void do_step(int times = 1);
/** If times is given back steps for that many instructions else back steps 1 instruction */
void do_back(int times = 1);
/** If times is given steps that that many lines (not stepping into any subroutines / traps) else steps 1 line */
void do_next(int times = 1);
/** If times is given goes back that many lines (not stepping into any subroutines / traps) else back steps 1 line */
void do_prev(int times = 1);
/** Steps out of the current subroutine */
void do_finish(void);
/** Backsteps until undo stack empty */
void do_rewind(void);

// Debugging
/** Puts a temporary breakpoint at addr (this is equivalent to break addr, name, 1, 1) */
void do_tempbreak(unsigned short address);
/** Puts breakpoint at addr with condition cond, times number of hits before becoming inactive, and name name */
void do_break(unsigned short address, const std::string& condition = "", int times = -1, const std::string& name = "");
/** Sets a watchpoint on target with condition cond, times number of hits before becoming inactive, and name name */
void do_watch(const std::string& target, const std::string& condition, int times = -1, const std::string& name = "");
/** Marks address as a blackbox with name name and condition to activate. An address marked as a blackbox will never be stepped into */
void do_blackbox(unsigned short address, const std::string& name = "", const std::string& condition = "1");
/** Sets size of undo stack */
void do_undo_stack(unsigned short num);
/** Deletes all breakpoints/watchpoints/blackboxes with name or address */
void do_delete(const std::string& name);
void do_delete(unsigned short address);

// Manipulation
/** Sets register/cc/pc/address to value */
void do_set(const std::string& thing, short value);
void do_set(const std::string& thing, const std::string& value);
/** Sets source for console input */
void do_input(const std::string& source);
/** Sets source for console output */
void do_output(const std::string& sink);
/** Resets lc3 state */
void do_reset(void);
/** Randomizes lc3 state */
void do_randomize(void);
/** Turns on or off the true traps setting.  Will step into traps if set */
void do_true_traps(bool set);
/** Toggles interrupts if no param given else true sets interrupts false unsets interrupts */
void do_interrupt(bool set);

// Display
/** Prints register/cc/pc/address */
void do_print(const std::string& thing);
/** Without any parameters displays the instructions around the pc.  If level is given affects the disassemble level (dumb, normal, highlevel) */
void do_list(void);
void do_list(unsigned short start);
void do_list(unsigned short start, unsigned short end, int level = 1);
/** Prints out the data (in decimal and hex) between start and end (or start if end isn't given) */
void do_dump(unsigned short start);
void do_dump(unsigned short start, unsigned short end);
/** Displays breakpoint/watchpoint/blackbox info for address or name, or if no params displays all breakpoints/watchpoints/blackboxes */
void do_debug_info(void);
void do_debug_info(const std::string& name);
void do_debug_info(unsigned short address);
/** Displays all registers cc and pc in both decimal and hex.  Also displays total number of instructions executed */
void info(void);

// File
/** Reinitializes and loads filename */
void do_load(const std::string& filename);
/** Reinitializes and Reloads last file if there was a file already loaded */
void do_reload(const std::string& filename);
/** Loads filename over current state */
void loadover(const std::string& filename);
/** Reloads last filename over current state */
void reloadover(const std::string& filename);
/** Quits the program */
void quit(void);
void exit(void);

#endif
