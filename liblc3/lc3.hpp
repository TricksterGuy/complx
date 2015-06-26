#ifndef LC3_HPP
#define LC3_HPP

#include <string>
#include <fstream>
#include <deque>
#include <map>
#include <list>
#include <vector>
#include <cstdlib>

#define SIGN_CHR(x) (((x) < 0) ? "-" : "+")
#define SIGN_NEG_CHR(x) (((x) < 0) ? "-" : "")
#define ABS(x) (((x) < 0) ? -(x) : (x))

// Given 3 values when 2 of them are equal
// Returns the not equal one
// That is given 3, 7, 3 this will return 7
#define OTHER(x, y, z) ((x) ^ (y) ^ (z))

enum lc3_instruction_t
{
    BR_INSTR    = 0,
    ADD_INSTR   = 1,
    LD_INSTR    = 2,
    ST_INSTR    = 3,
    JSR_INSTR   = 4,
    JSRR_INSTR  = 4,
    AND_INSTR   = 5,
    LDR_INSTR   = 6,
    STR_INSTR   = 7,
    RTI_INSTR   = 8,
    NOT_INSTR   = 9,
    LDI_INSTR   = 10,
    STI_INSTR   = 11,
    RET_INSTR   = 12,
    JMP_INSTR   = 12,
    ERROR_INSTR = 13,
    LEA_INSTR   = 14,
    TRAP_INSTR  = 15
};

enum TRAPS
{
    TRAP_GETC = 0x20,
    TRAP_OUT = 0x21,
    TRAP_PUTS = 0x22,
    TRAP_IN = 0x23,
    TRAP_PUTSP = 0x24,
    TRAP_HALT = 0x25
};

enum DEVICES
{
    DEV_KBSR = 0xFE00,
    DEV_KBDR = 0xFE02,
    DEV_DSR = 0xFE04,
    DEV_DDR = 0xFE06,
    DEV_MCR = 0xFFFE
};

enum WARNINGS
{
    LC3_OUT_OF_INPUT = 0,
    LC3_RESERVED_MEM_WRITE = 1,
    LC3_RESERVED_MEM_READ = 2,
    LC3_UNSUPPORTED_TRAP = 3,
    LC3_UNSUPPORTED_INSTRUCTION = 4,
    LC3_USER_RTI = 5,
    LC3_INVALID_CHARACTER_WRITE = 6,
    LC3_PUTS_INVALID_MEMORY = 7,
    LC3_DISPLAY_NOT_READY = 8,
    LC3_KEYBOARD_NOT_READY = 9,
    LC3_TURN_OFF_VIA_MCR = 10,
    LC3_WARNINGS               // Must be last.
};

enum DISASSEMBLE_CASES
{

    BR_NOP = 0,
    BR_NORM_LABEL = 1,
    BR_NZP_LABEL = 2,
    BR_NORM_OFF = 3,
    BR_NZP_OFF = 4,

    BR_N = 1,
    BR_Z = 2,
    BR_P = 4,
    BR_NZ = 3,
    BR_NP = 5,
    BR_ZP = 6,



    ADD_EQ_REG = 0,
    ADD_TWO_REGS = 1,
    ADD_TEST = 2,
    ADD_SET = 3,
    ADD_INC = 4,
    ADD_DEC = 5,
    ADD_EQ_VAL = 6,
    ADD_REG_VAL = 7,



    // Used for LD,LDI,ST,STI,LEA
    MEM_LABEL = 0,
    MEM_OFFSET = 1,
    MEM_OFFSET_0 = 2,


    JSR_LABEL = 0,
    JSR_OFFSET = 1,
    JSR_OFFSET_0 = 3,
    JSR_JSRR = 2,


    AND_TEST = 0,
    AND_SET = 1,
    AND_EQ_REG = 2,
    AND_TWO_REGS = 3,
    AND_ZERO = 4,
    AND_NUM = 5,
    AND_REG_NUM = 6,


    LDR_STR = 0,
    JMP_REG = 0,
    JMP_R7 = 1,

    TRAP_OTHER = 6
};

enum DISASSEMBLE_LEVELS
{
    LC3_BASIC_DISASSEMBLE,
    LC3_NORMAL_DISASSEMBLE,
    LC3_ADVANCED_DISASSEMBLE
};

// Plugins forward declaration
class Plugin;
class InstructionPlugin;
class DeviceRegisterPlugin;
class TrapFunctionPlugin;
struct PluginInfo;

// AND, ADD, NOT
typedef struct arithreg_instruction
{
    unsigned short opcode:4;
    unsigned short dr:3;
    unsigned short sr1:3;
    unsigned short is_imm:1;
    unsigned short unused:2;
    unsigned short sr2:3;
} arithreg_instruction;

typedef struct arithimm_instruction
{
    unsigned short opcode:4;
    unsigned short dr:3;
    unsigned short sr1:3;
    unsigned short is_imm:1;
    short imm:5;
} arithimm_instruction;

typedef struct not_instruction
{
    unsigned short opcode:4;
    unsigned short dr:3;
    unsigned short sr1:3;
    unsigned short unused:6;
} not_instruction;

typedef union arith_instruction
{
    arithimm_instruction imm;
    arithreg_instruction reg;
    not_instruction inv;
} arith_instruction;

// BR
typedef struct br_instruction
{
    unsigned short opcode:4;
    unsigned short n:1;
    unsigned short z:1;
    unsigned short p:1;
    short pc_offset:9;
} br_instruction;

// JMP RET
typedef struct jmp_instruction
{
    unsigned short opcode:4;
    unsigned short unused_3:3;
    unsigned short base_r:3;
    unsigned short unused_6:6;
} jmp_instruction;

// JSR JSRR
typedef struct jsr_instruction
{
    unsigned short opcode:4;
    unsigned short is_jsr:1;
    short pc_offset:11;
} jsr_instruction;

typedef struct jsrr_instruction
{
    unsigned short opcode:4;
    unsigned short is_jsr:1;
    unsigned short unused_2:2;
    unsigned short base_r:3;
    unsigned short unused_6:6;
} jsrr_instruction;

typedef union subr_instruction
{
    jsr_instruction jsr;
    jsrr_instruction jsrr;
} subr_instruction;

// LD LDI LEA STI ST
typedef struct memoryoffset_instruction
{
    unsigned short opcode:4;
    unsigned short reg:3;
    short pc_offset:9;
} memoryoffset_instruction;

// LDR STR
typedef struct memoryreg_instruction
{
    unsigned short opcode:4;
    unsigned short reg:3;
    unsigned short base_r:3;
    short offset:6;
} memoryreg_instruction;

typedef union memory_instruction
{
    memoryoffset_instruction offset;
    memoryreg_instruction reg;
} memory_instruction;

typedef struct trap_instruction
{
    unsigned short opcode:4;
    unsigned short unused:4;
    unsigned short vector:8;
} trap_instruction;

typedef struct instruction_t
{
    unsigned short opcode:4;
    unsigned short data:12;
} instruction_t;

typedef arith_instruction arith_instr;
typedef arithreg_instruction arithreg_instr;
typedef arithimm_instruction arithimm_instr;
typedef not_instruction not_instr;
typedef memory_instruction mem_instr;
typedef memoryreg_instruction memoryreg_instr;
typedef memoryoffset_instruction memoryoffset_instr;
typedef br_instruction br_instr;
typedef jmp_instruction jmp_instr;
typedef jsr_instruction jsr_instr;
typedef jsrr_instruction jsrr_instr;
typedef subr_instruction subr_instr;
typedef trap_instruction trap_instr;

/**
  * Lc3 instruction type
  * contains the data for the different types of instructions
  * Note: the bits field != the instruction as data
  * The bits field gets it as how the system is representing
  * it in memory not the actual LC3 view of it.
  * That is TRAP x25 = 0xF025 but will appear in the
  * system as 0x250F
  */
typedef union lc3_instr
{
    arith_instr arith;
    br_instr    br;
    jmp_instr   jmp;
    subr_instr  subr;
    mem_instr   mem;
    trap_instr  trap;
    instruction_t rti;
    instruction_t data;
    unsigned short bits;
} lc3_instr;

enum lc3_change_t
{
    LC3_NO_CHANGE = 0,
    LC3_REGISTER_CHANGE = 1,
    LC3_MEMORY_CHANGE = 2,
    LC3_MULTI_CHANGE = 3,       // Multiple registers / memory addresses or both have changed.  For plugins.
    LC3_SUBROUTINE_BEGIN = 4,
    LC3_SUBROUTINE_END = 5,
    LC3_INTERRUPT_BEGIN = 6,    // Signals begin of interrupt can't backstep past this. (this will be in the undo stack while the interrupt is handled)
    LC3_INTERRUPT_END = 7,      // Signals end of interrupt (this will never be in the undo stack)
    LC3_INTERRUPT = 8,          // Signals a processed interrupt. (LC3_INTERRUPT_BEGIN changes to this after its processed.
};

typedef struct lc3_change_info
{
    bool is_reg;
    unsigned short location;
    unsigned short value;
} lc3_change_info;

typedef struct lc3_breakpoint_info
{
    bool enabled;
    unsigned short addr;
    int max_hits;
    int hit_count;
    std::string label;
    std::string condition;
} lc3_breakpoint_info;

typedef struct lc3_watchpoint_info
{
    bool enabled;
    bool is_reg;
    unsigned short data;
    int max_hits;
    int hit_count;
    std::string label;
    std::string condition;
} lc3_watchpoint_info;

typedef struct lc3_blackbox_info
{
    bool enabled;
    unsigned short addr;
    int hit_count;
    std::string label;
    std::string condition;
} lc3_blackbox_info;

typedef struct lc3_subroutine_info
{
    unsigned short address;
    std::string name;
    int num_params;
    // Will be empty if not used.
    std::vector<std::string> params;
} lc3_subroutine_info;

typedef struct lc3_subroutine_call
{
    unsigned short address;
    unsigned short r6;
    bool is_trap;
} lc3_subroutine_call;

typedef struct lc3_memory_stats
{
    lc3_memory_stats() : reads(0), writes(0) {}
    unsigned long reads;
    unsigned long writes;
} lc3_memory_stats;

typedef struct lc3_interrupt_req
{
    unsigned char priority:3;
    unsigned char vector;
} lc3_interrupt_req;

typedef struct lc3_state_change
{
    unsigned short pc;
    short r7; /* In the case of fake traps two registers can be modified. So we have a special place for r7*/
    unsigned char n:1;
    unsigned char z:1;
    unsigned char p:1;
    unsigned char halted:1;
    unsigned char changes:4;
    unsigned short location;
    unsigned short value;
    unsigned int warnings;
    unsigned int executions;            // Only used for changes = LC3_INTERRUPT(_BEGIN) otherwise we know its changed by 1.
    lc3_subroutine_call subroutine;     // Only used for changes = LC3_SUBROUTINE_*
    std::vector<lc3_change_info> info;  // Only used for changes = LC3_MULTI_CHANGE
} lc3_state_change;

typedef void (*interrupt_test_func)(void);

typedef struct lc3_state
{
    short regs[8];
    unsigned short pc;
    unsigned char privilege:1;
    unsigned char priority:3;
    unsigned char n:1;
    unsigned char z:1;
    unsigned char p:1;
    unsigned char halted:1;
    unsigned char true_traps:1;
    unsigned char interrupt_enabled:1;
    unsigned int warnings;
    unsigned int executions;

    std::map<std::string, unsigned short> symbols;
    std::map<unsigned short, std::string> rev_symbols;

    short mem[65536];

    // Stream for input
    std::istream* input;
    int (*reader)(lc3_state& state, std::istream& file); // Function to read and consume one character from stream
    int (*peek)(lc3_state& state, std::istream& file); // Function to peek at next character in stream

    // Stream for output
    std::ostream* output;
    int (*writer)(lc3_state& state, std::ostream& file, int); // Function to write one character to stream

    // Stream for warnings
    std::ostream* warning;

    // Plugins loaded into the state
    InstructionPlugin* instructionPlugin;
    std::map<unsigned char, TrapFunctionPlugin*> trapPlugins;
    std::map<unsigned short, DeviceRegisterPlugin*> devicePlugins;
    std::vector<Plugin*> plugins;

    // Plugin handle information
    std::map<std::string, PluginInfo> filePlugin;

    // Plugin interrupt information
    std::map<unsigned char, Plugin*> interruptPlugin;

    // Maximum stack size just here for people who like to infinite loop/recurse and don't want their computers to explode.
    unsigned int max_stack_size;
    /* I did not use a stack since I can't remove from front if max stack size != 0
       So treat this as a "stack" */
    std::deque<lc3_state_change> undo_stack;

    // Maximum call stack size just here for people who like to infinite loop/recurse and don't want their computers to explode.
    unsigned int max_call_stack_size;
    // Subroutine debugging info (again see note above)
    std::deque<lc3_subroutine_call> call_stack;

    // Interrupt support push things here to cause interrupt
    std::list<lc3_interrupt_req> interrupts;
    std::list<interrupt_test_func> interrupt_test; // Functions to be called at the end of each step.
    int interrupt_vector; // Current interrupt vector that is being handled
    std::deque<int> interrupt_vector_stack; // Current stack of interrupts being handled.
    unsigned short savedusp;
    unsigned short savedssp;

    // Debugging information.
    std::map<unsigned short, lc3_breakpoint_info> breakpoints;
    std::map<unsigned short, lc3_blackbox_info> blackboxes;
    std::map<unsigned short, lc3_watchpoint_info> mem_watchpoints;
    std::map<unsigned short, lc3_watchpoint_info> reg_watchpoints;
    std::map<unsigned short, std::string> comments;
    std::map<unsigned short, lc3_subroutine_info> subroutines;

    // Statistics
    std::map<unsigned short, lc3_memory_stats> memory_ops;
    unsigned long total_reads;
    unsigned long total_writes;

    // test_only mode
    // The only effect is that it does not pop subroutine calls from stack (max_call_stack_size is ignored)
    bool in_lc3test;
} lc3_state;

/*
=======================
Disassembling Functions
=======================
*/
/** Disassemble the data passed in no label information a straight disassemble */
std::string lc3_basic_disassemble(lc3_state& state, unsigned short data);
/** Disassemble the data passed in with label information */
std::string lc3_disassemble(lc3_state& state, unsigned short data);
/** Disassembles the data passed into it You need symbol information to get the best results */
std::string lc3_smart_disassemble(lc3_state& state, unsigned short data);

/*
======================
Loading Data Functions
======================
*/
/** Loads the given file into the machine */
int lc3_load(lc3_state& state, std::istream& file, int (*reader)(std::istream&));
/** Reader for .obj files */
int lc3_reader_obj(std::istream& file);
/** Reader for .hex files */
int lc3_reader_hex(std::istream& file);
/** Reads in a character from the input stream passed in */
int lc3_read_char(lc3_state& state, std::istream& file);
/** Gets (Peek) next character from the input stream passed in */
int lc3_peek_char(lc3_state& state, std::istream& file);
/** Writes in a character to the output stream passed in */
int lc3_write_char(lc3_state& state, std::ostream& file, int);
/** Writes a string to the output stream passed in */
int lc3_write_str(lc3_state& state, int (*writer)(lc3_state& state, std::ostream& file, int), std::ostream& file, const std::string& str);
/** Enables true traps loads the OS code into the lc3 */
void lc3_set_true_traps(lc3_state& state, int value);
/** Generate a random number LC-3 */
inline unsigned short lc3_random(void) {return rand() & 0xFFFF;}
/** Randomize LC-3 Memory */
void lc3_randomize(lc3_state& state);


#include "lc3_debug.hpp"
#include "lc3_execute.hpp"
#include "lc3_expressions.hpp"
#include "lc3_runner.hpp"
#include "lc3_symbol.hpp"
#include "lc3_plugin.hpp"
#include "lc3_assemble.hpp"
#include "lc3_test.hpp"

#endif
