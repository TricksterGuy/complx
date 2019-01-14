#ifndef MULTIPLY_HPP
#define MULTIPLY_HPP

#include <lc3.hpp>

#define MULTIPLY_MAJOR_VERSION 1
#define MULTIPLY_MINOR_VERSION 6

class MultiplyPlugin : public InstructionPlugin
{
public:
    MultiplyPlugin() : InstructionPlugin(MULTIPLY_MAJOR_VERSION, MULTIPLY_MINOR_VERSION, "Multiplication Plugin") {}
    std::string GetOpcode() const override;
    unsigned short DoAssembleOne(lc3_state& state, LC3AssembleContext& context) override;
    void OnDecode(lc3_state& state, unsigned short data, lc3_instr& instr) override;
    void OnExecute(lc3_state& state, lc3_instr& instruction, lc3_state_change& changes) override;
    std::string OnDisassemble(lc3_state& state, lc3_instr& instr, unsigned int level) override;
    std::list<RLEColorEntry> GetInstructionColoring(unsigned short instr) const override;
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

extern "C" LC3_API Plugin* create_plugin(const PluginParams& params);
extern "C" LC3_API void destroy_plugin(Plugin* ptr);

#endif

