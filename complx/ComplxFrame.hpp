#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP
#define EXPAND_AND_STRINGIFY(S) STRINGIFY_TOKEN(S)
#define STRINGIFY_TOKEN(S) #S

#include <lc3_all.hpp>
#include <memory>
#include <vector>

#include <wx/event.h>
#include <wx/filename.h>
#include <wx/fswatcher.h>
#include <wx/grid.h>
#include <wx/textctrl.h>
#include <wx/window.h>

#include "ComplxFrameDecl.h"
#include "LC3Console.hpp"
#include "MemoryView.hpp"
#include "MemoryViewFrame.hpp"
#include "LoadingOptions.hpp"

enum
{
    BASE_2,
    BASE_10,
    BASE_16,
    BASE_CHAR,
    NUM_BASES,
    BASE_CC,
};

class wxThreadEvent;

class ComplxFrame : public ComplxFrameDecl
{
public:
    struct Options
    {
        wxString title;
        LoadingOptions loading_options;
        int disassemble;
        unsigned int stack_size;
        unsigned int call_stack_size;
        bool highlight;
        int width;
        int height;
        std::vector<int> column_sizes;
        bool exact_column_sizing;
    };
    ComplxFrame(const Options& opts);
    ~ComplxFrame();

    // File menu event handlers
    void OnLoad(wxCommandEvent& event) override;
    void OnCleanLoad(wxCommandEvent& event) override;
    void OnAdvancedLoad(wxCommandEvent& event) override;
    void OnReload(wxCommandEvent& event) override;
    void OnQuit(wxCommandEvent& event) override;

    // View menu event handlers
    void OnNewView(wxCommandEvent& event) override;
    void OnGoto(wxCommandEvent& event) override;
    void OnUpdateHideAddresses(wxCommandEvent& event) override;
    void OnHideAddressesCustom(wxCommandEvent& event) override;
    void OnDumbDisassemble(wxCommandEvent& event) override;
    void OnNormalDisassemble(wxCommandEvent& event) override;
    void OnCDisassemble(wxCommandEvent& event) override;
    void OnInstructionHighlight(wxCommandEvent& event) override;
    void OnFlipMemory(wxCommandEvent& event) override;
    // Helpers for view menu actions
    void OnDestroyView(wxCloseEvent& event);

    // State menu event handlers
    void OnStep(wxCommandEvent& event) override;
    void OnBackStep(wxCommandEvent& event) override;
    void OnNextLine(wxCommandEvent& event) override;
    void OnPrevLine(wxCommandEvent& event) override;
    void OnRun(wxCommandEvent& event) override;
    void OnRunFor(wxCommandEvent& event) override;
    void OnRunAgain(wxCommandEvent& event) override;
    void OnRewind(wxCommandEvent& event) override;
    void OnFinish(wxCommandEvent& event) override;
    void OnControlModeSimple(wxCommandEvent& event) override;
	void OnControlModeAdvanced(wxCommandEvent& event) override;
    void OnTrueTraps(wxCommandEvent& event) override;
    void OnInterrupts(wxCommandEvent& event) override;
    void OnStrictExecution(wxCommandEvent& event) override;
    void OnClearConsole(wxCommandEvent& event) override;
    void OnClearConsoleInput(wxCommandEvent& event) override;

    // Helpers
    void OnTextKillFocus(wxFocusEvent& event);
    void OnBaseChange(wxMouseEvent& event);
    void OnBaseChangeContext(wxMouseEvent& event);
    void OnEditAddress(wxCommandEvent& event);
    void OnRegisterChanged(wxCommandEvent& text);

    // Debug menu event handlers
    void OnUndoStack(wxCommandEvent& event) override;
    void OnCallStack(wxCommandEvent& event) override;
    void OnSetupTest(wxCommandEvent& event) override;
    void OnSwitchTest(wxCommandEvent& event) override;
    void OnSubroutineCall(wxCommandEvent& event) override;
    void OnBreakAndWatchpoints(wxCommandEvent& event) override;
    void OnTemppoint(wxCommandEvent& event) override;
    void OnBreakpoint(wxCommandEvent& event) override;
    void OnWatchpoint(wxCommandEvent& event) override;
    void OnAdvancedBreakpoint(wxCommandEvent& event) override;
    void OnBlackbox(wxCommandEvent& event) override;

    // Help menu event handlers
    void OnDocs(wxCommandEvent& event) override;
    void OnISA(wxCommandEvent& event) override;
    void OnChangeLog(wxCommandEvent& event) override;
    void OnCreateBugReport(wxCommandEvent& event) override;
    void OnAbout(wxCommandEvent& event) override;
    void OnFirstTime(wxCommandEvent& event) override;
    void OnTips(wxCommandEvent& event) override;

    // Misc event handlers required for THINGS.
    void OnActivate(wxActivateEvent& event);
    void OnIdle(wxIdleEvent& event);
    void OnRunUpdate(wxThreadEvent& event);
    void OnRunComplete(wxThreadEvent& event);
    void OnIo(wxThreadEvent& event);
    void OnOutput(wxThreadEvent& event);
    void OnNoIo(wxThreadEvent& event);
    void OnFileTestsChanged(wxFileSystemWatcherEvent& event);

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
    LoadingOptions reload_options;
    std::vector<MemoryViewFrame*> views;
    MemoryView* memoryView;

    long stack_size;
    long call_stack_size;
    wxString base_title;

    std::unique_ptr<wxFileSystemWatcher> fswatcher;

    void UpdateRegister(wxTextCtrl* text, int value, int index);

    void SetupExecution(int run_mode, int runtime = -1);
    void DoLoadFile(const LoadingOptions& opts, bool initializing = false);
    void PostInit();
    void UpdateWatchedFiles(const std::string& file, const std::string& tests, bool remove_old = true);
    /** DetectSubroutine
      *
      * Attempts to detect if a subroutine is found in the loaded code
      * The way this function works is it will try to find a RET, JSR, or JSRR instruction.
      * We do not look at trap instructions since they are black boxed anyway if true traps are disabled,
      * however if a custom trap is written then it will of course have a RETY statement.
      * The usage of this function is for enabling or disabling the Next/Prev Line and Finish control buttons.
      * @param ranges Code ranges the file that was loaded touches
      * @return True if a subroutine was detected false otherwise
      */
    bool DetectSubroutine(const std::vector<code_range>& ranges);
};

#endif
