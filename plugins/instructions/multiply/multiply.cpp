#include "multiply.hpp"

static Plugin* instance = NULL;

Plugin* create_plugin(const PluginParams& params)
{
    if (!instance)
        instance = new MultiplyPlugin();
    return instance;
}

void destroy_plugin(Plugin* ptr = NULL)
{
    if (ptr == instance)
    {
        delete instance;
        instance = NULL;
    }
}

/** @brief GetOpcode
  *
  * @todo: document this function
  */
std::string MultiplyPlugin::GetOpcode() const
{
    return std::string("MUL");
}

/** @brief DoAssembleOne
  *
  * @todo: document this function
  */
unsigned short MultiplyPlugin::DoAssembleOne(lc3_state& state, LC3AssembleContext& context)
{
    size_t pos = context.line.find_first_of(" \t");
    std::string opcode = context.line.substr(0, pos);
    trim(opcode);
    std::string line = (pos == std::string::npos) ? "" : context.line.c_str() + pos + 1;
    trim(line);
    tokenize(context.line, context.tokens, " \t,");

    std::vector<std::string> tokens;
    tokenize(line, tokens, ",");
    // Remove spaces
    for (unsigned int i = 0; i < tokens.size(); i++)
        trim(tokens[i]);

    if (tokens.size() < 3)
    {
        THROWANDDO(LC3AssembleException(context.line, "", SYNTAX_ERROR, context.lineno), return 0);
    }
    else if (tokens.size() > 3)
    {
        THROW(LC3AssembleException(context.line, "", EXTRA_INPUT, context.lineno));
    }

    bool is_reg;
    int dr, sr1, sr2_imm;
    unsigned short instruction = 0xD000;

    dr = get_register(tokens[0], context);
    sr1 = get_register(tokens[1], context);
    sr2_imm = get_register_imm5(tokens[2], is_reg, context);
    instruction |= (dr << 9) | (sr1 << 6) | (!is_reg << 5) | sr2_imm;

    return instruction;
}

/** @brief OnDecode
  *
  * @todo: document this function
  */
void MultiplyPlugin::OnDecode(lc3_state& state, unsigned short data, lc3_instr& instr)
{
    // ARITH FORMAT INSTRUCTION
    instr.arith.imm.dr = (data >> 9) & 0x7;
    instr.arith.imm.sr1 = (data >> 6) & 0x7;
    instr.arith.imm.is_imm = (data >> 5) & 0x1;
    if (instr.arith.imm.is_imm)
        instr.arith.imm.imm = data & 0x1F;
    else
        instr.arith.reg.sr2 = data & 0x7;
}

/** @brief OnExecute
  *
  * format for this is similar to ADD/AND
  */
void MultiplyPlugin::OnExecute(lc3_state& state, lc3_instr& instruction, lc3_state_change& changes)
{
    // Hey DR will change here save it
    changes.changes = LC3_REGISTER_CHANGE;
    changes.location = instruction.arith.imm.dr;
    changes.value = state.regs[changes.location];
    // Two modes immediate value and registers
    if (instruction.arith.imm.is_imm)
    {
        state.regs[changes.location] = state.regs[instruction.arith.imm.sr1] * instruction.arith.imm.imm;
    }
    else
    {
        state.regs[changes.location] = state.regs[instruction.arith.reg.sr1] *
                                       state.regs[instruction.arith.reg.sr2];
    }
    // Update NZP
    lc3_setcc(state, state.regs[changes.location]);
}

const char* MUL_DISASSEMBLE_LOOKUP[2] =
{
    "MUL R%d, R%d, R%d",
    "MUL R%d, R%d, #%d"
};

const char* MUL_ADVANCED_DISASSEMBLE[7] =
{
    "TEST R%d",         // MUL R0, R0, 1
    "R%d = R%d",        // MUL R0, R1, 1
    "R%d = 0",          // MUL R0, R0, 0
    "R%d *= %s%d",      // MUL R0, R0, 3
    "R%d = R%d * %s%d", // MUL R0, R1, 3
    "R%d *= R%d",       // MUL R0, R0, R1
    "R%d = R%d * R%d"   // MUL R0, R1, R2
};


/** @brief OnDisassemble
  *
  * @todo: document this function
  */
std::string MultiplyPlugin::OnDisassemble(lc3_state& state, lc3_instr& instr, unsigned int level)
{
    char buf[128];
    short data;

    // No difference between basic and normal
    if (level == LC3_BASIC_DISASSEMBLE || level == LC3_NORMAL_DISASSEMBLE)
    {
        if (instr.arith.imm.is_imm)
        {
            sprintf(buf, MUL_DISASSEMBLE_LOOKUP[1], instr.arith.imm.dr, instr.arith.imm.sr1, instr.arith.imm.imm);
        }
        else
        {
            sprintf(buf, MUL_DISASSEMBLE_LOOKUP[0], instr.arith.reg.dr, instr.arith.reg.sr1, instr.arith.reg.sr2);
        }
    }
    else if (level == LC3_ADVANCED_DISASSEMBLE)
    {
        if (instr.arith.imm.is_imm)
        {
            // Data is imm
            data = instr.arith.imm.imm;
            // TEST = MUL RX, RX, 1
            if (data == 1 && instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_TEST], instr.arith.imm.dr);
            // SET = MUL RX, RY, 1
            else if (data == 1 && instr.arith.imm.dr != instr.arith.imm.sr1)
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_SET], instr.arith.imm.dr, instr.arith.imm.sr1);
            // CLEAR = MUL RX, RANY, 0
            else if (data == 0)
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_ZERO], instr.arith.imm.dr);
            // MUL_EQ = MUL RX, RX, NUM
            else if (instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_REG_NUM], instr.arith.imm.dr, SIGN_NEG_CHR(data), ABS(data));
            // NORMAL MUL IMM
            else
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_NUM], instr.arith.imm.dr, instr.arith.imm.sr1, SIGN_NEG_CHR(data), ABS(data));
        }
        else
        {
            // DR is the data here
            data = instr.arith.reg.dr;

            // MUL EQ
            if (data == instr.arith.reg.sr1 || data == instr.arith.reg.sr2)
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_EQ_REG], data, OTHER(data, instr.arith.reg.sr1, instr.arith.reg.sr2));
            // NORMAL MUL
            else
                sprintf(buf, MUL_ADVANCED_DISASSEMBLE[MUL_TWO_REGS], data, instr.arith.reg.sr1, instr.arith.reg.sr2);
        }
    }
    return buf;
}

const RLEColorEntry mulColorings[2][4] =
{
    {{"DR_COLOR", 96, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3}, {"UNUSED_BITS_COLOR", 0, 0, 0, 1}, {"IMM_COLOR", 0, 0, 80, 5}}, // IMM Version
    {{"DR_COLOR", 96, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3}, {"UNUSED_BITS_COLOR", 0, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3}},  // REG Version
};

std::vector<RLEColorEntry> MultiplyPlugin::GetInstructionColoring(unsigned short instr) const
{
    // Just like the ADD instruction
    std::vector<RLEColorEntry> answer;

    if (instr >> 5 & 0x1)
        answer.assign(mulColorings[0], mulColorings[0] + 4);
    else
        answer.assign(mulColorings[1], mulColorings[1] + 4);

    return answer;
}


