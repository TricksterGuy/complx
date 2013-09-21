#ifndef LC3_EXEC_HPP
#define LC3_EXEC_HPP

#include "lc3.hpp"

/*
===================
Execution Functions
===================
*/
/** Forms the instruction from the given data */
lc3_instr lc3_decode(lc3_state& state, unsigned short data);
/** Executes the given instruction */
const lc3_state_change lc3_execute(lc3_state& state, lc3_instr instruction);
/** Executes a trap instruction */
void lc3_trap(lc3_state& state, lc3_state_change& changes, trap_instr trap);
/** Updates the Condition Code Registers */
void lc3_setcc(lc3_state& state, short value);
/** Reads from memory */
short lc3_mem_read(lc3_state& state, unsigned short addr, bool privileged = false);
/** Writes to memory */
void lc3_mem_write(lc3_state& state, unsigned short addr, short val, bool privileged = false);
/** Prints out a warning message */
void lc3_warning(lc3_state& state, unsigned int warn_id, short arg1, short arg2);
/** Prints out a custom warning message */
void lc3_warning(lc3_state& state, const std::string& warning);
/** Notifies plugins of a read */
void lc3_notify_plugins_read(lc3_state& state, unsigned short addr);
/** Notifies plugins of a write */
void lc3_notify_plugins_write(lc3_state& state, unsigned short addr, short val);

#endif
