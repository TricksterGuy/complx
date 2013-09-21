#ifndef LC3_DEBUG_HPP
#define LC3_DEBUG_HPP

#include "lc3.hpp"

/*
===================
Debugging Functions
===================
*/
/** Adds a breakpoint at the given symbol */
bool lc3_add_break(lc3_state& state, const std::string& symbol, const std::string& label = "", const std::string& condition = "1", int times = -1);
/** Adds a breakpoint at the given address */
bool lc3_add_break(lc3_state& state, unsigned short addr, const std::string& label = "", const std::string& condition = "1", int times = -1);
/** Adds a blackbox at the given symbol */
bool lc3_add_blackbox(lc3_state& state, const std::string& symbol, const std::string& label = "", const std::string& condition = "1");
/** Adds a blackbox at the given address */
bool lc3_add_blackbox(lc3_state& state, unsigned short addr, const std::string& label = "", const std::string& condition = "1");
/** Adds a watchpoint for the given register/address */
bool lc3_add_watch(lc3_state& state, bool is_reg, unsigned short data, const std::string& condition, const std::string& label = "", int times = -1);
/** Adds a watchpoint for the given symbol */
bool lc3_add_watch(lc3_state& state, const std::string& symbol, const std::string& condition, const std::string& label = "", int times = -1);
/** Makes the given symbol a subroutine with params */
bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name, const std::vector<std::string>& param_names);
/** Makes the given symbol a subroutine */
bool lc3_add_subroutine(lc3_state& state, const std::string& symbol, const std::string& name = "", int num_params = 0);
/** Adds a subroutine with param names at the given address */
bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name, const std::vector<std::string>& param_names);
/** Makes the given address a subroutine */
bool lc3_add_subroutine(lc3_state& state, unsigned short address, const std::string& name = "", int num_params = 0);

/** Removes a breakpoint at the given symbol */
bool lc3_remove_break(lc3_state& state, const std::string& symbol);
/** Remove a breakpoint at the given address */
bool lc3_remove_break(lc3_state& state, unsigned short addr);
/** Removes a blackbox at the given symbol */
bool lc3_remove_blackbox(lc3_state& state, const std::string& symbol);
/** Remove a blackbox at the given address */
bool lc3_remove_blackbox(lc3_state& state, unsigned short addr);
/** Removes a watchpoint for the given register/address */
bool lc3_remove_watch(lc3_state& state, bool is_reg, unsigned short data);
/** Removes a watchpoint for the given symbol */
bool lc3_remove_watch(lc3_state& state, const std::string& symbol);

/** Tests all breakpoints and watchpoints */
bool lc3_break_test(lc3_state& state, const lc3_state_change* changes);
/** Tests for blackboxes */
bool lc3_blackbox_test(lc3_state& state);;

#endif
