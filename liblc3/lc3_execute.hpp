#ifndef LC3_EXEC_HPP
#define LC3_EXEC_HPP

#include "lc3.hpp"

/** lc3_decode
  *
  * Decodes the given instruction.
  *
  * @param state LC3State object.
  * @param data Instruction data.
  * @return an lc3_instr with the data in instruction form.
  */
lc3_instr lc3_decode(lc3_state& state, unsigned short data);
/** lc3_execute
  *
  * Executes the given instruction.
  * @param state LC3State object.
  * @param instruction Previously decoded instruction.
  * @return the changes in lc3 state as performed by the instruction executed.
  */
const lc3_state_change lc3_execute(lc3_state& state, lc3_instr instruction);
/** lc3_trap
  *
  * Executes the trap instruction passed in.
  * @param state LC3State object.
  * @param changes Output param for changes made as part of executing the trap.
  * @param trap Trap instruction data.
  */
void lc3_trap(lc3_state& state, lc3_state_change& changes, trap_instr trap);
/** lc3_setcc
  *
  * Updates the condition code registers.
  * @param state LC3State object.
  * @param value Value to use to set CC, negative sets N, zero sets Z, positive sets P.
  */
void lc3_setcc(lc3_state& state, short value);
/** lc3_mem_read
  *
  * Reads from memory.
  * @param state LC3State object.
  * @param addr The address to read from.
  * @param privileged If set allow to read from any address without a warning.
  * @return the value read from memory.
  */
short lc3_mem_read(lc3_state& state, unsigned short addr, bool privileged = false);
/** lc3_mem_write
  *
  * Writes to memory.
  * @param state LC3State object.
  * @param address The address to write to.
  * @param val The value to put into memory.
  * @param privileged If set allow to write to any address without a warning.
  */
void lc3_mem_write(lc3_state& state, unsigned short addr, short val, bool privileged = false);
/** lc3_warning
  *
  * Prints a warning message on the warnings stream.
  * @param state LC3State object.
  * @param warn_id Warning ID to print @see WARNINGS.
  * @param arg1 Argument 1 for formatting.
  * @param arg2 Argument 2 for formatting.
  */
void lc3_warning(lc3_state& state, unsigned int warn_id, short arg1, short arg2);
/** lc3_warning
  *
  * Prints a custom warning message on the warnings stream.
  * @param state LC3State object.
  * @param warning Warning message to print out.
  */
void lc3_warning(lc3_state& state, const std::string& warning);

#endif
