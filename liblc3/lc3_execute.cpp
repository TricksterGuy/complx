#include "lc3_execute.hpp"
#include "lc3_plugin.hpp"
#include "lc3_runner.hpp"
#include <algorithm>
#include <cstdlib>

const char* WARNING_MESSAGES[LC3_WARNINGS] =
{
    "Reading beyond end of input. Halting.",
    "Writing x%04x to reserved memory at x%04x.",
    "Reading from reserved memory at x%04x.",
    "Unsupported Trap x%02x. Assuming Halt.",
    "Unsupported Instruction x%04x. Halting.",
    "Malformed Instruction x%04x. Halting.",
    "RTI executed in user mode. Halting.",
    "Trying to write invalid character x%04x.",
    "PUTS called with invalid address x%04x.",
    "Trying to write to the display when its not ready.",
    "Trying to read from the keyboard when its not ready.",
    "Turning off machine via the MCR register.",
    "PUTSP called with invalid address x%04x",
    "PUTSP found an unexpected NUL byte at address x%04x.",
    "Invalid value x%04x loaded into the PSR."
};

lc3_instr lc3_decode(lc3_state& state, unsigned short data)
{
    lc3_instr instr;
    // Clear
    instr.bits = 0;
    // Gets the opcode bits
    instr.data.opcode = (data >> 12) & 0xF;

    switch(instr.data.opcode)
    {
    case BR_INSTR:
        instr.br.n = (data >> 11) & 0x1;
        instr.br.z = (data >> 10) & 0x1;
        instr.br.p = (data >> 9)  & 0x1;
        instr.br.pc_offset = data & 0x1FF;
        break;
    case ADD_INSTR:
    case AND_INSTR:
        instr.arith.imm.dr = (data >> 9) & 0x7;
        instr.arith.imm.sr1 = (data >> 6) & 0x7;
        instr.arith.imm.is_imm = (data >> 5) & 0x1;
        if (instr.arith.imm.is_imm)
            instr.arith.imm.imm = data & 0x1F;
        else
        {
            instr.arith.reg.sr2 = data & 0x7;
            instr.arith.reg.unused = (data >> 3) & 0x3;
        }
        break;
    case NOT_INSTR:
        instr.arith.inv.dr = (data >> 9) & 0x7;
        instr.arith.inv.sr1 = (data >> 6) & 0x7;
        instr.arith.inv.unused = data & 0x3F;
        break;
    case LD_INSTR:
    case ST_INSTR:
    case LEA_INSTR:
    case LDI_INSTR:
    case STI_INSTR:
        instr.mem.offset.reg = (data >> 9) & 0x7;
        instr.mem.offset.pc_offset = data & 0x1FF;
        break;
    case LDR_INSTR:
    case STR_INSTR:
        instr.mem.reg.reg = (data >> 9) & 0x7;
        instr.mem.reg.base_r = (data >> 6) & 0x7;
        instr.mem.reg.offset = data & 0x3F;
        break;
    case JSR_INSTR: // JSRR_INSTR
        if ((data >> 11) & 0x1)
        {
            instr.subr.jsr.is_jsr = 1;
            instr.subr.jsr.pc_offset = data & 0x7FF;
        }
        else
        {
            instr.subr.jsrr.is_jsr = 0;
            instr.subr.jsrr.unused_2 = (data >> 9) & 0x3;
            instr.subr.jsrr.base_r = (data >> 6) & 0x7;
            instr.subr.jsrr.unused_6 = data & 0x3F;
        }
        break;
    case JMP_INSTR: // RET_INSTR
        instr.jmp.unused_3 = (data >> 9) & 0x7;
        instr.jmp.base_r = (data >> 6) & 0x7;
        instr.jmp.unused_6 = data & 0x3F;
        break;
    case TRAP_INSTR:
        instr.trap.unused = (data >> 8) & 0xF;
        instr.trap.vector = data & 0xFF;
        break;
    case RTI_INSTR:
        instr.rti.data = data & 0xFFF;
        break;
    case ERROR_INSTR:
        if (state.instructionPlugin)
            state.instructionPlugin->OnDecode(state, data, instr);
        else
            instr.data.data = data & 0xFFF;
        break;
    }
    return instr;
}

const lc3_state_change lc3_execute(lc3_state& state, lc3_instr instruction)
{
    // Initialize Changes (We don't know everything yet)
    lc3_state_change changes;
    changes.pc = state.pc;
    changes.r7 = state.regs[0x7];
    changes.privilege = state.privilege;
    changes.n = state.n;
    changes.z = state.z;
    changes.p = state.p;
    changes.halted = state.halted;
    changes.changes = LC3_NO_CHANGE;
    changes.location = 0xFFFF;
    changes.value = 0xFFFF;
    changes.savedusp = state.savedusp;
    changes.savedssp = state.savedssp;
    changes.warnings = state.warnings;

    changes.subroutine.address = 0x0;
    changes.subroutine.r6 = 0x0;
    changes.subroutine.is_trap = false;

    switch(instruction.data.opcode)
    {
    case BR_INSTR:
        // Invalid instruction check
        if (!instruction.br.n && !instruction.br.z && !instruction.br.p && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else if ((instruction.br.n && state.n) || (instruction.br.z && state.z) || (instruction.br.p && state.p))
            state.pc = state.pc + instruction.br.pc_offset;
        break;
    case ADD_INSTR:
        // Invalid instruction check
        if (!instruction.arith.imm.is_imm && instruction.arith.reg.unused != 0 && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else
        {
            // Hey DR will change here save it
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.arith.imm.dr;
            changes.value = state.regs[changes.location];
            // Two modes immediate value and registers
            if (instruction.arith.imm.is_imm)
            {
                state.regs[changes.location] = state.regs[instruction.arith.imm.sr1] + instruction.arith.imm.imm;
            }
            else
            {
                state.regs[changes.location] = state.regs[instruction.arith.reg.sr1] +
                                               state.regs[instruction.arith.reg.sr2];
            }
            // Update NZP
            lc3_setcc(state, state.regs[changes.location]);
        }
        break;
    case LD_INSTR:
        // DR is going to change here.
        changes.changes = LC3_REGISTER_CHANGE;
        changes.location = instruction.mem.offset.reg;
        changes.value = state.regs[changes.location];

        state.regs[changes.location] = lc3_mem_read(state, state.pc + instruction.mem.offset.pc_offset);
        lc3_setcc(state, state.regs[changes.location]);
        break;
    case ST_INSTR:
        // MEM[PC + PCOFFSET]  is going to change here.
        changes.changes = LC3_MEMORY_CHANGE;
        changes.location = state.pc + instruction.mem.offset.pc_offset;
        changes.value = state.mem[changes.location];

        lc3_mem_write(state, changes.location, state.regs[instruction.mem.offset.reg]);
        break;
    case JSR_INSTR:
        // Invalid instruction check
        if (!instruction.subr.jsr.is_jsr && (instruction.subr.jsrr.unused_2 != 0 || instruction.subr.jsrr.unused_6 != 0) && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else
        {
            // R7's going to change but its already saved. (Done in initializer)
            // Save Return Address

            state.regs[0x7] = state.pc;
            // Perform the Jump
            if (instruction.subr.jsr.is_jsr)
                state.pc += instruction.subr.jsr.pc_offset;
            else if (instruction.subr.jsrr.base_r == 0x7)
                state.pc = changes.r7; // Special case you trash R7 by the above line...
            else
                state.pc = state.regs[instruction.subr.jsrr.base_r];

            // If not within an interrupt, then don't want to store subroutines within an interrupt.
            if (state.privilege)
            {
                changes.changes = LC3_SUBROUTINE_BEGIN;
                changes.subroutine.address = state.pc;
                changes.subroutine.r6 = state.regs[0x6];

                //printf("enter: %x %d\n", state.pc, state.call_stack.size());
                if (state.call_stack.empty() && state.in_lc3test)
                {
                    unsigned int num_params = 0;
                    lc3_subroutine_call_info call_info;
                    call_info.address = state.pc;
                    call_info.r6 = state.regs[0x6];
                    if (state.subroutines.find(state.pc) != state.subroutines.end())
                        num_params = state.subroutines[state.pc].num_params;
                    for (unsigned int i = 0; i < num_params; i++)
                        call_info.params.push_back(state.mem[call_info.r6 + i]);
           	        for (unsigned int i = 0; i < 8; i++)
                        call_info.regs[i] = state.regs[i];

                    state.first_level_calls.push_back(call_info);
                }
                if (state.max_call_stack_size != 0)
                {
                    state.call_stack.push_back(changes.subroutine);
                    if (state.max_call_stack_size < state.call_stack.size())
                        state.call_stack.pop_front();
                }
                //printf("enter: %s\n", subroutine.name.c_str());
            }
        }
        break;
    case AND_INSTR:
        // Invalid instruction check
        if (!instruction.arith.imm.is_imm && instruction.arith.reg.unused != 0 && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else
        {
            // Hey DR will change here save it
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.arith.imm.dr;
            changes.value = state.regs[changes.location];
            // Two modes immediate value and registers
            if (instruction.arith.imm.is_imm)
            {
                state.regs[changes.location] = state.regs[instruction.arith.imm.sr1] & instruction.arith.imm.imm;
            }
            else
            {
                state.regs[changes.location] = state.regs[instruction.arith.reg.sr1] &
                                               state.regs[instruction.arith.reg.sr2];
            }
            // Update NZP
            lc3_setcc(state, state.regs[changes.location]);
        }
        break;
    case LDR_INSTR:
        changes.changes = LC3_REGISTER_CHANGE;
        changes.location = instruction.mem.reg.reg;
        changes.value = state.regs[changes.location];

        state.regs[changes.location] = lc3_mem_read(state, state.regs[instruction.mem.reg.base_r] +
                                       instruction.mem.reg.offset);
        lc3_setcc(state, state.regs[changes.location]);
        break;
    case STR_INSTR:
        // MEM[BASE + OFFSET] is going to change
        changes.changes = LC3_MEMORY_CHANGE;
        changes.location = state.regs[instruction.mem.reg.base_r] + instruction.mem.reg.offset;
        changes.value = state.mem[changes.location];

        lc3_mem_write(state, changes.location, state.regs[instruction.mem.reg.reg]);
        break;
    case RTI_INSTR:
        // Invalid instruction check
        if (instruction.rti.data != 0 && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        // SECURITY!
        else if (state.privilege) // user mode
        {
            // OOOOH I'M TELLING
            if (state.interrupt_enabled)
            {
                // Cause an interrupt
                lc3_signal_interrupt(state, state.priority, 0x00);
            }
            else
            {
                // Warning
                lc3_warning(state, LC3_USER_RTI, 0, 0);
                state.halted = 1;
                state.pc--;
            }
        }
        else
        {
            const bool bits[8] = {0, 1, 1, 0, 1, 0, 0, 0};
            // Pop PC and psr
            state.pc = state.mem[state.regs[6]];
            unsigned short psr = state.mem[state.regs[6] + 1];
            // Invalid PSR check, if the unspecified bits are filled or trying
            // to set multiple nzp bits warn.
            if ((psr & 0x78F8) != 0 || bits[psr & 7] != 1)
                lc3_warning(state, LC3_INVALID_PSR_VALUE, psr, 0);

            state.regs[6] += 2;
            state.privilege = (psr >> 15) & 1;
            state.priority = (psr >> 8) & 7;
            state.n = (psr >> 2) & 1;
            state.z = (psr >> 1) & 1;
            state.p = psr & 1;

            if (state.privilege) {
                state.savedssp = state.regs[6];
                state.regs[6] = state.savedusp;
            }

            // Determine if this RTI is for an interrupt or a trap.
            // If not lc3 2019 revision this should always be true.
            bool in_interrupt = state.lc3_version == 0;
            if (!state.rti_stack.empty())
            {
                lc3_rti_stack_item item = state.rti_stack.back();
                state.rti_stack.pop_back();
                in_interrupt = item.is_interrupt;
            }

            if (in_interrupt)
            {
                if (!state.interrupt_vector_stack.empty())
                {
                    state.interrupt_vector = state.interrupt_vector_stack.back();
                    state.interrupt_vector_stack.pop_back();
                }
                else
                {
                    state.interrupt_vector = -1;
                }

                changes.changes = LC3_INTERRUPT_END; // second flag.
            }
            else
            {
                changes.changes = LC3_SUBROUTINE_END;
                if (!state.call_stack.empty())
                {
                    changes.subroutine = state.call_stack.back();
                    state.call_stack.pop_back();
                }
            }
        }
        break;
    case NOT_INSTR:
        // Invalid instruction check
        if (instruction.arith.inv.unused != 0x3F && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else
        {
            // Hey DR will change here save it
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = instruction.arith.imm.dr;
            changes.value = state.regs[changes.location];
            state.regs[changes.location] = ~state.regs[instruction.arith.inv.sr1];
            // Update NZP
            lc3_setcc(state, state.regs[changes.location]);
        }
        break;
    case LDI_INSTR:
        changes.changes = LC3_REGISTER_CHANGE;
        changes.location = instruction.mem.offset.reg;
        changes.value = state.regs[changes.location];

        state.regs[changes.location] = lc3_mem_read(state, lc3_mem_read(state, state.pc + instruction.mem.offset.pc_offset));
        lc3_setcc(state, state.regs[changes.location]);
        break;
    case STI_INSTR:
        // MEM[MEM[PC + PCOFFSET]]  is going to change here.
        changes.changes = LC3_MEMORY_CHANGE;
        changes.location = lc3_mem_read(state, state.pc + instruction.mem.offset.pc_offset);
        changes.value = state.mem[changes.location];

        lc3_mem_write(state, changes.location, state.regs[instruction.mem.offset.reg]);
        break;
    case JMP_INSTR:
        // Invalid instruction check
        if ((instruction.jmp.unused_3 != 0 || instruction.jmp.unused_6 != 0) && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else
        {
            // Jump
            state.pc = state.regs[instruction.jmp.base_r];
            // If not within an interrupt and is actually RET
            if (state.privilege && instruction.jmp.base_r == 0x7)
            {
                changes.changes = LC3_SUBROUTINE_END;
                if (!state.call_stack.empty())
                {
                    changes.subroutine = state.call_stack.back();
                    state.call_stack.pop_back();
                    //printf("exit: %s %d\n", subroutine.name.c_str(), state.call_stack.size());
                }
                //else
                //    lc3_warning(state, "RET encountered when call stack was empty.");
            }
        }
        break;
    case LEA_INSTR:
        changes.changes = LC3_REGISTER_CHANGE;
        changes.location = instruction.mem.offset.reg;
        changes.value = state.regs[changes.location];

        state.regs[changes.location] = state.pc + instruction.mem.offset.pc_offset;
        // In the 2019 revision of LC-3 LEA no longer sets condition codes.
        if (state.lc3_version == 0)
            lc3_setcc(state, state.regs[changes.location]);
        break;
    case TRAP_INSTR:
        // Invalid instruction check
        if (instruction.trap.unused != 0 && state.strict_execution)
        {
            state.halted = 1;
            state.pc--;
            lc3_warning(state, LC3_MALFORMED_INSTRUCTION, state.mem[state.pc], 0);
        }
        else
        {
            if (state.lc3_version == 0)
            {
                // R7's going to change save it But again its already saved.
                // Save Return Address
                state.regs[0x7] = state.pc;
            }

            // Return information is done via the stack in the lc3 revision.

            // Execute the trap
            lc3_trap(state, changes, instruction.trap);
        }
        break;
    case ERROR_INSTR:
        // Do we have a plugin handler?
        if (state.instructionPlugin)
        {
            // Success use it.
            state.instructionPlugin->OnExecute(state, instruction, changes);
        }
        else
        {
            if (state.interrupt_enabled)
            {
                // Cause an interrupt
                lc3_signal_interrupt(state, state.priority, 0x01);
            }
            else
            {
                // Warning
                lc3_warning(state, LC3_UNSUPPORTED_INSTRUCTION, instruction.data.opcode << 12 | instruction.data.data, 0);
                state.halted = 1;
                state.pc--;
            }
        }

        break;
    }

    // Post processing.  If it is a register change and the register is r7
    // then move it.  Though why people would do something like ADD R7, R0, #1 is
    // beyond me...
    if (changes.changes == LC3_REGISTER_CHANGE && changes.location == 0x7)
    {
        changes.changes = LC3_NO_CHANGE;
        changes.r7 = changes.value;
    }

    return changes;
}

void lc3_trap(lc3_state& state, lc3_state_change& changes, trap_instr trap)
{
    // If not within an interrupt.
    if (state.privilege)
    {
        if (state.call_stack.empty() && state.in_lc3test && trap.vector != TRAP_HALT)
        {
            lc3_trap_call_info call_info;
            call_info.vector = trap.vector;
            for (unsigned int i = 0; i < 8; i++)
                call_info.regs[i] = state.regs[i];
            state.first_level_traps.push_back(call_info);
        }
    }

    // Declarations
    unsigned short r0 = state.regs[0];
    bool kernel_mode = (state.pc >= 0x200 && state.pc < 0x3000) || (state.privilege == 0);

    // The only nice thing about the revision is that traps now set PSR[15] to 0 for us.
    // No need to check the PC's location.
    if (state.lc3_version != 0)
        kernel_mode = state.privilege == 0;

    // If we are doing true traps.
    if (state.true_traps)
    {
        changes.changes = LC3_SUBROUTINE_BEGIN;
        if (state.lc3_version > 0)
        {
            short psr = (short)((state.privilege << 15) | (state.priority << 8) | (state.n << 2) | (state.z << 1) | state.p);
            // If we are in user mode we must switch usp/ssp
            if (state.privilege)
            {
                state.savedusp = state.regs[6];
                state.regs[6] = state.savedssp;
            }

            state.privilege = 0;
            state.regs[6] -= 2;
            state.mem[(unsigned short)state.regs[6]] = state.pc;
            state.mem[(unsigned short)(state.regs[6] + 1)] = psr;
            state.rti_stack.push_back(lc3_rti_stack_item{false});
        }

        // PC = MEM[VECTOR]
        state.pc = state.mem[trap.vector];

        // If not within an interrupt
        /// TODO why do I have this check here?
        if (state.privilege || state.lc3_version != 0)
        {
            changes.changes = LC3_SUBROUTINE_BEGIN;
            changes.subroutine.address = state.pc;
            changes.subroutine.r6 = state.regs[0x6];
            changes.subroutine.is_trap = true;
            if (state.max_call_stack_size != 0)
            {
                state.call_stack.push_back(changes.subroutine);
                if (state.max_call_stack_size < state.call_stack.size())
                    state.call_stack.pop_front();
            }
        }
    }
    else
    {
        bool putsp_should_stop = false;
        switch(trap.vector)
        {
        case TRAP_GETC:
            // R0 changes so save it.
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = 0;
            changes.value = state.regs[0];
            state.regs[0] = state.reader(state, *state.input);
            break;
        case TRAP_OUT:
            lc3_write_char(state, *state.output, state.regs[0]);
            state.output->flush();
            break;
        case TRAP_PUTS:
            if ((r0 < 0x3000U || r0 >= 0xFE00U) && !kernel_mode)
            {
                lc3_warning(state, LC3_PUTS_INVALID_MEMORY, r0, 0);
            }
            else
            {
                while (state.mem[r0] != 0x0000)
                {
                    lc3_write_char(state,*state.output, state.mem[r0]);
                    r0++;
                }
                state.output->flush();
            }
            break;
        case TRAP_IN:
            // R0 changes so save it.
            changes.changes = LC3_REGISTER_CHANGE;
            changes.location = 0;
            changes.value = state.regs[0];
            lc3_write_str(state, state.writer, *state.output, "Input character: ");
            state.regs[0] = state.reader(state, *state.input);
            // Don't call lc3_write_char since it will spit out a warning on non printable character
            state.writer(state, *state.output, state.regs[0]);
            state.output->flush();
            break;
        case TRAP_PUTSP:
            // PUTSP is considered in the appendix to be incorrect.
            // Or at least every implementation I've seen writes it
            // so that it ends when a NUL is encountered and not a
            // memory address with 0x0000 in it. Meh...
            if ((r0 < 0x3000U || r0 >= 0xFE00U) && !kernel_mode)
            {
                lc3_warning(state, LC3_PUTSP_INVALID_MEMORY, r0, 0);
            }
            else
            {
                while (state.mem[r0] != 0x0000)
                {
                    if (putsp_should_stop)
                        lc3_warning(state, LC3_PUTSP_UNEXPECTED_NUL, r0, 0);
                    unsigned short chunk = state.mem[r0];
                    if ((chunk & 0xFF) != 0)
                        lc3_write_char(state, *state.output, chunk & 0xFF);
                    else
                        lc3_warning(state, LC3_PUTSP_UNEXPECTED_NUL, r0, 0);
                    if ((chunk & 0xFF00) != 0)
                        lc3_write_char(state, *state.output, (chunk >> 8) & 0xFF);
                    else
                        putsp_should_stop = true;
                    r0++;
                }
                state.output->flush();
            }

            break;
        case TRAP_HALT:
            state.halted = 1;
            state.pc--;
            // In case anyone writes a bad interrupt and halts in the middle.
            state.privilege = 1;
            state.priority = 0;
            break;
        default:
        {
            // Hey does a plugin handle this?
            if (state.trapPlugins.find(trap.vector) != state.trapPlugins.end())
            {
                state.trapPlugins[trap.vector]->OnExecute(state, changes);
            }
            else
            {
                lc3_warning(state, LC3_UNSUPPORTED_TRAP, trap.vector, 0);
                state.halted = 1;
                state.pc--;
                // In case anyone writes a bad interrupt and bad traps.
                state.privilege = 1;
                state.priority = 0;
            }
        }
        }
    }
}

void lc3_setcc(lc3_state& state, short value)
{
    state.n = value < 0;
    state.z = value == 0;
    state.p = value > 0;
}

short lc3_mem_read(lc3_state& state, unsigned short addr, bool privileged)
{
    state.memory_ops[addr].reads++;
    state.total_reads++;

    // You are executing a trap if you are between 0x200 and 0x3000.
    bool kernel_mode = (state.pc >= 0x200 && state.pc < 0x3000) || (state.privilege == 0) || privileged;

    if (addr < 0x3000U || addr >= 0xFE00U)
    {
        switch(addr)
        {
        case DEV_KBSR:
            if (rand() % 16 < 5)
            {
                int val = state.peek(state, *state.input);
                if (val != -1)
                {
                    state.mem[DEV_KBSR] |= 0x8000;
                    state.mem[DEV_KBDR] = val;
                }
            }
            break;
        case DEV_KBDR:
            if (state.mem[DEV_KBSR]) // Will work if interrupts are enabled immediately available!
            {
                state.mem[DEV_KBDR] = state.reader(state, *state.input); // In case of interrupt
                state.mem[DEV_KBSR] &= 0x4000;
            }
            else
            {
                lc3_warning(state, LC3_KEYBOARD_NOT_READY, 0, 0);
                state.mem[DEV_KBDR] = 0;
            }
            break;
        case DEV_DSR:
            if (rand() % 4 < 1)
            {
                state.mem[DEV_DSR] = (short)(1 << 15);
            }
            break;
        case DEV_DDR:
            if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_READ, addr, 0);
            break;
        case DEV_PSR:
            if (state.lc3_version > 0)
            {
                state.mem[DEV_PSR] = (short)((state.privilege << 15) | (state.priority << 8) | (state.n << 2) | (state.z << 1) | state.p);
            } else
            {
                if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                    return state.address_plugins[addr]->OnRead(state, addr);
                else if (!kernel_mode)
                    // Warn if reading from reserved memory if you aren't in kernel mode
                    lc3_warning(state, LC3_RESERVED_MEM_READ, addr, 0);
            }
            break;
        case DEV_MCR:
            state.mem[DEV_MCR] = (short)(1 << 15);
            break;
        default:
            // Hey does a plugin handle this address
            if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                return state.address_plugins[addr]->OnRead(state, addr);
            else if (!kernel_mode)
                // Warn if reading from reserved memory if you aren't in kernel mode
                lc3_warning(state, LC3_RESERVED_MEM_READ, addr, 0);
            break;
        }
    }

    // Intercept if plugin registered for address.
    if (state.address_plugins.find(addr) != state.address_plugins.end() && state.address_plugins[addr])
            return state.address_plugins[addr]->OnRead(state, addr);

    return state.mem[addr];
}

void lc3_mem_write(lc3_state& state, unsigned short addr, short value, bool privileged)
{
    state.memory_ops[addr].writes++;
    state.total_writes++;

    // You are executing a trap if you are between 0x200 and 0x3000.
    bool kernel_mode = (state.pc >= 0x200 && state.pc < 0x3000) || (state.privilege == 0) || privileged;

    if (addr < 0x3000U || addr >= 0xFE00U)
    {
        switch(addr)
        {
        case DEV_KBSR:
            // Bit 14 is writable
            value &= 0x4000;
            state.mem[DEV_KBSR] &= ~0x4000;
            state.mem[DEV_KBSR] |= value;
            break;
        case DEV_KBDR:
            if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, addr, 0);
            break;
        case DEV_DSR:
            if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, addr, 0);
            break;
        case DEV_DDR:
            if (state.mem[DEV_DSR])
            {
                state.mem[DEV_DSR] = 0;
                lc3_write_char(state, *state.output, value);
                state.output->flush();
            }
            else
            {
                lc3_warning(state, LC3_DISPLAY_NOT_READY, 0, 0);
            }
            break;
        case DEV_PSR:
            if (state.lc3_version > 0) {
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, addr, 0);
                /// TODO consider allowing writing to the PSR.
            } else {
                if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                    state.address_plugins[addr]->OnWrite(state, addr, value);
                else if (!kernel_mode)
                    lc3_warning(state, LC3_RESERVED_MEM_WRITE, value, addr);
            }
            break;
        case DEV_MCR:
            if (!(value & 0x8000))
            {
                if (!kernel_mode) lc3_warning(state, LC3_TURN_OFF_VIA_MCR, 0, 0);
                state.halted = 1;
                state.pc--;
            }
            break;
        default:
            // Hey does a plugin handle this address
            if (addr >= 0xFE00U && state.address_plugins.find(addr) != state.address_plugins.end())
                state.address_plugins[addr]->OnWrite(state, addr, value);
            else if (!kernel_mode)
                lc3_warning(state, LC3_RESERVED_MEM_WRITE, value, addr);
        }
    }

    // Intercept if plugin registered for address.
    if (state.address_plugins.find(addr) != state.address_plugins.end() && state.address_plugins[addr])
        return state.address_plugins[addr]->OnWrite(state, addr, value);

    state.mem[addr] = value;
}

void lc3_warning(lc3_state& state, unsigned int warn_id, short arg1, short arg2)
{
    if (state.warn_limits.find(warn_id) != state.warn_limits.end() && state.warn_limits[warn_id] <= state.warn_stats[warn_id])
    {
        state.warn_stats[warn_id] += 1;
        state.warnings++;
        return;
    }

    char warning[128];
    std::string msg;

    sprintf(warning, WARNING_MESSAGES[warn_id], (unsigned short) arg1, (unsigned short) arg2);
    msg = warning;

    lc3_warning(state, msg);
    state.warn_stats[warn_id] += 1;
    if (state.warn_limits.find(warn_id) != state.warn_limits.end() && state.warn_limits[warn_id] <= state.warn_stats[warn_id])
    {
        lc3_warning(state, "Limit for previous warning has been reached will no longer output messages of this type");
        return;
    }
}

void lc3_warning(lc3_state& state, const std::string& msg)
{
    char warning[256];
    std::string message;

    unsigned short addr = state.pc - 1;
    std::string instr = lc3_disassemble(state, state.mem[addr], 1);

    sprintf(warning, "Warning at x%04x (instruction - %s): %s", addr, instr.c_str(), msg.c_str());

    message = warning;
    (*state.warning) << message << std::endl;

    state.warnings++;
}
