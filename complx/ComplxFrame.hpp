#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP
#define EXPAND_AND_STRINGIFY(S) STRINGIFY_TOKEN(S)
#define STRINGIFY_TOKEN(S) #S

#include <wx/grid.h>
#include <wx/textctrl.h>
#include <wx/event.h>
#include <wx/window.h>
#include <wx/filename.h>

#include "ComplxFrameDecl.h"
#include "LC3Console.hpp"
#include "MemoryView.hpp"
#include "MemoryViewFrame.hpp"

enum
{
    BASE_2,
    BASE_10,
    BASE_16,
    NUM_BASES,
    BASE_CC,
};

class wxThreadEvent;

class ComplxFrame : public ComplxFrameDecl
{
    public:
        ComplxFrame(long disassemble, long stack_size, long call_stack_size, long true_traps, long interrupts, long highlight, wxString address_str,
                    wxArrayString files);
        ~ComplxFrame();

        // File menu event handlers
        void OnRandomizeAndLoad(wxCommandEvent& event);
        void OnRandomizeAndReload(wxCommandEvent& event);
        void OnLoad(wxCommandEvent& event);
        void OnReload(wxCommandEvent& event);
        void OnLoadOver(wxCommandEvent& event);
        void OnReloadOver(wxCommandEvent& event);
        void OnLoadMachine(wxCommandEvent& event);
        void OnSaveMachine(wxCommandEvent& event);
        void OnQuit(wxCommandEvent& event);

        // View menu event handlers
        void OnNewView(wxCommandEvent& event);
        void OnGoto(wxCommandEvent& event);
		    void OnUpdateHideAddresses(wxCommandEvent& event);
		    void OnHideAddressesCustom(wxCommandEvent& event);
        void OnDumbDisassemble(wxCommandEvent& event);
        void OnNormalDisassemble(wxCommandEvent& event);
        void OnCDisassemble(wxCommandEvent& event);
        void OnInstructionHighlight(wxCommandEvent& event);
        void OnFlipMemory(wxCommandEvent& event);
        // Helpers for view menu actions
        void OnDestroyView(wxCloseEvent& event);

        // State menu event handlers
        void OnStep(wxCommandEvent& event);
        void OnBackStep(wxCommandEvent& event);
        void OnNextLine(wxCommandEvent& event);
        void OnPrevLine(wxCommandEvent& event);
        void OnRun(wxCommandEvent& event);
        void OnRunFor(wxCommandEvent& event);
        void OnRunAgain(wxCommandEvent& event);
        void OnRewind(wxCommandEvent& event);
        void OnFinish(wxCommandEvent& event);
        void OnRandomize(wxCommandEvent& event);
        void OnReinitialize(wxCommandEvent& event);
        void OnFillMemoryWith(wxCommandEvent& event);
        void OnTrueTraps(wxCommandEvent& event);
        void OnInterrupts(wxCommandEvent& event);
        void OnClearConsole(wxCommandEvent& event);
        void OnClearConsoleInput(wxCommandEvent& event);

        // Helpers
        void OnTextKillFocus(wxFocusEvent& event);
        void OnBaseChange(wxMouseEvent& event);
        void OnBaseChangeContext(wxMouseEvent& event);
        void OnEditAddress(wxCommandEvent& event);
        void OnRegisterChanged(wxCommandEvent& text);

        // Debug menu event handlers
        void OnUndoStack(wxCommandEvent& event);
        void OnCallStack(wxCommandEvent& event);
        void OnSubroutineCall(wxCommandEvent& event);
        void OnBreakAndWatchpoints(wxCommandEvent& event);
        void OnTemppoint(wxCommandEvent& event);
        void OnBreakpoint(wxCommandEvent& event);
        void OnWatchpoint(wxCommandEvent& event);
        void OnAdvancedBreakpoint(wxCommandEvent& event);
        void OnBlackbox(wxCommandEvent& event);

        // Testing menu event handlers
        void OnRunTests(wxCommandEvent& event);
        void OnRerunTests(wxCommandEvent& event);
        bool TryLoadTests(lc3_test_suite& suite, const wxString& path);

        // Help menu event handlers
        void OnDocs(wxCommandEvent& event);
        void OnISA(wxCommandEvent& event);
        void OnChangeLog(wxCommandEvent& event);
        void OnCreateBugReport(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnFirstTime(wxCommandEvent& event);
        void OnTips(wxCommandEvent& event);

        // Misc event handlers required for THINGS.
        void OnActivate(wxActivateEvent& event);
        void OnIdle(wxIdleEvent& event);
        void OnRunUpdate(wxThreadEvent& event);
        void OnRunComplete(wxThreadEvent& event);
        void OnIo(wxThreadEvent& event);
        void OnOutput(wxThreadEvent& event);
        void OnNoIo(wxThreadEvent& event);

        // Other methods
        void OnGetIo();
        void UpdatePlay(void);
        void UpdatePhrase(const wxString& message);
        void UpdateRegisters(void);
        void UpdateMemory(void);
        void UpdateStatus(void);
        bool Running();

    private:
        LC3Console* console;
        std::vector<MemoryViewFrame*> views;
        MemoryView* memoryView;

        long stack_size;
        long call_stack_size;

        void UpdateRegister(wxTextCtrl* text, int value, int index);
        void OnInit(bool randomize_registers = true, bool randomize_memory = false, short fill_value = 0);

        void SetupExecution(int run_mode, int runtime = -1);
        void DoLoadFile(const wxFileName& filename);
};

#endif
