#include "windows.hpp"
#include <bitset>

extern lc3_state state;

std::string binary_instruction_string(unsigned short data)
{
    std::bitset<16> b = data;
    std::stringstream stringstr;
    stringstr << b;
    return stringstr.str();
}

// Color Configuration
#define COLORID_OPCODE 9
#define COLORID_BRN 10
#define COLORID_BRZ 11
#define COLORID_BRP 12
#define COLORID_OFFSET 13
#define COLORID_DRREG 14
#define COLORID_SRREG 15

#define DEFCOLORID_OPCODE COLOR_CYAN
#define DEFCOLORID_BRN COLOR_RED
#define DEFCOLORID_BRZ COLOR_BLUE
#define DEFCOLORID_BRP COLOR_GREEN
#define DEFCOLORID_OFFSET COLOR_MAGENTA
#define DEFCOLORID_DRREG COLOR_RED
#define DEFCOLORID_SRREG COLOR_BLUE

#define COLOR_OPCODE   0,375,375
#define COLOR_BRN    625,250,250
#define COLOR_BRZ    250,250,625
#define COLOR_BRP    250,625,250
#define COLOR_OFFSET 187,187,281
#define COLOR_DRREG  375,  0,0
#define COLOR_SRREG  0,    0,300

enum ColorID
{
    UNUSEDBIT = -1,
    OPCODE = 0,
    BRN = 1,
    BRZ = 2,
    BRP = 3,
    OFFSET = 4,
    DRREG = 5,
    SRREG = 6,
    NUM_COLORID = 7,
};

std::vector<int> color_id_table(NUM_COLORID, 0);

struct RLEColorEntryTerminal
{
    int pair_id;
    int length;
};

enum Colorings
{
    BR_TYPE,
    JSR_TYPE,
    JUMP_REG_TYPE,
    TRAP_TYPE,
    ARITH_IMM_TYPE,
    ARITH_REG_TYPE,
    NOT_TYPE,
    PCOFFSET_TYPE,
    BASE_OFFSET_TYPE,
    NULL_TYPE,
    COLORINGS_SIZE
};

RLEColorEntryTerminal instrColorings[COLORINGS_SIZE][4] =
{
    {{BRN, 1}, {BRZ, 1}, {BRP, 1}, {OFFSET, 9}},
    {{UNUSEDBIT, 1}, {OFFSET, 11}},
    {{UNUSEDBIT, 3}, {SRREG, 3}, {UNUSEDBIT, 6}},
    {{UNUSEDBIT, 4}, {OFFSET, 8}},
    {{DRREG, 3}, {SRREG, 3}, {UNUSEDBIT, 1}, {SRREG, 5}},
    {{DRREG, 3}, {SRREG, 3}, {UNUSEDBIT, 3}, {SRREG, 3}},
    {{DRREG, 3}, {SRREG, 3}, {UNUSEDBIT, 6}},
    {{DRREG, 3}, {OFFSET, 9}},
    {{DRREG, 3}, {SRREG, 3}, {OFFSET, 6}},
    {{UNUSEDBIT, 12}},
};

static int try_init_color(int id, int r, int g, int b, int defid)
{
    return (init_color(id, r, g, b) == ERR) ? defid : id;
}

void MemoryWindow::InitColors()
{
    if (!has_colors() || COLORS < 8) return;

    if (!can_change_color())
    {
        color_id_table[OPCODE] = DEFCOLORID_OPCODE;
        color_id_table[BRN] = DEFCOLORID_BRN;
        color_id_table[BRZ] = DEFCOLORID_BRZ;
        color_id_table[BRP] = DEFCOLORID_BRP;
        color_id_table[OFFSET] = DEFCOLORID_OFFSET;
        color_id_table[DRREG] = DEFCOLORID_DRREG;
        color_id_table[SRREG] = DEFCOLORID_SRREG;
        return;
    }

    if (COLORS == 8)
    {
        color_id_table[OPCODE] = try_init_color(OPCODE + 1, COLOR_OPCODE, DEFCOLORID_OPCODE);
        color_id_table[BRN] = try_init_color(BRN + 1, COLOR_BRN, DEFCOLORID_BRN);
        color_id_table[BRZ] = try_init_color(BRZ + 1, COLOR_BRZ, DEFCOLORID_BRZ);
        color_id_table[BRP] = try_init_color(BRP + 1, COLOR_BRP, DEFCOLORID_BRP);
        color_id_table[OFFSET] = try_init_color(OFFSET + 1, COLOR_OFFSET, DEFCOLORID_OFFSET);
        color_id_table[DRREG] = try_init_color(DRREG + 1, COLOR_DRREG, DEFCOLORID_DRREG);
        color_id_table[SRREG] = try_init_color(SRREG + 1, COLOR_SRREG, DEFCOLORID_SRREG);
    }
    else
    {
        color_id_table[OPCODE] = try_init_color(COLORID_OPCODE, COLOR_OPCODE, DEFCOLORID_OPCODE);
        color_id_table[BRN] = try_init_color(COLORID_BRN, COLOR_BRN, DEFCOLORID_BRN);
        color_id_table[BRZ] = try_init_color(COLORID_BRZ, COLOR_BRZ, DEFCOLORID_BRZ);
        color_id_table[BRP] = try_init_color(COLORID_BRP, COLOR_BRP, DEFCOLORID_BRP);
        color_id_table[OFFSET] = try_init_color(COLORID_OFFSET, COLOR_OFFSET, DEFCOLORID_OFFSET);
        color_id_table[DRREG] = try_init_color(COLORID_DRREG, COLOR_DRREG, DEFCOLORID_DRREG);
        color_id_table[SRREG] = try_init_color(COLORID_SRREG, COLOR_SRREG, DEFCOLORID_SRREG);
    }

    for (int i = 0; i < NUM_COLORID; i++)
        init_pair(color_id_table[i], COLOR_WHITE, color_id_table[i]);
}

void MemoryWindow::Display(unsigned short start, unsigned short end, int level)
{
    window->clear();
    for (unsigned short s = 0; s < height - 2; s++)
    {
        WriteRow(s + 1, s + start, level);
    }
    window->box();
    window->refresh();
}

void MemoryWindow::Update(void)
{
    Display(state.pc, state.pc, 1);
    /*window->clear();
    for (int i = 0; i < height - 2; i++)
    {
        WriteRow(i + 1, state.pc + i);
    }
    window->box();
    window->refresh();*/
}

void MemoryWindow::WriteRow(int row, unsigned short addr, int level)
{
    unsigned short data = state.mem[addr];

    std::string output;
    switch(level)
    {
        case LC3_BASIC_DISASSEMBLE:
            output = lc3_basic_disassemble(state, data);
            break;
        case LC3_NORMAL_DISASSEMBLE:
            output = lc3_disassemble(state, data);
            break;
        case LC3_ADVANCED_DISASSEMBLE:
            output = lc3_smart_disassemble(state, data);
            break;
        default:
            output = lc3_disassemble(state, data);
            break;
    }

    const auto& comment = state.comments.find(addr);

    if (state.breakpoints.find(addr) != state.breakpoints.end())
    {
        window->add_attr(COLOR_PAIR(COLOR_RED));
        window->write(1, row, "!");
        window->remove_attr(COLOR_PAIR(COLOR_RED));
    }

    if (addr == state.pc) window->add_attr(A_BOLD);
    window->write(2, row, "x%04hx", addr);
    if (addr == state.pc) window->remove_attr(A_BOLD);

    window->write(" : ");

    WriteInstruction(data);

    window->advance(1, 0);
    window->write("%s", output.c_str());
    if (comment != state.comments.end())
    {
        window->advance(1, 0);
        window->write(comment->second.c_str());
    }
}

void MemoryWindow::WriteInstruction(unsigned short instr)
{
    int opcode = (instr >> 12) & 0xF;
    std::vector<RLEColorEntryTerminal> colors;

    switch (opcode)
    {
        case BR_INSTR:
            colors.assign(instrColorings[BR_TYPE], instrColorings[BR_TYPE] + 4);
            break;
        case ADD_INSTR:
        case AND_INSTR:
            if (instr >> 5 & 0x1)
                colors.assign(instrColorings[ARITH_IMM_TYPE], instrColorings[ARITH_IMM_TYPE] + 4);
            else
                colors.assign(instrColorings[ARITH_REG_TYPE], instrColorings[ARITH_REG_TYPE] + 4);
            break;
        case NOT_INSTR:
            colors.assign(instrColorings[NOT_TYPE], instrColorings[NOT_TYPE] + 4);
            break;
        case LD_INSTR:
        case ST_INSTR:
        case LDI_INSTR:
        case STI_INSTR:
        case LEA_INSTR:
            colors.assign(instrColorings[PCOFFSET_TYPE], instrColorings[PCOFFSET_TYPE] + 4);
            break;
        case JSR_INSTR:
            if ((instr >> 11) & 1)
                colors.assign(instrColorings[JSR_TYPE], instrColorings[JSR_TYPE] + 4);
            else
                colors.assign(instrColorings[JUMP_REG_TYPE], instrColorings[JUMP_REG_TYPE] + 4);
            break;
        case LDR_INSTR:
        case STR_INSTR:
            colors.assign(instrColorings[BASE_OFFSET_TYPE], instrColorings[BASE_OFFSET_TYPE] + 4);
            break;
        case JMP_INSTR:
            colors.assign(instrColorings[JUMP_REG_TYPE], instrColorings[JUMP_REG_TYPE] + 4);
            break;
        case TRAP_INSTR:
            colors.assign(instrColorings[TRAP_TYPE], instrColorings[TRAP_TYPE] + 4);
            break;
        case ERROR_INSTR:
            //if (state.instructionPlugin)
            //    colors = state.instructionPlugin->GetInstructionColoring(instr);
            //else
                colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);
            break;
        default:
            colors.assign(instrColorings[NULL_TYPE], instrColorings[NULL_TYPE] + 4);
            break;
    }

    std::string binstr = binary_instruction_string(instr);

    // no-op
    if (instr >> 9 == 0 || (instr & (~0xE00)) == 0)
    {
        window->write("%s", binstr.c_str());
        return;
    }

    window->add_attr(COLOR_PAIR(color_id_table[OPCODE]));
    for (int i = 0; i < 4; i++)
        window->write(binstr[i]);
    window->remove_attr(COLOR_PAIR(color_id_table[OPCODE]));


    int colored = 4;
    int colorptr = 0;
    while (colored < 16)
    {
        RLEColorEntryTerminal& entry = colors[colorptr];
        if (entry.pair_id != -1) window->add_attr(COLOR_PAIR(color_id_table[entry.pair_id]));
        for (int i = 0; i < entry.length; i++)
            window->write(binstr[colored + i]);
        if (entry.pair_id != -1) window->remove_attr(COLOR_PAIR(color_id_table[entry.pair_id]));
        colored += entry.length;
        colorptr++;
    }
}

void RegisterWindow::Update(void)
{
    window->clear();
    window->move(1, 1);
    for (int i = 0; i < 8; i++)
    {
        short value = state.regs[i];
        window->write("R%d %6hd x%04hx", i, value, value);
        window->advance(2, 0);
        if (i == 3) window->move(1, 2);
    }
    window->write(1, 3, "PC x%04hx", state.pc);
    window->write(1, 4, "CC %c", state.n ? 'n' : (state.z ? 'z' : 'p'));
    window->box();
    window->refresh();
}
