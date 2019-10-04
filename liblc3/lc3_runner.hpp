#ifndef LC3_RUNNER_HPP
#define LC3_RUNNER_HPP

#include "lc3.hpp"

/** lc3_init
  *
  * Initializes the state of the lc3.
  * @param state LC3State object.
  * @param randomize_registers if true randomizes registers.
  * @param randomize_memory if true randomizes memory.
  * @param register_fill_value ignored if randomize_registers is true otherwise sets registers to this value.
  * @param memory_fill_value ignored if randomize_memory is true otherwise sets memory to this value (except for TVT, IVT and lc3 os code).
  */
void lc3_init(lc3_state& state, bool randomize_registers = true, bool randomize_memory = true, short register_fill_value = 0, short memory_fill_value = 0);
/** lc3_set_version
  * Sets the lc3's version should be done after lc3_init.
  * This function will overwrite the LC3OS code with the proper OS for that version.
  * @param version. LC-3 Version. Valid values are 0 for original LC-3 and 1 for 2019's revision.
  */
void lc3_set_version(lc3_state& state, int version);
/** lc3_remove_plugins
  *
  * Removes all installed lc3 plugins.
  * @param state LC3State object.
  */
void lc3_remove_plugins(lc3_state& state);
/** lc3_run
  *
  * Executes instructions until the lc3 is halted.
  * @param state LC3State object.
  */
void lc3_run(lc3_state& state);
/** lc3_run
  *
  * Runs for X instructions or until the lc3 is halted.
  * @param state LC3State object.
  * @param num Number of instructions to execute.
  */
void lc3_run(lc3_state& state, unsigned int num);
/** lc3_step
  *
  * Executes one instruction.
  * @param state LC3State object.
  */
void lc3_step(lc3_state& state);
/** lc3_back
  *
  * Attempts to backsteps (undoes) one instruction.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  */
void lc3_back(lc3_state& state);
/** lc3_rewind
  *
  * Attempts to backstep a number of instructions.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  * @param num Number of instructions to back step.
  */
void lc3_rewind(lc3_state& state, unsigned int num);
/** lc3_rewind
  *
  * Attempts to backstep as many instructions as possible to get back to initial state.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  */
void lc3_rewind(lc3_state& state);
/** lc3_next_line
  *
  * Executes the next line and blackboxes any subroutines and traps.
  * @param state LC3State object.
  */
void lc3_next_line(lc3_state& state);
/** lc3_prev_line
  *
  * Goes back one line again blackboxes any subroutines and traps.
  * This may not be possible depending on the size of the undo stack.
  * @param state LC3State object.
  */
void lc3_prev_line(lc3_state& state);
/** lc3_finish
  *
  * Finishes the current subroutine.
  * @param state LC3State object.
  */
void lc3_finish(lc3_state& state);
/** lc3_interrupt
  *
  * Checks for and processes a single pending interrupt.
  * @param state LC3State object.
  * @return true if an interrupt occurred.
  */
bool lc3_interrupt(lc3_state& state);
/** lc3_keyboard_interrupt
  *
  * Signals a keyboard interrupt.
  * @param state LC3State object.
  */
void lc3_keyboard_interrupt(lc3_state& state);
/** lc3_signal_interrupt
  *
  * Adds an interrupt to be handled to the pending queue.
  * Will stack if called multiple times with the same priority and interrupt vector.
  * @param state LC3State object.
  * @param priority Priority of the interrupt.
  * @param vector Interrupt vector to be accessed when interrupt occurs.
  */
void lc3_signal_interrupt(lc3_state& state, int priority, int vector);
/** lc3_signal_interrupt_once
  *
  * Adds an interrupt to be handled to the pending queue ONLY if it doesn't already exist.
  * @param state LC3State object.
  * @param priority Priority of the interrupt.
  * @param vector Interrupt vector to be accessed when interrupt occurs.
  * @return bool true if it was added false otherwise.
  */
bool lc3_signal_interrupt_once(lc3_state& state, int priority, int vector);
/** lc3_check_keyboard_interrupt
  *
  * Checks if there is a character waiting and if we aren't in a keyboard interrupt
  * already and if so (with random jitter) signals a keyboard interrupt.
  * @param state LC3State object.
  */
void lc3_check_keyboard_interrupt(lc3_state& state);
/** lc3_tick_plugins
  *
  * Calls OnTick for all plugins.
  * This happens before the instruction is fetch.
  * @param state LC3State object.
  */
void lc3_tick_plugins(lc3_state& state);
/** lc3_tock_plugins
  *
  * Calls OnTock for all plugins.
  * This happens after the instruction is executed.
  * @param state LC3State object.
  */
void lc3_tock_plugins(lc3_state& state);


#endif
