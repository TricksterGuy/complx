#include "comp.hpp"
#include <iostream>


int main(int argc, char** argv){	
	lc3_state state;
	std::cout << "Starting comp. Type 'help' for a full list of commands" << std::endl;
	if(argc > 1){
		start_simulator(state, std::string(argv[1]));
	}
	else{
		start_simulator(state);
	}
	//For the sake of example. Could be default view, I guess.
	print_lc3_state_and_info(state, 0x3000, 0x3010);

	run_simulation(state);//Use return value as error code, deliver message, maybe

	return 0;
}
