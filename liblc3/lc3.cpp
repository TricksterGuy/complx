#include <iostream>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "lc3.hpp"
#include "lc3_execute.hpp"
#include "lc3_expressions.hpp"
#include "lc3_plugin.hpp"
#include "lc3_symbol.hpp"
#include "lc3_os.hpp"

const char* BASIC_DISASSEMBLE_LOOKUP[16][2] =
{
    {"BR%s%s%s #%d", "NOP%s"             },
    {"ADD R%d, R%d, R%d", "ADD R%d, R%d, #%d" },
    {"LD R%d, #%d", ""                  },
    {"ST R%d, #%d", ""                  },
    {"JSRR R%d", "JSR #%d"           },
    {"AND R%d, R%d, R%d", "AND R%d, R%d, #%d" },
    {"LDR R%d, R%d, #%d", ""                  },
    {"STR R%d, R%d, #%d", ""                  },
    {"RTI", ""                  },
    {"NOT R%d, R%d", ""                  },
    {"LDI R%d, #%d", ""                  },
    {"STI R%d, #%d", ""                  },
    {"JMP R%d", "RET"               },
    {"ERROR", ""                  },
    {"LEA R%d, #%d", ""                  },
    {"TRAP x%02x", ""                  },
};

const char* DISASSEMBLE_LOOKUP[16][3] =
{
    {"BR%s%s%s #%d", "BR%s%s%s %s", "NOP%s"           },
    {"ADD R%d, R%d, R%d", "ADD R%d, R%d, #%d", ""                },
    {"LD R%d, #%d", "LD R%d, %s", ""                },
    {"ST R%d, #%d", "ST R%d, %s", ""                },
    {"JSRR R%d", "JSR #%d", "JSR %s"          },
    {"AND R%d, R%d, R%d", "AND R%d, R%d, #%d", ""                },
    {"LDR R%d, R%d, #%d", "", ""                },
    {"STR R%d, R%d, #%d", "", ""                },
    {"RTI", "", ""                },
    {"NOT R%d, R%d", "", ""                },
    {"LDI R%d, #%d", "LDI R%d, %s", ""                },
    {"STI R%d, #%d", "STI R%d, %s", ""                },
    {"JMP R%d", "RET", ""                },
    {"ERROR", "", ""                },
    {"LEA R%d, #%d", "LEA R%d, %s", ""                },
    {"TRAP x%02X", "", ""                },
};

const char* TRAP_CASES[6] = {"GETC", "OUT", "PUTS", "IN", "PUTSP", "HALT"};

//nul(NOP), N, Z, NZ, P, NP, ZP, nul(NZP)
const char* BR_CASES[8] = {"", "<", "=", "<=", ">", "!=", ">=", ""};

const char* ADV_DISASSEMBLE_LOOKUP[16][8] =
{
    {"NOP%s", "if cc%s0, PC = %s", "PC = %s", "if cc%s0, PC %s= %d", "PC %s= %d"},
    {"R%d += R%d", "R%d = R%d + R%d", "TEST R%d", "R%d = R%d", "R%d++", "R%d--", "R%d %s= %d", "R%d = R%d %s %d"},
    {"R%d = %s", "R%d = (PC %s %d)", "R%d = PC"}, //LD
    {"%s = R%d", "(PC %s %d) = R%d", "PC = R%d"}, //ST
    {"CALL %s", "CALL PC %s %d", "CALL R%d", "CALL PC"}, //
    {"TEST R%d", "R%d = R%d", "R%d &= R%d", "R%d = R%d & R%d", "R%d = 0", "R%d &= %d", "R%d = R%d & %d"},
    {"R%d = R%d[%d]"},
    {"R%d[%d] = R%d"},
    {"RTI"},
    {"R%d = ~R%d"},
    {"R%d = *%s", "R%d = *(PC %s %d)", "R%d = *PC"}, // LDI
    {"*%s = R%d", "*(PC %s %d) = R%d", "*PC = R%d"}, // STI
    {"JUMP R%d", "RET"},
    {"ERROR"},
    {"R%d = &%s", "R%d = PC %s %d", "R%d = PC"}, // LEA
    {"GETC", "OUT", "PUTS", "IN", "PUTSP", "HALT", "TRAP x%02x"},
};

std::string lc3_basic_disassemble(lc3_state& state, unsigned short data)
{
    lc3_instr instr = lc3_decode(state, data);
    unsigned int opcode = instr.data.opcode;
    char buf[128];
    char minibuf[7];

    switch(opcode)
    {
    case BR_INSTR:
        // If all flags are off or offset is 0
        if (!(instr.br.n || instr.br.z || instr.br.p) || instr.br.pc_offset == 0)
        {
            if (!instr.br.n && !instr.br.z && !instr.br.p)
            {
                // Hey do I have a printable character?
                if (isprint(instr.br.pc_offset))
                {
                    // Write the character
                    snprintf(minibuf, 7, " ('%c')", instr.br.pc_offset);
                    // NOP (with character)
                    sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], minibuf);
                }
                else
                {
                    // NOP no character
                    sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], "");
                }
            }
            else
                // NOP2
                sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], "");
        }
        // If not all flags are on
        else if (!(instr.br.n && instr.br.z && instr.br.p))
        {
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.br.n ? "N" : "",
                    instr.br.z ? "Z" : "", instr.br.p ? "P" : "", instr.br.pc_offset);
        }
        // All flags are on then
        else
        {
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], "", "", "", instr.br.pc_offset);
        }
        break;
    case ADD_INSTR:
    case AND_INSTR:
        if (instr.arith.imm.is_imm)
        {
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1],
                    instr.arith.imm.dr, instr.arith.imm.sr1, instr.arith.imm.imm);
        }
        else
        {
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0],
                    instr.arith.reg.dr, instr.arith.reg.sr1, instr.arith.reg.sr2);
        }
        break;
    case NOT_INSTR:
        sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.arith.inv.dr, instr.arith.inv.sr1);
        break;
    case LD_INSTR:
    case ST_INSTR:
    case LEA_INSTR:
    case LDI_INSTR:
    case STI_INSTR:
        sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.mem.offset.reg,
                instr.mem.offset.pc_offset);
        break;
    case LDR_INSTR:
    case STR_INSTR:
        sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.mem.reg.reg, instr.mem.reg.base_r,
                instr.mem.reg.offset);
        break;
    case JSR_INSTR: // JSRR_INSTR
        if (instr.subr.jsr.is_jsr)
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][1], instr.subr.jsr.pc_offset);
        else
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.subr.jsrr.base_r);
        break;
    case JMP_INSTR: // RET_INSTR
        if (instr.jmp.base_r == 0x7)
            sprintf(buf, "%s", BASIC_DISASSEMBLE_LOOKUP[opcode][1]);
        else
            sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.jmp.base_r);
        break;
    case TRAP_INSTR:
        sprintf(buf, BASIC_DISASSEMBLE_LOOKUP[opcode][0], instr.trap.vector);
        break;
    case RTI_INSTR:
        sprintf(buf, "%s", BASIC_DISASSEMBLE_LOOKUP[opcode][0]);
        break;
    case ERROR_INSTR:
        if (state.instructionPlugin)
            return state.instructionPlugin->OnDisassemble(state, instr, LC3_BASIC_DISASSEMBLE);
        else
            sprintf(buf, "%s", BASIC_DISASSEMBLE_LOOKUP[opcode][0]);
        break;
        //default:
        //fprintf(stderr, "Unknown instruction %04x", data);
    }

    return buf;
}

std::string lc3_normal_disassemble(lc3_state& state, unsigned short data)
{
    lc3_instr instr = lc3_decode(state, data);
    unsigned int opcode = instr.data.opcode;
    char buf[128];
    char minibuf[7];
    int offset;
    std::string label;

    switch(opcode)
    {
    case BR_INSTR:
        offset = instr.br.pc_offset;
        label = lc3_sym_rev_lookup(state, state.pc + offset);
        // If all flags are off
        if (!(instr.br.n || instr.br.z || instr.br.p) || instr.br.pc_offset == 0)
        {
            if (!instr.br.n && !instr.br.z && !instr.br.p)
            {
                // Hey do I have a character?
                if (isprint(instr.br.pc_offset))
                {
                    snprintf(minibuf, 7, " ('%c')", instr.br.pc_offset);
                    // NOP (maybe with character)
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], minibuf);
                }
                else
                {
                    // NOP no character
                    sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], "");
                }
            }
            else
                // NOP2
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], "");
        }
        // If not all flags are on
        else if (!(instr.br.n && instr.br.z && instr.br.p))
        {
            if (label.empty())
            {
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.br.n ? "N" : "",
                        instr.br.z ? "Z" : "", instr.br.p ? "P" : "", instr.br.pc_offset);
            }
            else
            {
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], instr.br.n ? "N" : "",
                        instr.br.z ? "Z" : "", instr.br.p ? "P" : "", label.c_str());
            }
        }
        // All flags are on then
        else
        {
            if (label.empty())
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], "", "", "", instr.br.pc_offset);
            else
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], "", "", "", label.c_str());
        }
        break;
    case ADD_INSTR:
    case AND_INSTR:
        if (instr.arith.imm.is_imm)
        {
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1],
                    instr.arith.imm.dr, instr.arith.imm.sr1, instr.arith.imm.imm);
        }
        else
        {
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0],
                    instr.arith.reg.dr, instr.arith.reg.sr1, instr.arith.reg.sr2);
        }
        break;
    case NOT_INSTR:
        sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.arith.inv.dr, instr.arith.inv.sr1);
        break;
    case LD_INSTR:
    case ST_INSTR:
    case LEA_INSTR:
    case LDI_INSTR:
    case STI_INSTR:
        offset = instr.mem.offset.pc_offset;
        label = lc3_sym_rev_lookup(state, state.pc + offset);
        if (label.empty())
        {
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.mem.offset.reg,
                    instr.mem.offset.pc_offset);
        }
        else
        {
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], instr.mem.offset.reg, label.c_str());
        }
        break;
    case LDR_INSTR:
    case STR_INSTR:
        sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.mem.reg.reg, instr.mem.reg.base_r,
                instr.mem.reg.offset);
        break;
    case JSR_INSTR: // JSRR_INSTR
        if (instr.subr.jsr.is_jsr)
        {
            offset = instr.subr.jsr.pc_offset;
            label = lc3_sym_rev_lookup(state, state.pc + offset);

            if (label.empty())
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][1], instr.subr.jsr.pc_offset);
            else
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][2], label.c_str());
        }
        else
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.subr.jsrr.base_r);
        break;
    case JMP_INSTR: // RET_INSTR
        if (instr.jmp.base_r == 0x7)
            sprintf(buf, "%s", DISASSEMBLE_LOOKUP[opcode][1]);
        else
            sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.jmp.base_r);
        break;
    case TRAP_INSTR:
        data = instr.trap.vector;
        if (data >= TRAP_GETC && data <= TRAP_HALT)
            sprintf(buf, "%s", TRAP_CASES[data - TRAP_GETC]);
        else
        {
            // Plugin check time!
            if (state.trapPlugins.find(data) != state.trapPlugins.end())
                return state.trapPlugins[data]->GetTrapName();
            else
                sprintf(buf, DISASSEMBLE_LOOKUP[opcode][0], instr.trap.vector);
        }
        break;
    case RTI_INSTR:
        sprintf(buf, "%s", DISASSEMBLE_LOOKUP[opcode][0]);
        break;
    case ERROR_INSTR:
        if (state.instructionPlugin)
            return state.instructionPlugin->OnDisassemble(state, instr, LC3_NORMAL_DISASSEMBLE);
        else
            sprintf(buf, "%s", DISASSEMBLE_LOOKUP[opcode][0]);
        break;
        //default:
        //fprintf(stderr, "Unknown instruction %04x", data);
    }

    return buf;
}

std::string lc3_smart_disassemble(lc3_state& state, unsigned short instruction)
{
    lc3_instr instr = lc3_decode(state, instruction);
    int offset;
    unsigned int opcode = instr.data.opcode;
    int data;
    std::string label;
    char buf[128];
    char minibuf[7];

    switch(opcode)
    {
    case BR_INSTR:
        offset = instr.br.pc_offset;
        label = lc3_sym_rev_lookup(state, state.pc + offset);
        // If no flags are on or offset == 0 its a NOP
        if (!(instr.br.n || instr.br.z || instr.br.p) || offset == 0)
        {
            if (!(instr.br.n || instr.br.z || instr.br.p) && offset <= 256)
            {
                if (isprint(offset))
                {
                    snprintf(minibuf, 7, " ('%c')", offset);
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NOP], minibuf);
                }
                else
                    sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NOP], "");
            }
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NOP], "");

        }
        // If all flags are on
        else if (instr.br.n && instr.br.z && instr.br.p)
        {
            // If we know the label
            if (!label.empty())
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NZP_LABEL], label.c_str());
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NZP_OFF], SIGN_CHR(offset), ABS(offset));
        }
        // Cases BRN,BRZ,BRP,BRNZ,BRNP,BRZP
        else
        {
            // Data is the NZP flags
            data = instr.br.n | instr.br.z << 1 | instr.br.p << 2;
            if (!label.empty())
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NORM_LABEL], BR_CASES[data], label.c_str());
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][BR_NORM_OFF], BR_CASES[data],
                        SIGN_CHR(offset), ABS(offset));
        }
        break;
    case ADD_INSTR:
        if (instr.arith.imm.is_imm)
        {
            // Data is imm
            data = instr.arith.imm.imm;
            // TEST = ADD RX, RX, 0
            if (data == 0 && instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_TEST], instr.arith.imm.dr);
            // SET
            else if (data == 0 && instr.arith.imm.dr != instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_SET], instr.arith.imm.dr,
                        instr.arith.imm.sr1);
            // INC = ADD RX, RX, 1
            else if (data == 1 && instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_INC], instr.arith.imm.dr);
            // DEC = ADD RX, RX, -1
            else if (data == -1 && instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_DEC], instr.arith.imm.dr);
            // ADD_EQ = ADD RX, RX, NUM
            else if (instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_EQ_VAL], instr.arith.imm.dr,
                        SIGN_CHR(data), ABS(data));
            // NORMAL ADD IMM
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_REG_VAL], instr.arith.imm.dr,
                        instr.arith.imm.sr1, SIGN_CHR(data), ABS(data));
        }
        else
        {
            // DR is the data here
            data = instr.arith.reg.dr;

            // ADD EQ
            if (data == instr.arith.reg.sr1 || data == instr.arith.reg.sr2)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_EQ_REG], data,
                        OTHER(data, instr.arith.reg.sr1, instr.arith.reg.sr2));
            // NORMAL ADD
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][ADD_TWO_REGS], data,
                        instr.arith.reg.sr1, instr.arith.reg.sr2);
        }
        break;
    case AND_INSTR:
        if (instr.arith.imm.is_imm)
        {
            data = instr.arith.imm.imm;
            // ZERO OUT
            if (data == 0)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_ZERO], instr.arith.imm.dr);
            // TEST REG
            else if (data == -1 && instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_TEST], instr.arith.imm.dr);
            // SET REG
            else if (data == -1 && instr.arith.imm.dr != instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_SET], instr.arith.imm.dr,
                        instr.arith.imm.sr1);
            // AND EQUALS
            else if (instr.arith.imm.dr == instr.arith.imm.sr1)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_NUM], instr.arith.imm.dr, data);
            // NORMAL IMM ADD
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_REG_NUM], instr.arith.imm.dr,
                        instr.arith.imm.sr1, data);
        }
        else
        {
            // Data is DR
            data = instr.arith.reg.dr;

            if (data == instr.arith.reg.sr1 && data == instr.arith.reg.sr2)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_TEST], data);
            else if (instr.arith.reg.sr1 == instr.arith.reg.sr2)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_SET], data, instr.arith.reg.sr1);
            else if (data == instr.arith.reg.sr1 || data == instr.arith.reg.sr2)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_EQ_REG], data,
                        OTHER(data, instr.arith.reg.sr1, instr.arith.reg.sr2));
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][AND_TWO_REGS], data,
                        instr.arith.reg.sr1, instr.arith.reg.sr2);
        }
        break;
    case NOT_INSTR:
        // Easy :D
        sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][0], instr.arith.inv.dr, instr.arith.inv.sr1);
        break;
    case LD_INSTR:
    case LEA_INSTR:
    case LDI_INSTR:
        offset = instr.mem.offset.pc_offset;
        label = lc3_sym_rev_lookup(state, state.pc + offset);
        if (!label.empty())
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_LABEL], instr.mem.offset.reg, label.c_str());
        else if (offset != 0)
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET], instr.mem.offset.reg, SIGN_CHR(offset),
                    ABS(offset));
        else
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET_0], instr.mem.offset.reg);
        break;
    case ST_INSTR:
    case STI_INSTR:
        offset = instr.mem.offset.pc_offset;
        label = lc3_sym_rev_lookup(state, state.pc + offset);
        if (!label.empty())
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_LABEL], label.c_str(), instr.mem.offset.reg);
        else if (offset != 0)
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET], SIGN_CHR(offset), ABS(offset),
                    instr.mem.offset.reg);
        else
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][MEM_OFFSET_0], instr.mem.offset.reg);
        break;
    case LDR_INSTR:
        sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][0], instr.mem.reg.reg, instr.mem.reg.base_r,
                instr.mem.reg.offset);
        break;
    case STR_INSTR:
        sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][0], instr.mem.reg.base_r, instr.mem.reg.offset,
                instr.mem.reg.reg);
        break;
    case JSR_INSTR:
        if (instr.subr.jsr.is_jsr)
        {
            offset = instr.subr.jsr.pc_offset;
            label = lc3_sym_rev_lookup(state, state.pc + offset);

            if (!label.empty())
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JSR_LABEL], label.c_str());
            else if (offset != 0)
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JSR_OFFSET], SIGN_CHR(offset), ABS(offset));
            else
                sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][JSR_OFFSET_0]);
        }
        else
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JSR_JSRR], instr.subr.jsrr.base_r);
        break;
    case JMP_INSTR:
        if (instr.jmp.base_r == 0x7)
            sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][JMP_R7]);
        else
            sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][JMP_REG], instr.jmp.base_r);
        break;
    case TRAP_INSTR:
        data = instr.trap.vector;
        if (data >= TRAP_GETC && data <= TRAP_HALT)
            sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][data - TRAP_GETC]);
        else
        {
            // Plugin check time!
            if (state.trapPlugins.find(data) != state.trapPlugins.end())
                return state.trapPlugins[data]->GetTrapName();
            else
                sprintf(buf, ADV_DISASSEMBLE_LOOKUP[opcode][TRAP_OTHER], (unsigned char)data);
        }
        break;
    case RTI_INSTR:
        sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][0]);
        break;
    case ERROR_INSTR:
        if (state.instructionPlugin)
            return state.instructionPlugin->OnDisassemble(state, instr, LC3_ADVANCED_DISASSEMBLE);
        else
            sprintf(buf, "%s", ADV_DISASSEMBLE_LOOKUP[opcode][0]);
        break;
    }

    return buf;
}

std::string lc3_disassemble(lc3_state& state, unsigned short data, int level)
{
    std::string instr;
    switch(level)
    {
        case 0:
            instr = lc3_basic_disassemble(state, data);
            break;
        case 1:
            instr = lc3_normal_disassemble(state, data);
            break;
        case 2:
            instr = lc3_smart_disassemble(state, data);
            break;
        default:
            instr = "";
            break;
    }

    if (state.strict_execution)
    {
        switch((data >> 12) & 0xF)
        {
            case ADD_INSTR:
            case AND_INSTR:
                if ((data & 0x20) == 0 && (data & 0x18) != 0)
                    instr += " *";
                break;
            case BR_INSTR:
                if ((data & 0xE00) == 0)
                    instr += " *";
                break;
            case JMP_INSTR:
                if ((data & 0xE3F) != 0)
                    instr += " *";
                break;
            case JSRR_INSTR:
                if ((data & 0x800) == 0 && (data & 0x63F) != 0)
                    instr += " *";
                break;
            case NOT_INSTR:
                if ((data & 0x3F) != 0x3F)
                    instr += " *";
                break;
            case RTI_INSTR:
                if ((data & 0xFFF) != 0)
                    instr += " *";
                break;
            case TRAP_INSTR:
                if ((data & 0xF00) != 0)
                    instr += " *";
                break;
            default:
                break;
        }
    }

    return instr;
}

int lc3_load(lc3_state& state, std::istream& file, int (*reader)(std::istream&))
{
    if (!file.good()) return -1;

    int read_data = reader(file);

    // Until the reader runs out of data
    while (read_data >= 0)
    {
        // Get Address Start from Reader
        unsigned short addr_start = read_data & 0xFFFF;
        unsigned short addr_size = reader(file) & 0xFFFF;

        for (unsigned short i = 0; i < addr_size; i++)
            // Put it in memory
            state.mem[addr_start + i] = (short)(reader(file) & 0xFFFF);

        read_data = reader(file);
    }

    return 0;
}

int lc3_reader_hex(std::istream& file)
{
    std::string line;
    // Read one line
    if (file.eof()) return -1;
    getline(file, line);

    // Interpret as hex
    std::stringstream ss(line);
    unsigned int result;
    if(!(ss>>std::hex>>result)) return -1;

    return result;
}

int lc3_reader_obj(std::istream& file)
{
    unsigned short data;
    // Read two bytes
    file.read((char*) &data, sizeof(data));
    if (file.eof()) return -1;
    // Convert to LSB
    return ((data >> 8) & 0xFF) | ((data & 0xFF) << 8);
}

int lc3_read_char(lc3_state& state, std::istream& file)
{
    char c = file.get();
    if (!file.good())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0, 0);
        state.halted = true;
        return -1;
    }
    return c;
}

int lc3_peek_char(lc3_state& state, std::istream& file)
{
    char c = file.peek();
    if (!file.good())
    {
        lc3_warning(state, LC3_OUT_OF_INPUT, 0, 0);
        state.halted = true;
        return -1;
    }
    return c;
}

int lc3_write_char(lc3_state& state, std::ostream& file, int chr)
{
    if (chr > 255 || !(isgraph(chr) || isspace(chr)))
        lc3_warning(state, LC3_INVALID_CHARACTER_WRITE, chr, 0);
    return state.writer(state, file, chr);
}

int lc3_do_write_char(lc3_state&, std::ostream& file, int chr)
{
    if (!file.good()) return -1;
    file.put((char) chr);
    return (!file.good()) ? -1 : 0;
}

int lc3_write_str(lc3_state& state, int (*writer)(lc3_state& state, std::ostream& file, int), std::ostream& file, const std::string& str)
{
    for (unsigned int i = 0; i < str.size(); i++)
    {
        if (writer(state, file, str[i])) return -1;
    }
    return 0;
}

void lc3_randomize(lc3_state& state)
{
    // If true traps is set overwrite overwrite overwrite!
    if (state.true_traps)
    {
        // Add LC3 OS
        memcpy(state.mem, lc3_os, LC3_OS_SIZE * sizeof(unsigned short));
    }
    // Stage 1 don't clobber the TVT or IVT
    // Stage 2 system memory only overwrite 0's
    for (int i = 0x200; i < 0x3000; i++)
    {
        if (!state.mem[i])
            state.mem[i] = lc3_random(state);
    }

    // Stage 3 write over it all (even device registers).
    for (int i = 0x3000; i <= 0xFFFF; i++)
        state.mem[i] = lc3_random(state);
}

