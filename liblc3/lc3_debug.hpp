#ifndef LC3_DEBUG_HPP
#define LC3_DEBUG_HPP

#include "lc3.hpp"

/** lc3_add_break
  *
  * Adds a breakpoint at the given symbol.
  * @param state LC3State object.
  * @param symbol Symbol to add breakpoint to.
  * @param label The name for this breakpoint.
  * @param condition If condition evaluates to true, then the breakpoint will stop execution.
  * @param times Hit count. After the breakpoint is hit this number of times it disappears.
  * @return True if there was an error adding the breakpoint false otherwise.
  */
bool lc3_add_break(lc3_state& state, const std::string& symbol, const std::string& label = "", const std::string& condition = "1", int times = -1);
/** lc3_add_break
  *
  * Adds a breakpoint at the given address.
  * @param state LC3State object.
  * @param addr Address to place the breakpoint.
  * @param label The name for this breakpoint.
  * @param condition If condition evaluates to true, then the breakpoint will stop execution.
  * @param times Hit count. After the breakpoint is hit this number of times it disappears
  * @return True if there was an error adding the breakpoint false otherwise.
  */
bool lc3_add_break(lc3_state& state, unsigned short addr, const std::string& label = "", const std::string& condition = "1", int times = -1);
/** lc3_has_breakpoint
  *
  * Checks if there is a breakpoint at the given symbol.
  * @param state LC3State object.
  * @param symbol Address to check.
  * @return True if there is a breakpoint at the given symbol.
  */
bool lc3_has_breakpoint(lc3_state& state, const std::string& symbol);
/** lc3_has_breakpoint
  *
  * Checks if there is a breakpoint at the given address.
  * @param state LC3State object.
  * @param addr Address to check.
  * @return True if there is a breakpoint at the given address.
  */
bool lc3_has_breakpoint(lc3_state& state, unsigned short addr);
/** lc3_add_blackbox
  *
  * Adds a blackbox at the given symbol.
  * @param state LC3State object.
  * @param symbol Symbol to add blackbox to.
  * @param label The name for this blackbox.
  * @param condition If condition evaluates to true, then the code will not be traced (will execute all of the code when stepped through).
  * @return True if there was an error adding the blackbox false otherwise.
  */
bool lc3_add_blackbox(lc3_state& state, const std::string& symbol, const std::string& label = "", const std::string& condition = "1");
/** lc3_add_blackbox
  *
  * Adds a blackbox at the given address.
  * @param state LC3State object.
  * @param addr Address to place the blackbox
  * @param label The name for this blackbox.
  * @param condition If condition evaluates to true, then the code will not be traced (will execute all of the code when stepped through).
  * @return True if there was an error adding the blackbox false otherwise.
  */
bool lc3_add_blackbox(lc3_state& state, unsigned short addr, const std::string& label = "", const std::string& condition = "1");
/** lc3_has_blackbox
  *
  * Checks if there is a blackbox at the given symbol.
  * @param state LC3State object.
  * @param symbol Address to check.
  * @return True if there is a blackbox at the given symbol.
  */
bool lc3_has_blackbox(lc3_state& state, const std::string& symbol);
/** lc3_has_blackbox
  *
  * Checks if there is a blackbox at the given address.
  * @param state LC3State object.
  * @param addr Address to check.
  * @return True if there is a blackbox at the given address.
  */
bool lc3_has_blackbox(lc3_state& state, unsigned short addr);
/** lc3_add_watch
  *
  * Adds a watchpoint at the given address or register.
  * A watch point is a breakpoint on stores to addresses/registers.
  * @param state LC3State object.
  * @param is_reg True if data is referring to a register, otherwise false for a memory address
  * @param data Register number or memory address.
  * @param condition If condition evaluates to true, then the watchpoint will stop execution.
  * @param times Hit count. After the watchpoint is hit this number of times it disappears.
  * @return True if there was an error adding the watchpoint false otherwise.
  */
bool lc3_add_watch(lc3_state& state, bool is_reg, unsigned short data, const std::string& condition, const std::string& label = "", int times = -1);
/** lc3_add_watch
  *
  * Adds a watchpoint at the given symbol.
  * A watch point is a breakpoint on stores to addresses/registers.
  * @param state LC3State object.
  * @param symbol Symbol to associate with the watchpoint.
  * @param label The name for this watchpoint.
  * @param condition If condition evaluates to true, then the watchpoint will stop execution.
  * @param times Hit count. After the watchpoint is hit this number of times it disappears.
  * @return True if there was an error adding the watchpoint false otherwise.
  */
bool lc3_add_watch(lc3_state& state, const std::string& symbol, const std::string& condition, const std::string& label = "", int times = -1);
/** lc3_has_watch
  *
  * Checks if there is a watchpoint at the given symbol.
  * @param state LC3State object.
  * @param symbol Symbol to associate with the watchpoint.
  * @return True if there is a watchpoint at the symbol false otherwise
  */
bool lc3_has_watch(lc3_state& state, const std::string& symbol);
/** lc3_has_watch
  *
  * Checks if there is a watchpoint at the given address or register.
  * @param state LC3State object.
  * @param is_reg True if data is referring to a register, otherwise false for a memory address
  * @param data Register number or memory address.
  * @return True if there was a watchpoint at the given address/register false otherwise.
  */
bool lc3_has_watch(lc3_state& state, bool is_reg, unsigned short data);

/** lc3_add_subroutine
  *
  * Registers a symbol as a subroutine.
  * @param state LC3State object.
  * @param symbol Symbol associated with the subroutine's address.
  * @param name Name of the subroutine.
  * @param params Names of parameters.
  * @return True if there was an error adding subroutine false otherwise.
  */
bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name, const std::vector<std::string>& params);
/** lc3_add_subroutine
  *
  * Registers a symbol as a subroutine.
  * @param state LC3State object.
  * @param symbol Symbol associated with the subroutine's address.
  * @param name Name of the subroutine.
  * @param num_params Number of parameters the subroutine accepts.
  * @return True if there was an error adding subroutine false otherwise.
  */
bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name = "", int num_params = 0);
/** lc3_add_subroutine
  *
  * Registers an address as a subroutine.
  * @param state LC3State object.
  * @param address Address where the subroutine is located
  * @param name Name of the subroutine
  * @param params Names of parameters
  * @return True if there was an error adding the subroutine false otherwise.
  */
bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name, const std::vector<std::string>& params);
/** lc3_add_subroutine
  *
  * Registers an address as a subroutine.
  * @param state LC3State object.
  * @param address Address where the subroutine is located
  * @param name Name of the subroutine
  * @param num_params Number of parameters the subroutine accepts
  * @return True if there was an error adding the subroutine false otherwise.
  */
bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name = "", int num_params = 0);
/** lc3_remove_break
  *
  * Removes the breakpoints associated with the symbol.
  * @param state LC3State object.
  * @param symbol Symbol to remove breakpoint from.
  * @return True if there was an error removing the breakpoint false otherwise.
  */
bool lc3_remove_break(lc3_state& state, const std::string& symbol);
/** lc3_remove_break
  *
  * Removes the breakpoints associated with the address.
  * @param state LC3State object.
  * @param addr Address to remove breakpoint from.
  * @return True if there was an error removing the blackbox false otherwise.
  */
bool lc3_remove_break(lc3_state& state, unsigned short addr);
/** lc3_remove_blackbox
  *
  * Removes the blackbox associated with the symbol.
  * @param state LC3State object.
  * @param symbol Symbol to remove blackbox from.
  * @return True if there was an error removing the blackbox false otherwise.
  */
bool lc3_remove_blackbox(lc3_state& state, const std::string& symbol);
/** lc3_remove_blackbox
  *
  * Removes the blackbox associated with the address.
  * @param state LC3State object.
  * @param addr address to remove blackbox from.
  * @return True if there was an error removing the blackbox false otherwise.
  */
bool lc3_remove_blackbox(lc3_state& state, unsigned short addr);
/** lc3_remove_watch
  *
  * Removes any watchpoints associated with the register/memory address.
  * @param state LC3State object.
  * @param is_reg True if data refers to a register otherwise false for a memory address.
  * @param data Register number or memory address.
  */
bool lc3_remove_watch(lc3_state& state, bool is_reg, unsigned short data);
/** lc3_remove_watch
  *
  * Removes any watchpoints associated with the symbol.
  * @param state LC3State object.
  * @param symbol symbol to remove watchpoint from
  */
bool lc3_remove_watch(lc3_state& state, const std::string& symbol);
/** lc3_break_test
  *
  * Tests if we have hit a breakpoint or a watchpoint.
  * @param state LC3State object.
  * @param changes Changes that was previously made to the LC3State as part of executing an instruction.
  * @return True if we have triggered a breakpoint/watchpoint.
  */
bool lc3_break_test(lc3_state& state, const lc3_state_change* changes);
/** lc3_blackbox_test
  *
  * Tests if we have hit a blackbox.
  * @param state LC3State object.
  * @return True if we have triggered a blackbox.
  */
bool lc3_blackbox_test(lc3_state& state);

#endif
