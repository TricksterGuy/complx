#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

#include <lc3_all.hpp>

#define MULTIPLY_MAJOR_VERSION 1
#define MULTIPLY_MINOR_VERSION 3

class MultiplyPlugin : public InstructionPlugin
{
public:
    MultiplyPlugin() : InstructionPlugin(MULTIPLY_MAJOR_VERSION, MULTIPLY_MINOR_VERSION, "Multiplication Plugin") {}
    virtual std::string GetOpcode() const;
    virtual unsigned short DoAssembleOne(lc3_state& state, LC3AssembleContext& context);
    virtual void OnDecode(lc3_state& state, unsigned short data, lc3_instr& instr);
    virtual void OnExecute(lc3_state& state, lc3_instr& instruction, lc3_state_change& changes);
    virtual std::string OnDisassemble(lc3_state& state, lc3_instr& instr, unsigned int level);
    virtual std::vector<RLEColorEntry> GetInstructionColoring(unsigned short instr) const;
};


enum MUL_DISASSEMBLE_CASES
{
    MUL_TEST = 0,		// MUL RX, RX, 1
    MUL_SET = 1, 		// MUL RX, RY, 1
    MUL_ZERO = 2,		// MUL RX, R?, 0
    MUL_REG_NUM = 3,	// MUL RX, RX, ?
    MUL_NUM = 4,		// MUL RX, RY, ?
    MUL_EQ_REG = 5,		// MUL RX, RX, RY
    MUL_TWO_REGS = 6,	// MUL RX, RY, RZ
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

