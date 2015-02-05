#ifndef COMP_HPP
#define COMP_HPP

#include <lc3.hpp>

/*start simulator with nothing loaded into memory*/
void start_simulator(lc3_state& state);
/*load file into simulator, then start simulator*/
void start_simulator(lc3_state& state, const std::string& filename);
/*prints memory view from address lower_bound to upper_bound, registers, PC, and CC*/
void print_lc3_state(lc3_state& state, int lower_bound, int upper_bound, int level = 1);

bool run_cmds(lc3_state& state, std::vector<std::string>& commands);

bool run_cmds(lc3_state& state, const std::string& cmd);

void print_lc3_info(lc3_state& state);
/*simulation main loop*/
int run_simulation(lc3_state& state);
/*interprets commands, runs procedures based on them*/
int parse_command(lc3_state& state, const std::string& command);
/*Command: "set thing value" e.g. set pc 0x3000 */
void set_cmd(lc3_state& state, const std::string& thing, const std::string& value);
/*interprets value string for set commands, returns appropriate value*/
short parse_set_value(const std::string& value);
/*Runs op on state n times*/
void run_execution_op(lc3_state& state, void (*op)(lc3_state&), int n);

void parse_execution_op(lc3_state& st, const std::string& cmd, const std::string& interval);

void assemble_load_asm_file(lc3_state& st, const std::string& fname);
/*Returns n, z, or p*/
char get_active_condition(lc3_state& st);

void run_list_cmd(lc3_state& state, std::string& list_args);

void print_lc3_state_and_info(lc3_state& state, int lower_bound, int upper_bound);
#endif