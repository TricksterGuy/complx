#ifndef LC3_RUNNER_HPP
#define LC3_RUNNER_HPP

#include "lc3.hpp"

/*
=================================
Initialization and Play Functions
=================================
*/
/** Initializes the lc3 state */
void lc3_init(lc3_state& state, bool randomize_registers = true, bool randomize_memory = false, short fill_value = 0);
/** Removes all installed plugins */
void lc3_remove_plugins(lc3_state& state);
/** Runs until halt */
void lc3_run(lc3_state& state);
/** Runs for X instructions */
void lc3_run(lc3_state& state, unsigned int num);
/** Runs one instruction */
void lc3_step(lc3_state& state);
/** Backsteps one instruction */
void lc3_back(lc3_state& state);
/** Backsteps many instructions */
void lc3_rewind(lc3_state& state, unsigned int num);
/** Fully rewind back to initial state */
void lc3_rewind(lc3_state& state);
/** Executes the next line and blackboxes any subroutines and traps*/
void lc3_next_line(lc3_state& state);
/** Goes back one line again blackboxes any subroutines and traps */
void lc3_prev_line(lc3_state& state);
/** Finishes the current subroutine */
void lc3_finish(lc3_state& state);
/** Checks and processes interrupt */
void lc3_interrupt(lc3_state& state);
/** Signals a keyboard interrupt */
void lc3_keyboard_interrupt(lc3_state& state);
/** Singals a general interrupt will stack */
void lc3_signal_interrupt(lc3_state& state, int priority, int vector);
/** Singals a general interrupt only if it does not already exist pending */
bool lc3_signal_interrupt_once(lc3_state& state, int priority, int vector);
/** Ticks all LC3 Plugins */
void lc3_tick_plugins(lc3_state& state);
/** Tocks all LC3 Plugins */
void lc3_tock_plugins(lc3_state& state);

#endif
