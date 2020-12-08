#include "Lc3BinaryDisplayData.hpp"

#include <bitset>
#include <sstream>

namespace
{

typedef std::list<RLEColorEntry> InstructionColoring;

const RLEColorEntry opcodeColoring = {"OPCODE_COLOR", 0, 96, 96, 4};

InstructionColoring brColoring   = {{"BRN_COLOR", 160, 64, 64, 1},     {"BRZ_COLOR", 64, 64, 160, 1},      {"BRP_COLOR", 64, 160, 64, 1},      {"PCOFFSET_COLOR", 48, 48, 72, 9}   };
InstructionColoring jsrColoring  = {{"UNUSED_BITS_COLOR", 0, 0, 0, 1}, {"PCOFFSET_COLOR", 48, 48, 72, 11}                                                                          };
InstructionColoring jmpColoring  = {{"UNUSED_BITS_COLOR", 0, 0, 0, 3}, {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 6}                                       };
InstructionColoring trapColoring = {{"UNUSED_BITS_COLOR", 0, 0, 0, 4}, {"VECTOR_COLOR", 48, 48, 72, 8}                                                                             };
InstructionColoring ariColoring  = {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 1},  {"IMM_COLOR", 0, 0, 80, 5}          };
InstructionColoring arrColoring  = {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 3},  {"SR_COLOR", 0, 0, 80, 3}           };
InstructionColoring notColoring  = {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"UNUSED_BITS_COLOR", 0, 0, 0, 6}                                       };
InstructionColoring pcoColoring  = {{"DR_COLOR", 96, 0, 0, 3},         {"PCOFFSET_COLOR", 48, 48, 72, 9}                                                                           };
InstructionColoring boColoring   = {{"DR_COLOR", 96, 0, 0, 3},         {"SR_COLOR", 0, 0, 80, 3},          {"OFFSET_COLOR", 48, 48, 72, 6}                                         };
InstructionColoring defColoring  = {{"UNUSED_BITS_COLOR", 0, 0, 0, 12}                                                                                                             };
InstructionColoring dataColoring = {{"UNUSED_BITS_COLOR", 0, 0, 0, 16}};

wxString FormBinary(uint16_t value)
{
    std::stringstream binary;
    std::bitset<16> b = value;
    binary << b;
    return binary.str();
}

std::list<RLEColorEntry> ConstructRLEColorEntries(uint16_t instruction,  const InstructionPlugin* instruction_plugin)
{
    // Data. Two cases.
    //  1) If BR instruction and NZP bits are 0.
    //  2) If the PCOffset bits are 0.
    if (instruction >> 9 == 0 || (instruction & 0xF1FF) == 0)
        return dataColoring;

    std::list<RLEColorEntry> colors;

    uint16_t opcode = instruction >> 12;
    switch (opcode)
    {
        case BR_INSTR:
            colors = brColoring;
            break;
        case ADD_INSTR:
        case AND_INSTR:
            colors = ((instruction >> 5) & 0x1) ? ariColoring : arrColoring;
            break;
        case NOT_INSTR:
            colors = notColoring;
            break;
        case LD_INSTR:
        case ST_INSTR:
        case LDI_INSTR:
        case STI_INSTR:
        case LEA_INSTR:
            colors = pcoColoring;
            break;
        case JSR_INSTR:
            colors = ((instruction >> 11) & 0x1) ? jsrColoring : jmpColoring;
            break;
        case LDR_INSTR:
        case STR_INSTR:
            colors = boColoring;
            break;
        case JMP_INSTR:
            colors = jmpColoring;
            break;
        case TRAP_INSTR:
            colors = trapColoring;
            break;
        case ERROR_INSTR:
            if (instruction_plugin)
            {
                colors = instruction_plugin->GetInstructionColoring(instruction);
            }
            else
                colors = defColoring;
            break;
        default:
            colors = defColoring;
            break;
    }

    if (colors.empty())
        colors = defColoring;

    colors.push_front(opcodeColoring);

    return colors;
}

}

IMPLEMENT_DYNAMIC_CLASS(Lc3BinaryDisplayData, wxObject)
IMPLEMENT_VARIANT_OBJECT(Lc3BinaryDisplayData)

bool Lc3BinaryDisplayData::operator==(const Lc3BinaryDisplayData& other) const
{
    return instruction == other.instruction && binary == other.binary && colors == other.colors;
}

Lc3BinaryDisplayData ConstructBinaryDisplayData(uint16_t instruction, const InstructionPlugin* instruction_plugin)
{
    wxString binary = FormBinary(instruction);
    std::list<RLEColorEntry> colors = ConstructRLEColorEntries(instruction, instruction_plugin);

    return Lc3BinaryDisplayData(instruction, binary, colors);
}
