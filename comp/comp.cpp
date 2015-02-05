#include <iostream>
#include <string>
#include "comp.hpp"
#include <stdio.h>

using namespace std;




/*Based on complx. I have no idea what I'm doing yet */
//lc3_state state;
string current_filename;// = string();

string last_command;// = string();



void start_simulator(lc3_state& state){
	lc3_init(state, true);
}

void start_simulator(lc3_state& state, const string& filename){
	start_simulator(state);
	current_filename = filename;
	assemble_load_asm_file(state, filename);

	//cout << "Loaded file " << filename << endl;
	//TODO - figure out how to understand input format, assemble if necessary; Actually load the file
}

void assemble_load_asm_file(lc3_state& st, const string& fname){
	try{
		lc3_assemble(st, fname);
		current_filename = fname;
	}
	catch(LC3AssembleException e){
		current_filename = "";
		cout << e.what() <<endl;
	}

}


/*
TODO - Formatted Hex function for 16-bit numbers. Would make everything prettier
	 - Add ostream parameter to print for better reusability
*/
void print_lc3_state(lc3_state& state, int lower_bound, int upper_bound, int level){
	cout << "ADDRESS\t" << "VALUE\t"<<"INSTRUCTION"<<endl;
	for(int i = lower_bound; i < upper_bound; i++){
		cout<< "0x"<< hex << i <<"\t" << "0x" << hex << state.mem[i]
			<<"\t";
		switch(level){
			case 0:
				cout <<lc3_basic_disassemble(state, state.mem[i]);
				break;
			case 1:
				cout << lc3_disassemble(state, state.mem[i]);
				break;
			case 2:
				cout << lc3_smart_disassemble(state, state.mem[i]);
		}
		//<< lc3_disassemble(state, state.mem[i])<<endl;
		cout << endl;
	}
	cout << endl;

	//print_lc3_info(state);
}

void print_lc3_state_and_info(lc3_state& state, int lower_bound, int upper_bound){
	print_lc3_state(state, lower_bound, upper_bound);
	print_lc3_info(state);
}

void print_lc3_info(lc3_state& state){
	for(int i = 0; i < 8; i++){
		cout << "R" << i <<": 0x" << hex << state.regs[i] <<"\t";
		if(i==3) cout << endl;
	}
	cout << endl;

	cout << "PC: 0x" << hex << state.pc
		<< "\tcc: "<<get_active_condition(state)<<endl;
}

/*
	TODO - Return result system
	e.g. 0 - continue; 1 - exit; -1 - error
*/

int run_simulation(lc3_state& state){

	bool running = true;
	string cmd;

	while(running){
		cout << "Enter Command(s): ";
		getline(cin,cmd);
		if(cmd=="") cmd = "repeat";

		running = run_cmds(state, cmd);
	}

	return 0;
}

bool run_cmds(lc3_state& state, const string& cmd){
	vector<string> v;

	tokenize(cmd, v, ";");
	bool ret = run_cmds(state, v);
	if(cmd!="repeat") last_command = cmd;
	return ret;
}

bool run_cmds(lc3_state& state, vector<string> & commands){
	for(int i = 0; (unsigned int)i < commands.size()/*getting rid of errors*/; i++){
		trim(commands[i]);
		int parse_value = parse_command(state, commands[i]);
		if(parse_value == -1){
			return false;
		}
		else if(parse_value != 0){
			string cmd_to_repeat;
			if(i != 0){//If repeat is first command, use last executed; If it is not (case here), repeat last command in the vector
				cmd_to_repeat = commands[i-1];
			}
			else{
				cmd_to_repeat = last_command;
			}
			cout <<"Repeating '" << cmd_to_repeat << "'"<< endl;
			for(int j = 0; j < parse_value; j++){
				run_cmds(state, cmd_to_repeat);
			}
		}
	}
	return true;
}


/*
	Run commands on state variable. Use return value to communicate intent to exit
	TODO - decide on an efficient/elegant way of interpreting string

*/

int parse_command(lc3_state& state, const string& command){
	vector<string> cmd_args;
	tokenize(command, cmd_args);
	string cmd = cmd_args[0];
	try{
		if(cmd == "quit" || cmd == "exit"){
			cout << "Exiting comp" << endl;
			return -1;
		}

		/*EXECUTION*/
		else if(cmd == "run"){
			cout<<"running"<<endl;//TODO See if there is a way to interrupt running without killing whole program.
			lc3_run(state);
		}
		/*
			NOTE - This code desperately needs exception handling
		*/
		else if(cmd=="step" || cmd == "back" || cmd=="next" || cmd=="prev"){
			parse_execution_op(state, cmd, cmd_args.size()>1?cmd_args[1]:"");
		}

		else if(cmd == "finish"){
			//cout << "finishing" << endl;//TODO
			run_execution_op(state, lc3_finish, 1);
		}
		else if(cmd == "rewind"){
			//cout << "rewinding" << endl;//TODO
			run_execution_op(state, lc3_rewind,1);
		}

		/* DEBUGGING */
		else if(cmd == "temp"){
			if(cmd_args.size()==2){
				//TODO - convert address to short
				short addr = 0x3000;
				lc3_add_break(state, addr);
			}
			else{
				cout << "Usage: temp addr" <<endl;
			}
		}

		/* MANIPULATION */
		else if(cmd == "set"){

			if(cmd_args.size() < 3){
				cout << "Usage: set thing value" << endl;
			}
			else{
				set_cmd(state, cmd_args[1],cmd_args[2]);

			}
		}

		else if(cmd == "reset"){
			lc3_init(state);
		}

		else if(cmd == "randomize" || cmd == "randomise"){
			lc3_randomize(state);
		}

		else if(cmd == "true_traps"){
			if(cmd_args.size()==1){
				state.true_traps = ~state.true_traps;
				const string& val = state.true_traps ? "true": "false";
				cout << "Set true_traps to " << val <<endl;
			}
			else{
				if(cmd_args[1]=="true"){
					state.true_traps = 1;
				}
				else if(cmd_args[1] == "false"){
					state.true_traps = 0;
				}

				else{
					cout << "Usage: true_traps [true | false]" <<endl;
				}
			}
		}

		else if(cmd=="interrupt" || cmd == "interrupts"){
			if(cmd_args.size()==1){
				state.interrupt_enabled = ~state.true_traps;
				const string& val = state.interrupt_enabled ? "true": "false";
				cout << "Set interrupts enabled to " << val <<endl;
			}
			else{
				if(cmd_args[1]=="true"){
					state.interrupt_enabled = 1;
				}
				else if(cmd_args[1] == "false"){
					state.interrupt_enabled = 0;
				}

				else{
					cout << "Usage: interrupt [true | false]" <<endl;
				}
			}
		}

		/* DISPLAY */
		else if(cmd == "print"){
			//cout << cmd_args.size()<<endl;
			if(cmd_args.size()==1)
				print_lc3_state_and_info(state, (int)state.pc, (int)state.pc + 0x10);
			else if(cmd_args.size()==2){

				if((cmd_args[1].c_str()[0]=='R' || cmd_args[1].c_str()[0]=='r') && cmd_args[1].size()== 2 && cmd_args[1].c_str()[1] >= '0' && cmd_args[1].c_str()[1] <= '7')//TODO - length, range check #laziness
					cout << "R" << cmd_args[1].c_str()[1] << ": 0x" << state.regs[cmd_args[1].c_str()[1]-'0'] <<endl;
				else if(cmd_args[1]=="pc" || cmd_args[1]=="PC")//TODO - string insensitive comparison for a lot of this stuff
					cout << "PC: 0x" << state.pc<<endl;
				else if(cmd_args[1].substr(0,2)=="0x"){
					unsigned short addr = (unsigned short)parse_set_value(cmd_args[1]);

					cout <<cmd_args[1]<<" : 0x"<< state.mem[addr]<<" - "<<lc3_disassemble(state, state.mem[addr])<<endl; //todo
				}
				else if(cmd_args[1]=="CC" || cmd_args[1]=="cc" || cmd_args[1]=="nzp")
					cout << "cc: "<<get_active_condition(state) <<endl;
				else
					goto invalid_comp;
			}
			else{
				invalid_comp: cout << "Invalid component '"<< cmd_args[1] <<"'"<<endl;
			}
		}

		else if(cmd=="list"){
			if(cmd_args.size()==1){
				print_lc3_state(state, (int)state.pc, (int)state.pc + 0x10);
			}
			/*
			else if(cmd_args.size()==2){
				//TODO
			}
			*/
			else{
				string list_args;
				for(unsigned int i = 1; i < cmd_args.size(); i++)
					list_args +=cmd_args[i];
				run_list_cmd(state, list_args);
			}
		}


		else if(cmd == "info"){
			print_lc3_info(state);
		}

		/* FILE */
		else if(cmd == "load"){
			if(cmd_args.size()<2)//BUG - ~/ isn't recognized as home directory, works otherwise
				cout << "Usage: load filename" << endl;
			else{
				current_filename = cmd_args[1];
				assemble_load_asm_file(state, current_filename);
			}
		}

		else if(cmd=="reload"){
			if(current_filename != ""){
				lc3_init(state);
				assemble_load_asm_file(state, current_filename);
			}
			else{
				cout << "No file to reload" << endl;
			}
		}

		/* OTHER */
		else if(cmd == "clear"){
			system("clear");
		}

		else if(cmd == "repeat"){
			int repeat_num = 1;
			if(cmd_args.size()>1){
				repeat_num = atoi(cmd_args[1].c_str());
			}
			return repeat_num;
		}


		else{
				cout << "Command '" << cmd_args[0] << "' not recognized" << endl;
		}
	}
	catch(LC3AssembleException e){
		current_filename = "";
		cout << e.what() <<endl;
	}
	catch(...){
		cout <<"Unknown exception! Sorry :("<<endl;
	}
	return 0;
}


void run_list_cmd(lc3_state& state, string& list_args){
	vector<string> arg_v;
	tokenize(list_args,arg_v, ",");
	for(unsigned int i = 0; i < arg_v.size(); i++){
		trim(arg_v[i]);
		if(arg_v[i].c_str()[arg_v[i].size()-1]==',')
			arg_v[i].erase(arg_v[i].end()-1, arg_v[i].end());

	}
	unsigned short start = (unsigned short)parse_set_value(arg_v[0]);
	if(start==0 && (false)){
		//TODO - error checking
	}
	if(arg_v.size()==1){
		print_lc3_state(state, start, start+1);
	}
	else if(arg_v.size()==2){
		if(arg_v[1].find(string("level"))!=string::npos){
			int level = arg_v[1].c_str()[arg_v[1].size()-1]-'0';
			print_lc3_state(state, start, start+1, level);
		}
		else{
			unsigned short end = (unsigned short)parse_set_value(arg_v[1]);
			if(end==0){
				//TODO - error checking
			}
			print_lc3_state(state, start, end+1,1);
		}
	}
	else{
		unsigned short end = (unsigned short)parse_set_value(arg_v[1]);
		if(end==0 &(false)){
			//TODO - error checking
		}
		int level = arg_v[2].c_str()[arg_v[2].size()-1]-'0';
		print_lc3_state(state, start, end, level);
	}

}

void parse_execution_op(lc3_state& state, const string& cmd, const string& interval){
	int numsteps;
	if(interval !=""){
		numsteps = atoi(interval.c_str());
	}
	else{
		numsteps = 1;
	}

	//cout << "running for "<<numsteps<<" steps"<< endl;//TODO
	if(numsteps < 1 /*|| state.pc + numsteps > 65535*/){
		cout << "Could not run "<<"'"<<cmd<<"' for interval "<<"'"<<interval<<"'"<<endl;
		return;
	}
	if(cmd=="step")
		run_execution_op(state, lc3_step, numsteps);
	else if(cmd=="back")
		run_execution_op(state,lc3_back,numsteps);
	else if(cmd=="next")
		run_execution_op(state,lc3_next_line,numsteps);
	else if(cmd=="prev")
		run_execution_op(state,lc3_prev_line,numsteps);
	else
		cout << "This should never happen!"<<endl;

}


/*
Takes in a state and a function from lc3_runner, runs it n times
*/
void run_execution_op(lc3_state& st, void (*op)(lc3_state&), int n){
	for(int i = 0; i < n; i++){
		op(st);
	}
}

/*
	-Convert anything starting with "0x" as a hexadecimal number
	-other numbers as decimal
	-TODO - exception handling
*/
short parse_set_value(const string& value){
	if(value.substr(0,2)==string("0x")){
		short ret;
		sscanf(value.c_str(),"%hx",&ret);
		return ret;
	}

	else{
		return (short)atoi(value.c_str());
	}
}

char get_active_condition(lc3_state& st){
	if(st.n) return 'n';
	else if(st.p) return 'p';
	else return 'z';
}

/*
	WARNING - so much could go wrong right now :(
*/
void set_cmd(lc3_state& state, const string& thing, const string& value){
	if(thing == string("cc") || thing == string("nzp")){
		//cout << "set cc here" << endl;
		char cc_char;
		if(value.size()!= 1){
			goto invalid_cc;
		}

		cc_char = value.c_str()[0];
		//Converts upper case chars to lower case. If the char is something other
		//than a letter, it won't work, but it doesn't matter here
		if(cc_char < 'a') cc_char += ('a'-'A');

		if(cc_char!='n' && cc_char!='z' && cc_char!='p'){
			invalid_cc:	cout << "invalid condition '"<< value <<"'"<<endl;
		}
		else{
			state.n = 0;
			state.z = 0;
			state.p = 0;
			if(cc_char == 'n'){
				state.n = 1;
			}
			else if(cc_char == 'p'){
				state.p = 1;
			}
			else if(cc_char == 'z'){
				state.z = 1;
			}
		}
	}

	else{
		/* TODO - learn C++ exceptions */
		short value_short = parse_set_value(value);
		//cout << "Value_short: "<<value_short<<endl;
		if(value_short == 0 && value != "0" && value != "0x0"){
			cout << "Could not set value '"<<value<<"'"<<endl;
			return;
		}

		if(thing[0] == 'r' || thing[0]== 'R'){
			if(thing.size() <3 && (thing[1]>='0' || thing[1] >= '9')){
				state.regs[thing[1] - '0'] = value_short;
			}
		}
		else if(thing == string("pc")){
			state.pc = (unsigned short)value_short;
		}
		else{
			unsigned short addr = (unsigned short)parse_set_value(thing);
			if(addr == 0 && thing!="0" && thing!="0x0" && thing!="0x0000"){//Not technically correct right now
				cout << "Could not set component '"<<thing<<"'"<<endl;
			}
			state.mem[addr] = value_short;
		}

	}
}
