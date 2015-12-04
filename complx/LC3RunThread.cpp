#include "LC3RunThread.hpp"
#include <lc3_all.hpp>

wxDEFINE_EVENT(wxEVT_COMMAND_RUNTHREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_RUNTHREAD_UPDATE, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_RUNTHREAD_IO, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_RUNTHREAD_NOIO, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_RUNTHREAD_OUTPUT, wxThreadEvent);

extern LC3RunThread* thread;
extern lc3_state state;


///TODO consider making this a singleton
LC3RunThread::~LC3RunThread()
{
    thread = NULL;
}

/** Entry
  *
  *
  */
void* LC3RunThread::Entry()
{
    ///TODO consider writting this without rewriting next_line/prev_line.
    int depth = 0;
    lc3_instr instr;
    bool interrupt_begin = false;

    state.halted = 0;

    switch(run_mode)
    {
        case RUNMODE_RUN:

           /* while(!state.halted)
            {
                static int i = 0;
                static int max = rand() % 1000 + 500;
                lc3_step(state);
                if (TestDestroy()) return NULL;
                i++;
                if (i % max == 0)
                {
                    wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_UPDATE));
                    max = rand() % 1000 + 500;
                    Sleep(rand() % 100 + 0);
                }
                else
                    Yield();
            }*/

            while(!state.halted)
            {
                lc3_step(state);
                if (TestDestroy()) return NULL;
                wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_UPDATE));
                Yield();
            }
            /*break;*/
            break;
        case RUNMODE_RUNFOR:
            if (runtime > 0)
                lc3_run(state, runtime);
            else
                lc3_rewind(state, -runtime);
            break;
        case RUNMODE_STEP:
            lc3_step(state);
            break;
        case RUNMODE_BACK:
            lc3_back(state);
            break;
        case RUNMODE_FINISH:
            depth = 1;
        case RUNMODE_NEXTLINE:
            // Subroutine depth
            do
            {
                // Get Next Instruction.
                instr = lc3_decode(state, state.mem[state.pc]);
                // So if we get a JSR/JSRR or if we get a TRAP and true traps are enabled
                if (instr.data.opcode == JSR_INSTR || (instr.data.opcode == TRAP_INSTR && state.true_traps))
                    depth++;

                // If we get a RET instruction JMP R7
                if (instr.data.opcode == JMP_INSTR && instr.jmp.base_r == 7)
                    depth--;

                // If we get an RTI instruction
                if (instr.data.opcode == RTI_INSTR)
                    depth--;

                // Execute
                lc3_step(state);

                // If we got interrupted
                if (state.interrupt_enabled && state.undo_stack.back().changes == LC3_INTERRUPT_BEGIN)
                    depth++;

                if (TestDestroy()) return NULL;
                wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_UPDATE));
                Yield();
            } while (depth != 0 && !state.halted);
            break;
        case RUNMODE_PREVLINE:
            // Subroutine depth
            depth = 0;

            do
            {
                if (!state.undo_stack.empty())
                {
                    lc3_state_change& last = state.undo_stack.back();
                    // Can't backstep through interrupt
                    if (last.changes == LC3_INTERRUPT_BEGIN)
                        break;

                    // Get rid of all processed interrupts.
                    while (last.changes == LC3_INTERRUPT && !state.undo_stack.empty())
                    {
                        lc3_back(state);
                        last = state.undo_stack.back();
                        if (TestDestroy()) return NULL;
                        wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_UPDATE));
                        Yield();
                    }
                }

                // Execute (Have to do this first you can't assume mem[pc - 1] was the last
                // instruction due to jumps.
                lc3_back(state);
                // Get the instruction that got you where you are.
                lc3_instr instr = lc3_decode(state, state.mem[state.pc]);
                // If we get a RET instruction JMP R7
                if (instr.data.opcode == JMP_INSTR && instr.jmp.base_r == 7)
                    depth++;
                // So if we get a JSR/JSRR or if we get a TRAP and true traps are enabled
                if (instr.data.opcode == JSR_INSTR || (instr.data.opcode == TRAP_INSTR && state.true_traps))
                    depth--;
                if (TestDestroy()) return NULL;
                wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_UPDATE));
                Yield();
                // Don't have to handle interrupts here...
            } while (depth != 0 && !state.halted && !state.undo_stack.empty());
            break;
        case RUNMODE_REWIND:
            // Do this until no more changes.
            while (!state.undo_stack.empty() && !interrupt_begin)
            {
                lc3_state_change& last = state.undo_stack.back();
                interrupt_begin = (last.changes == LC3_INTERRUPT_BEGIN);
                // Backstep
                lc3_back(state);

                if (TestDestroy()) return NULL;
                wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_UPDATE));
                Yield();
            }
            break;
    }

    wxQueueEvent(frame, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_COMPLETED));

    /*frame->UpdateRegisters();
    frame->UpdateMemory();
    frame->UpdateStatus();*/

    return NULL;
}
