#include <wx/arrstr.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/utils.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>
#include <wx/textdlg.h>
#include <wx/imaglist.h>
#include <wx/dcmemory.h>
#include <wx/bitmap.h>
#include <wx/gdicmn.h>
#include <wx/brush.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>
#include <wx/config.h>
#include <wx/tipdlg.h>
#include <cassert>
#include <bitset>
#include <algorithm>
#include <iostream>
#include "icon64.xpm"

#include "ComplxFrame.hpp"
#include "MemoryGrid.hpp"
#include "MemoryViewContainer.hpp"
#include "AdvancedLoadDialog.hpp"
#include "CallStackDialog.hpp"
#include "CallSubroutineDialog.hpp"
#include "DebugInfoDialog.hpp"
#include "WatchpointDialog.hpp"
#include "AddressDialog.hpp"
#include "RunForDialog.hpp"
#include "LC3RunThread.hpp"
#include "version.h"

#ifdef ENABLE_LC3_REPLAY
#include <lc3_replay.hpp>
#endif

lc3_state state;

extern ComplxFrame* complxframe;
int runforRunTime = 0;
wxCriticalSection threadCS;
LC3RunThread* thread = NULL;
bool refreshednoio = false;

int complx_reader(lc3_state& state, std::istream& file);
int complx_peek(lc3_state& state, std::istream& file);
int complx_writer(lc3_state& state, std::ostream& file, int chr);
void complx_step(lc3_state& state);

// R0-R7, PC, CC
int register_display[10] = {BASE_10, BASE_10, BASE_10, BASE_10, BASE_10, BASE_16, BASE_16, BASE_16, BASE_16, BASE_CC};

// Modified addresses provided by assembler
std::vector<ViewRange> modified_addresses;

wxImageList* infoImages = new wxImageList(22, 16, true);
void InitImages(void);
void DestroyImages(void);
void PrintError(int error);

/** ComplxFrame
  *
  * Constructor
  */
ComplxFrame::ComplxFrame(const ComplxFrame::Options& opts) :
    ComplxFrameDecl(NULL, wxID_ANY, opts.title, wxDefaultPosition, wxSize(opts.width, opts.height)), console(NULL), memoryView(NULL), base_title(opts.title)
{
    InitImages();

    menuViewInstructionHighlighting->Check(opts.highlight);
    menuViewBasic->Check(opts.disassemble == 0);
    menuViewNormal->Check(opts.disassemble == 1);
    menuViewHighLevel->Check(opts.disassemble == 2);

    this->stack_size = opts.stack_size;
    this->call_stack_size = opts.call_stack_size;

    DoLoadFile(opts.loading_options);

    memoryView = new MemoryView();
    memory->SetView(memoryView, opts.exact_column_sizing, opts.column_sizes);
    memory->SetDisassembleLevel(opts.disassemble);
    memory->SetUnsignedMode(false);
    memory->SetHighlight(opts.highlight);

    int widths[5] = {-6, -2, -2, -2, -2};
    int styles[5] = {wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL};
    statusBar->SetStatusWidths(5, widths);
    statusBar->SetStatusStyles(5, styles);

    UpdateMemory();
    UpdateStatus();

    Connect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_COMPLETED, wxThreadEventHandler(ComplxFrame::OnRunComplete));
    Connect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_IO, wxThreadEventHandler(ComplxFrame::OnIo));
    Connect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_NOIO, wxThreadEventHandler(ComplxFrame::OnNoIo));
    Connect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_OUTPUT, wxThreadEventHandler(ComplxFrame::OnOutput));
}

/** ~ComplxFrame
  *
  * Destructor
  */
ComplxFrame::~ComplxFrame()
{
    auto* config = wxConfigBase::Get();

    int width, height;
    GetSize(&width, &height);

    config->Write("/appwidth", wxString::Format("%d", width));
    config->Write("/appheight", wxString::Format("%d", height));

    const auto& column_sizes = memory->GetColSizes();
    std::stringstream oss;
    for (unsigned int i = 0; i < MemorySize; i++)
    {
        oss << column_sizes.GetSize(i) << ",";
    }
    wxString sizes = oss.str();
    config->Write("/columnsizes", sizes);

    complxframe = NULL;
    Disconnect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_COMPLETED, wxThreadEventHandler(ComplxFrame::OnRunComplete));
    Disconnect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_IO, wxThreadEventHandler(ComplxFrame::OnIo));
    Disconnect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_NOIO, wxThreadEventHandler(ComplxFrame::OnNoIo));
    Disconnect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_OUTPUT, wxThreadEventHandler(ComplxFrame::OnOutput));
    {
        wxCriticalSectionLocker enter(threadCS);

        if (thread != NULL)
        {
            thread->Delete();
            thread = NULL;
        }
    }
    DestroyImages();
    Destroy();
}

/** OnIdle
  *
  * Called once when the app starts.
  */
void ComplxFrame::OnIdle(wxIdleEvent& event)
{
    Disconnect(wxEVT_IDLE, wxIdleEventHandler(ComplxFrame::OnIdle));
    UpdateRegisters();
    UpdateMemory();
}

/** OnLoad
  *
  * Called when a file needs to be loaded.
  */
void ComplxFrame::OnLoad(wxCommandEvent& event)
{
    if (Running()) return;
    wxFileDialog* dialog = new wxFileDialog(NULL, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (dialog->ShowModal() == wxID_OK)
    {
        LoadingOptions opts;
        opts.file = dialog->GetPath().ToStdString();
        DoLoadFile(opts);
    }

    delete dialog;
}


/** OnCleanLoad
  *
  * Called when a file needs to be loaded with zero'd memory/registers.
  */
void ComplxFrame::OnCleanLoad(wxCommandEvent& event)
{
    if (Running()) return;
    wxFileDialog* dialog = new wxFileDialog(NULL, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (dialog->ShowModal() == wxID_OK)
    {
        LoadingOptions opts;
        opts.file = dialog->GetPath().ToStdString();
        opts.memory = ZEROED;
        opts.registers = ZEROED;
        DoLoadFile(opts);
    }

    delete dialog;
}

void ComplxFrame::OnAdvancedLoad(wxCommandEvent& event)
{
    if (Running()) return;

    AdvancedLoadDialog* dialog = new AdvancedLoadDialog(this, reload_options);
    if (dialog->ShowModal() == wxID_OK)
    {
        LoadingOptions options = dialog->GetOptions();
        if (options.file.empty())
        {
            wxMessageBox("No file to load", "Error");
            delete dialog;
            return;
        }
        reload_options = options;
        DoLoadFile(reload_options);
        if (!reload_options.replay_string.empty())
            DoSetupReplayString(reload_options.replay_string);
    }
    delete dialog;
}

/** OnReload
  *
  * Called when the user wants to reload the last file.
  */
void ComplxFrame::OnReload(wxCommandEvent& event)
{
    if (Running()) return;
    if (reload_options.file.empty())
    {
        OnLoad(event);
        return;
    }

    DoLoadFile(reload_options);
}


/** DoLoadFile
  *
  * Handles loading an assembly file into the simulator
  */
void ComplxFrame::DoLoadFile(const LoadingOptions& opts)
{
    auto* config = wxConfigBase::Get();

    wxFileName filename(opts.file);
    bool randomize_registers = opts.registers == RANDOMIZE;
    bool randomize_memory = opts.memory == RANDOMIZE;
    short fill_registers = opts.registers;
    short fill_memory = opts.memory;

    lc3_init(state, randomize_registers, randomize_memory, fill_registers, fill_memory);
    state.pc = opts.pc;

    PostInit();

    // Now the actual loading
    if (opts.file.empty())
        return;

    lc3_state dummy_state;
    lc3_init(dummy_state);

    // Save the symbols
    std::map<std::string, unsigned short> symbol_table = state.symbols;
    std::map<unsigned short, std::string> rev_symbol_table = state.rev_symbols;

    state.symbols.clear();
    state.rev_symbols.clear();
    lc3_remove_plugins(state);

    bool tvt_modification = false;
    bool ivt_modification = false;
    bool subroutine_found = false;

    try
    {
        ///TODO should only make one call to lc3_assemble.
        std::vector<code_range> ranges;
        LC3AssembleOptions options;
        options.multiple_errors = false;
        lc3_assemble(dummy_state, filename.GetFullPath().ToStdString(), options);
        options.multiple_errors = true;
        options.warnings_as_errors = false;
        options.process_debug_comments = true;
        options.enable_warnings = false;
        options.disable_plugins = false;
        lc3_assemble(state, filename.GetFullPath().ToStdString(), ranges, options);

        // Update list of addresses modified for Show only Code/Data option.
        modified_addresses.clear();
        for (const auto& code_range : ranges)
            modified_addresses.push_back(ViewRange(code_range.location, code_range.location + code_range.size));
        /// TODO Automatically determine state of true traps and interrupts via the code ranges.
        // Dummy call to update hidden addresses.
        wxCommandEvent event;
        OnUpdateHideAddresses(event);
        // Check for TVT and IVT modification
        for (const auto& code_range : ranges)
        {
            if ((code_range.location >= 0 && code_range.location <= 0xFF) || (code_range.location + code_range.size >= 0 && code_range.location + code_range.size <= 0xFF))
                tvt_modification = true;
            if ((code_range.location >= 0x100 && code_range.location <= 0x1FF) || (code_range.location + code_range.size >= 0x100 && code_range.location + code_range.size <= 0x1FF))
                ivt_modification = true;
        }
        subroutine_found = DetectSubroutine(ranges);
    }
    catch (LC3AssembleException e)
    {
        wxMessageBox(wxString::Format("ERROR! %s", e.what()), wxString::Format("Loading %s failed", filename.GetFullName()));
        goto merge;
    }
    catch (std::vector<LC3AssembleException> e)
    {
        std::stringstream oss;
        for (unsigned int i = 0; i < e.size(); i++)
            oss << e[i].what() << std::endl;
        wxMessageBox(wxString::Format("ERROR! %s", oss.str()), wxString::Format("Loading %s failed", filename.GetFullName()));
        goto merge;
    }

    lc3_set_true_traps(state, opts.true_traps || tvt_modification);
    state.interrupt_enabled = opts.interrupts || ivt_modification;
    console->SetInput(opts.console_input);
    state.strict_execution = opts.strict_execution;
    // Update menus
    menuStateTrueTraps->Check(opts.true_traps || tvt_modification);
    menuStateInterrupts->Check(opts.interrupts || ivt_modification);
    menuStateStrictExecution->Check(opts.strict_execution);

    if (tvt_modification)
    {
        bool tvt_popup = config->Read("/firsttimetrap", "").IsEmpty();
        if (tvt_popup)
        {
            wxMessageBox("Pardon the interruption!\n"
                         "It appears you have loaded a file with a custom trap.\n"
                         "This will automatically enable true traps mode.\n"
                         "This will allow you to step into the code for the standard traps (IN,OUT,GETC,PUTS,HALT) and your own custom trap.\n"
                         "This notice to to remind you of the above, executing HALT will jump to some random code, but I assure you this random code is the code to HALT the LC-3.\n"
                         "If you'd rather stop the execution right at the HALT statement then put a breakpoint on your HALT statement.\n",
                         "Notice");
            config->Write("/firsttimetrap", "1");
        }
    }

    if (subroutine_found)
    {
        wxCommandEvent event;
        OnControlModeAdvanced(event);
        bool subroutine_popup = config->Read("/firsttimesubroutine", "").IsEmpty();
        if (subroutine_popup)
        {
            wxMessageBox("Pardon the interruption!\n"
                         "It appears you have loaded a file with a custom trap or subroutine.\n"
                         "You may notice 3 new buttons in the control area.\n"
                         "Next Line\n    Allows you to STEP OVER a subroutine or trap, meaning it will skip over it (but still execute it).\n"
                         "Prev Line\n    Allows you to BACK STEP OVER a subroutine or trap.\n"
                         "Finish\n    Allows you to STEP OUT of a subroutine, it finishes execution of it.\n\n"
                         "Another thing to note if you want to always step over/out a subroutine to select the subroutine label and select mark as blackbox.\n"
                         "If you use step on a JSR/JSRR/TRAP instruction that is blackboxed it will never step into it.\n"
                         "For more information Read The Manual :)\n"
                         , "Notice");
            config->Write("/firsttimesubroutine", "1");
        }
    }

    SetTitle(wxString::Format("%s - %s", base_title, filename.GetFullPath()));

merge:
    std::map<std::string, unsigned short>::const_iterator i;
    std::map<unsigned short, std::string>::const_iterator j;
    for (i = symbol_table.begin(); i != symbol_table.end(); ++i)
    {
        state.symbols[i->first] = i->second;
    }
    for (j = rev_symbol_table.begin(); j != rev_symbol_table.end(); ++j)
    {
        state.rev_symbols[j->first] = j->second;
    }

    UpdateStatus();
    UpdateRegisters();
    UpdateMemory();

    // Save in reload options
    reload_options = opts;

    // Update timestamps
    wxFileName file_loaded(reload_options.file);
    reload_options.file_modification_time = file_loaded.GetModificationTime();
}

/** PostInit
  *
  * Called after initializing.
  */
void ComplxFrame::PostInit()
{
    static bool first = true;
    if (console != NULL) delete console;
    console = new LC3Console(this);

    state.input = &console->inputStream;
    state.reader = complx_reader;
    state.peek = complx_peek;
    state.writer = complx_writer;
    // For keyboard interrupts
    state.interrupt_test.push_back(complx_step);

    console->Show();

    int x, y;
    GetScreenPosition(&x, &y);
    if (first)
    {
        Move(x + console->GetSize().GetX() / 2, y);
        console->Move(x - console->GetSize().GetX() / 2, y);
        first = false;
    }
    else
    {
        console->Move(x - console->GetSize().GetX(), y);
    }

    state.max_stack_size = stack_size;
    state.max_call_stack_size = call_stack_size;
    // Fix for console stealing focus.
    SetFocus();
}

void ComplxFrame::OnActivate(wxActivateEvent& event)
{
    bool reload_asm = false;
    bool reload_tests = false;
    /*if (!reload_options.tests.empty())
    {
        wxFileName tests_file(reload_options.tests);
        if (tests_file.GetModificationTime().IsLaterThan(reload_options.tests_modification_time))
        {
            // To prevent multiple occurrences
            reload_options.tests_modification_time = tests_file.GetModificationTime();
            int answer = wxMessageBox(wxString::Format("%s has been modified.\n"
                                                       "Do you wish to reload it?\n"
                                                       "(Note simulation will be reset and breakpoints "
                                                       "not present in code will not be reloaded.)", tests_file.GetFullName()),
                                      "Reload test xml file?", wxOK | wxOK_DEFAULT | wxCANCEL);
            reload_tests = answer == wxID_OK;
        }
    }*/
    if (!reload_options.file.empty())
    {
        wxFileName asm_file(reload_options.file);
        if (asm_file.GetModificationTime().IsLaterThan(reload_options.file_modification_time))
        {
            // To prevent multiple occurrences
            reload_options.file_modification_time = asm_file.GetModificationTime();
            if (!reload_tests)
            {
                int answer = wxMessageBox(wxString::Format("%s has been modified.\n"
                                                           "Do you wish to reload it?\n"
                                                           "(Note simulation will be reset and breakpoints "
                                                           "not present in code will not be reloaded.)", asm_file.GetFullName()),
                                          "Reload asm file?", wxOK | wxOK_DEFAULT | wxCANCEL);
                reload_asm = answer == wxOK;
            }
        }
    }

    if (reload_asm)
        DoLoadFile(reload_options);
}

/** OnQuit
  *
  * Called when the user quits the program.
  */
void ComplxFrame::OnQuit(wxCommandEvent& event)
{
    Destroy();
}

/** OnRegisterChanged
  *
  * Called when a registers value is changed.
  */
void ComplxFrame::OnRegisterChanged(wxCommandEvent& event)
{
    wxTextCtrl* text = static_cast<wxTextCtrl*>(event.GetEventObject());

    wxString name = text->GetName();
    std::string strdata = text->GetValue().ToStdString();
    int data;
    int error = lc3_calculate(state, strdata, data);

    if (name == _("CC") && strdata.size() == 1)
    {
        error = 0;
        if (strdata[0] == 'N' || strdata[0] == 'n')
            data = -1;
        else if (strdata[0] == 'Z' || strdata[0] == 'z')
            data = 0;
        else if (strdata[0] == 'P' || strdata[0] == 'p')
            data = 1;
        else
            error = 1;
    }

    if (error) lc3_warning(state, "Unable to parse expression " + strdata);
    if (name == _("PC"))
    {
        if (!error) state.pc = (unsigned short) data;
        UpdateRegister(text, error ? state.pc : data, 8);
        memory->SelectLocation(0);
        memory->SelectLocation(state.pc);
        memory->Refresh();
    }
    else if (name == _("CC"))
    {
        if (!error) lc3_setcc(state, data);
        int cc = state.n ? -1 : (state.z ? 0 : 1);
        UpdateRegister(text, error ? cc : data, 9);
    }
    else
    {
        int reg = name[1] - '0';
        if (!error) state.regs[reg] = (short) data;
        assert(reg <= 7);
        UpdateRegister(text, error ? state.regs[reg] : data, reg);
    }
}

/** OnTextKillFocus
  *
  * Reverts whatever textbox to the proper text
  */
void ComplxFrame::OnTextKillFocus(wxFocusEvent& event)
{
    wxTextCtrl* text = static_cast<wxTextCtrl*>(event.GetEventObject());
    wxString name = text->GetName();

    if (name == _("PC"))
    {
        UpdateRegister(text, state.pc, 8);
    }
    else if (name == _("CC"))
    {
        int cc = state.n ? -1 : (state.z ? 0 : 1);
        UpdateRegister(text, cc, 9);
    }
    else
    {
        int reg = name[1] - '0';
        assert(reg <= 7);
        UpdateRegister(text, state.regs[reg], reg);
    }
    event.Skip();
}

/** OnBaseChange
  *
  * Called when the user changes the base of a textbox
  */
void ComplxFrame::OnBaseChange(wxMouseEvent& event)
{
    wxTextCtrl* text = static_cast<wxTextCtrl*>(event.GetEventObject());

    wxString name = text->GetName();

    int index = 0;
    int value = 0;
    if (name == "PC")
    {
        index = 8;
        value = state.pc;
    }
    else if (name == "CC")
    {
        index = 9;
        value = state.n ? -1 : (state.z ? 0 : 1);
    }
    else
    {
        index = name[1] - '0';
        assert(index <= 7);
        value = state.regs[index];
    }

    int mode = register_display[index];
    if (mode == BASE_2)
        mode = BASE_10;
    else if (mode == BASE_10)
        mode = BASE_16;
    else if (mode == BASE_16)
    {
        //TODO register display as a character.
        //if (value > 0 && value <= 255)
        //    mode = BASE_CHAR;
        //else
            mode = BASE_2;
    }
    //else if (mode == BASE_CHAR)
    //    mode = BASE_2;
    register_display[index] = mode;

    UpdateRegister(text, value, index);
}

/** OnBaseChangeContext
  *
  * Called when the user right clicks on a register textbox.
  */
void ComplxFrame::OnBaseChangeContext(wxMouseEvent& event)
{
    ///TODO implement.
}

/** OnClearConsole
  *
  * Clears the console output.
  */
void ComplxFrame::OnClearConsole(wxCommandEvent& event)
{
    console->ClearOutput();
}

/** OnClearConsoleInput
  *
  * Clears the console input.
  */
void ComplxFrame::OnClearConsoleInput(wxCommandEvent& event)
{
    console->ClearInput();
}

void ComplxFrame::OnControlModeSimple(wxCommandEvent& event)
{
    nextLineButton->Hide();
    prevLineButton->Hide();
    finishButton->Hide();
    controlSizer->Layout();
}

void ComplxFrame::OnControlModeAdvanced(wxCommandEvent& event)
{
    nextLineButton->Show();
    prevLineButton->Show();
    finishButton->Show();
    controlSizer->Layout();
}

/** OnRun
  *
  * Called when the user clicks the run button (runs the program until it halts)
  */
void ComplxFrame::OnRun(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_RUN);
}

/** OnRunFor
  *
  * Called when the user clicks the run for button (runs the program for X steps prompting them for X)
  */
void ComplxFrame::OnRunFor(wxCommandEvent& event)
{
    // If the thread is not null then just call setup execution to stop.
    if (thread != NULL)
    {
        // Waiting on IO
        if (thread->IsPaused()) return;
        SetupExecution(RUNMODE_RUN);
        return;
    }

    RunForDialog* dialog = new RunForDialog(this, runforRunTime);
    if (dialog->ShowModal() == wxID_OK)
    {
        int retVal = dialog->GetValue();
        if (retVal != 0)
        {
            runforRunTime = retVal;
            SetupExecution(RUNMODE_RUNFOR, runforRunTime);
        }
    }
    delete dialog;
}

/** OnRunAgain
  *
  * Called when the user clicks the run for button (runs the program for X steps)
  */
void ComplxFrame::OnRunAgain(wxCommandEvent& event)
{
    if (runforRunTime == 0)
    {
        OnRunFor(event);
        return;
    }

    // If the thread is not null then just call setup execution to stop.
    if (thread != NULL)
    {
        // Waiting on IO
        if (thread->IsPaused()) return;
        SetupExecution(RUNMODE_RUN);
        return;
    }

    SetupExecution(RUNMODE_RUNFOR, runforRunTime);
    SetFocus();
}

/** OnStep
  *
  * Called when the user clicks the step button (runs the next instruction)
  */
void ComplxFrame::OnStep(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_STEP);
}

/** OnBackStep
  *
  * Called when the user clicks the back button (step back one instruction)
  */
void ComplxFrame::OnBackStep(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_BACK);
}

/** OnNextLine
  *
  * Called when the user clicks the next line button (runs the next instruction does not go into subroutines)
  */
void ComplxFrame::OnNextLine(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_NEXTLINE);
}

/** OnPrevLine
  *
  * Called when the user clicks the prev line button (goes back one instruction but does not go into subroutines)
  */
void ComplxFrame::OnPrevLine(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_PREVLINE);
}

/** OnFinish
  *
  * Called when the user clicks on the finish button (Finishes the current subroutine)
  */
void ComplxFrame::OnFinish(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_FINISH);
}

/** OnRewind
  *
  * Called when the user clicks on the rewind button (Goes back to start).
  */
void ComplxFrame::OnRewind(wxCommandEvent& event)
{
    SetupExecution(RUNMODE_REWIND);
}

void ComplxFrame::SetupExecution(int run_mode, int runtime)
{
    wxCriticalSectionLocker enter(threadCS);
    if (run_mode != RUNMODE_STEP && run_mode != RUNMODE_BACK)
        UpdatePhrase("&Stop");
    if (thread != NULL)
    {
        if (thread->IsPaused()) return;
        if (thread->IsRunning())
        {
            thread->Delete();
            thread = NULL;
            std::cout.flush();
            console->Update();
            UpdatePlay();
            UpdateRegisters();
            UpdateMemory();
            UpdateStatus();
            return;
        }
        else
        {
            thread->Delete();
            thread = NULL;
            return;
        }
    }
    else
    {
        thread = new LC3RunThread(this, run_mode, runtime);
        thread->Create();
    }

    thread->SetPriority(0);
    thread->Run();
}

void ComplxFrame::OnRunComplete(wxThreadEvent& event)
{
    std::cout.flush();
    console->Update();
    UpdatePlay();
    UpdateRegisters();
    UpdateMemory();
    UpdateStatus();
}

/** @brief OnNoIo
  *
  * @todo: document this function
  */
void ComplxFrame::OnNoIo(wxThreadEvent& event)
{
    //printf("There is no time\nYour sword is enough\n");
    wxCriticalSectionLocker enter(threadCS);
    if (thread != NULL && !thread->IsPaused()) thread->Pause();
    if (!refreshednoio)
    {
        UpdatePhrase("&NO IO");
        UpdateRegisters();
        UpdateMemory();
        UpdateStatus();
        refreshednoio = true;
    }
}

/** @brief OnIo
  *
  * @todo: document this function
  */
void ComplxFrame::OnIo(wxThreadEvent& event)
{
    console->OnReadChar();
    //printf("Great I'll grab my stuff\n");
}

/** @brief OnOutput
  *
  * @todo: document this function
  */
void ComplxFrame::OnOutput(wxThreadEvent& event)
{
    console->Update();
    //printf("Squadallah we're off\n");
}

/** @brief OnIo
  *
  * @todo: document this function
  */
void ComplxFrame::OnGetIo()
{
    wxCriticalSectionLocker enter(threadCS);
    if (thread != NULL && thread->IsPaused())
    {
        refreshednoio = false;
        UpdatePhrase("&Stop");
        thread->Resume();
    }
}

/** UpdateRegisters
  *
  * Updates all register values.
  */
void ComplxFrame::UpdateRegisters(void)
{
    UpdateRegister(r0text, state.regs[0], 0);
    UpdateRegister(r1text, state.regs[1], 1);
    UpdateRegister(r2text, state.regs[2], 2);
    UpdateRegister(r3text, state.regs[3], 3);
    UpdateRegister(r4text, state.regs[4], 4);
    UpdateRegister(r5text, state.regs[5], 5);
    UpdateRegister(r6text, state.regs[6], 6);
    UpdateRegister(r7text, state.regs[7], 7);
    UpdateRegister(pctext, state.pc, 8);
    UpdateRegister(cctext, (state.n ? -1 : (state.z ? 0 : 1)), 9);
}

/** UpdateRegister
  *
  * Updates a registers value
  */
void ComplxFrame::UpdateRegister(wxTextCtrl* text, int value, int index)
{
    int strategy = register_display[index];
    std::stringstream string;
    std::bitset<16> b = value;

    switch(strategy)
    {
    case BASE_2:
        string << b;
        text->SetValue(wxString::FromUTF8(string.str().c_str()));
        break;
    case BASE_10:
        text->SetValue(wxString::Format(_("%d"), (short)value));
        break;
    case BASE_16:
        text->SetValue(wxString::Format(_("x%04X"), (unsigned short)value));
        break;
    case BASE_CHAR:
        if (value > 0 && value <= 255)
            text->SetValue(wxString::Format(_("'%c'"), value));
        else
            text->SetValue(wxString::Format(_("%d"), (short)value));
        break;
    case BASE_CC:
        text->SetValue(wxString::Format(_("%s"), value < 0 ? _("n") : (value == 0 ? _("z") : _("p"))));
        break;
    default:
        break;
    }
}

/** OnWatchpoint
  *
  * Called when the user selects new watchpoint
  */
void ComplxFrame::OnWatchpoint(wxCommandEvent& event)
{
    lc3_watchpoint_info info;

    info.is_reg = false;
    info.data = 0x3000;
    info.condition = "1";
    info.enabled = true;
    info.hit_count = 0;
    info.label = "";
    info.max_hits = -1;

    WatchpointDialog* dialog = new WatchpointDialog(this, info, true);
    if (dialog->ShowModal() == wxID_OK)
    {
        int error;
        info = dialog->GetInfo(error);

        if (error)
            PrintError(error);
        else
        {
            bool multiple;

            if (info.is_reg)
                multiple = state.reg_watchpoints.find(info.data) != state.reg_watchpoints.end();
            else
                multiple = state.mem_watchpoints.find(info.data) != state.mem_watchpoints.end();

            if (multiple)
                wxMessageBox(_("ERROR! You can't have two watchpoints refer to the same target"), _("Error"));
            else
            {
                if (info.is_reg)
                    state.reg_watchpoints[info.data] = info;
                else
                    state.mem_watchpoints[info.data] = info;
            }
        }
    }
    delete dialog;
}

/** OnAdvancedBreakpoint
  *
  * Called when the user wants to add an advanced breakpoint.
  */
void ComplxFrame::OnAdvancedBreakpoint(wxCommandEvent& event)
{
    memory->OnAdvancedpoint(event);
}

/** OnBreakpoint
  *
  * Called when the user wants to add a breakpoint
  */
void ComplxFrame::OnBreakpoint(wxCommandEvent& event)
{
    memory->OnBreakpoint(event);
}

/** OnTemppoint
  *
  * Called when the user wants to add a temporary breakpoint
  */
void ComplxFrame::OnTemppoint(wxCommandEvent& event)
{
    memory->OnTemppoint(event);
}

/** OnBlackbox
  *
  * Called when the user wants to add a blackbox
  */
void ComplxFrame::OnBlackbox(wxCommandEvent& event)
{
    memory->OnBlackbox(event);
}

void ComplxFrame::OnTraceFile(wxCommandEvent& event)
{
    wxString file = wxFileSelector("Save trace file (note these files can potentially be huge)", wxEmptyString, wxEmptyString, wxEmptyString, "Trace files (*.trace)|*.trace", wxFD_OVERWRITE_PROMPT | wxFD_SAVE);
    if (file.empty())
        return;

    wxFileName filename(file);
    filename.SetExt("trace");

    std::string full_path = filename.GetFullPath().ToStdString();

    state.trace.reset(new std::ofstream(full_path.c_str()));

    if (state.trace->good())
    {
        wxMessageBox(wxString::Format("Trace file set to %s", full_path), "Trace file set successfully");
    }
    else
    {
        state.trace.reset();
        wxMessageBox(wxString::Format("Trace file could not be set to %s", full_path), "Failed to set trace file");
    }
}

/** OnBreakAndWatchpoints
  *
  * Called when the user wants to display all breakpoints.
  */
void ComplxFrame::OnBreakAndWatchpoints(wxCommandEvent& event)
{
    DebugInfoDialog* infos = new DebugInfoDialog(this);

    if (infos->ShowModal() == wxID_OK)
    {
        state.breakpoints.clear();
        state.reg_watchpoints.clear();
        state.mem_watchpoints.clear();

        std::vector<lc3_breakpoint_info> breakpoints = infos->GetBreakpoints();
        std::vector<lc3_watchpoint_info> reg_watchpoints = infos->GetRegWatchpoints();
        std::vector<lc3_watchpoint_info> mem_watchpoints = infos->GetMemWatchpoints();

        for (unsigned int i = 0; i < breakpoints.size(); i++)
        {
            lc3_breakpoint_info info = breakpoints[i];
            state.breakpoints[info.addr] = info;
        }

        for (unsigned int i = 0; i < reg_watchpoints.size(); i++)
        {
            lc3_watchpoint_info info = reg_watchpoints[i];
            state.reg_watchpoints[info.data] = info;
        }

        for (unsigned int i = 0; i < mem_watchpoints.size(); i++)
        {
            lc3_watchpoint_info info = mem_watchpoints[i];
            state.mem_watchpoints[info.data] = info;
        }

        memory->Refresh();
    }
    delete infos;
}

/** OnTrueTraps
  *
  * Turns on/off true traps.
  */
void ComplxFrame::OnTrueTraps(wxCommandEvent& event)
{
    lc3_set_true_traps(state, ~state.true_traps);
    UpdateMemory();
}

/** OnInterrupts
  *
  * Turns on/off interrupts.
  */
void ComplxFrame::OnInterrupts(wxCommandEvent& event)
{
    state.interrupt_enabled = ~state.interrupt_enabled;
    UpdateMemory();
}

/** OnStrictExecution
  *
  * Toggles strict execution mode.
  */
void ComplxFrame::OnStrictExecution(wxCommandEvent& event)
{
    state.strict_execution = ~state.strict_execution;
    UpdateMemory();
}

/** OnUndoStack
  *
  * Sets the undo stack's size
  */
void ComplxFrame::OnUndoStack(wxCommandEvent& event)
{
    wxString str = wxGetTextFromUser(
                       wxString::Format(_("Enter new undo stack size\n\
                           It is currently %d instructions\n\
                           Enter -1 for infinite."), state.max_stack_size),
                       _("Undo Stack size"),
                       wxString::Format(_("%d"), state.max_stack_size),
                       this
                   );

    if (str.IsEmpty()) return;

    std::string strdata = str.ToStdString();
    int data;
    int error = lc3_calculate(state, strdata, data);

    if (error) PrintError(error);

    stack_size = data;
    state.max_stack_size = data;
}

/** OnCallStack
  *
  * Sets the call stack's size
  */
void ComplxFrame::OnCallStack(wxCommandEvent& event)
{
    if (Running()) return;

    if (state.call_stack.empty())
    {
        wxMessageBox("ERROR! You are not even in a subroutine.", "Complx");
        return;
    }

    CallStackDialog* infos = new CallStackDialog(this);
    int ret = infos->ShowModal();
    int frame = infos->GetFrame();
    if (frame == -1)
    {
        delete infos;
        return;
    }

    if (ret == infos->GetRewindId())
    {
        while (!state.undo_stack.empty() && state.call_stack.size() > (unsigned int)frame)
            lc3_back(state);
        UpdateRegisters();
        UpdateMemory();
        UpdateStatus();
    }
    else if (ret == infos->GetViewStackId())
    {
        lc3_subroutine_call& call = state.call_stack[frame];
        memory->SelectLocation((unsigned short) call.r6 > 0xFFF0U ? 0xFFFFU : 0U);
        memory->SelectLocation((unsigned short) call.r6);
        memory->Refresh();
    }

    delete infos;
}

/** OnSubroutineCall
  *
  * "Calls" a subroutine uses same environment as lc3test
  */
void ComplxFrame::OnSubroutineCall(wxCommandEvent& event)
{
    if (Running()) return;

    if (reload_options.file.empty())
        OnLoad(event);

    if (reload_options.file.empty())
    {
        wxMessageBox("ERROR! An assembly file must be loaded to perform this operation", "Error");
        return;
    }

    CallSubroutineDialog* dialog = new CallSubroutineDialog(this);
    int stack_location = 0;
    int subroutine_location = 0;
    std::vector<short> params;
    unsigned short halt_statement = 0;

    if (dialog->ShowModal() != wxID_OK)
        goto end;

    stack_location = lc3_sym_lookup(state, dialog->GetStack());
    subroutine_location = lc3_sym_lookup(state, dialog->GetSubroutine());
    if (stack_location == -1)
    {
        wxMessageBox(wxString::Format("ERROR! Stack location: %s was not found in symbol table", dialog->GetStack()), "Error");
        goto end;
    }
    if (subroutine_location == -1)
    {
        wxMessageBox(wxString::Format("ERROR! Subroutine location: %s was not found in symbol table", dialog->GetSubroutine()), "Error");
        goto end;
    }
    for (const auto& expr : dialog->GetParams())
    {
        int value_calc;
        if (lc3_calculate(state, expr, value_calc))
        {
            wxMessageBox(wxString::Format("ERROR! Parameter %s is a malformed expression", expr), "Error");
            goto end;
        }
        params.push_back((short)value_calc);
    }
    // Meh... find a halt statement there needs to be somewhere to go afterward.
    for (unsigned int i = 0x3000; i < 0x10000; i++)
    {
        if ((unsigned short)state.mem[i] == 0xF025)
        {
            halt_statement = i;
            break;
        }
    }
    if ((unsigned short)state.mem[halt_statement] != 0xF025)
    {
        wxMessageBox("ERROR! No halt statement found in your program", "Error");
        goto end;
    }

    reload_options.memory = dialog->IsRandomMemory() ? RANDOMIZE : ZEROED;
    reload_options.registers = dialog->IsRandomRegisters() ? RANDOMIZE : ZEROED;

    DoLoadFile(reload_options);

    state.regs[6] = state.mem[(unsigned short)stack_location] - params.size();
    state.pc = (unsigned short) subroutine_location;
    for (unsigned int i = 0; i < params.size(); i++)
        state.mem[(unsigned short)state.regs[6] + i] = params[i];
    state.regs[7] = halt_statement;

    UpdateRegisters();
    UpdateMemory();
    UpdateStatus();

end:
    delete dialog;
}

/** OnGoto
  *
  * Called when the user wants to goto a certain address
  */
void ComplxFrame::OnGoto(wxCommandEvent& event)
{
    ::OnGoto(memory);
}

/** OnUpdateHideAddresses
  *
  * Called when the user wants change the hide memory address setting
  */
void ComplxFrame::OnUpdateHideAddresses(wxCommandEvent& event)
{
    int mode;
    if (menuViewHideAddressesShowAll->IsChecked())
        mode = SHOW_ALL;
    else if (menuViewHideAddressesShowOnlyCodeData->IsChecked())
        mode = SHOW_MODIFIED;
    else if ((menuViewHideAddressesShowNonZero->IsChecked()))
        mode = SHOW_NONZERO;
    else
        mode = 0;
    ::OnUpdateHideAddresses(memory, memoryView, mode);
}

/** OnHideAddressesCustom
  *
  * Called when the user wants customize what memory addresses are shown
  */
void ComplxFrame::OnHideAddressesCustom(wxCommandEvent& event)
{
    ::OnHideAddressesCustom(memory, memoryView);
}

/** OnDumbDisassemble
  *
  * Sets the mode to just straight disassembling
  * Does not take into account symbols.
  */
void ComplxFrame::OnDumbDisassemble(wxCommandEvent& event)
{
    ::OnDumbDisassemble(memory);
}

/** OnNormalDisassemble
  *
  * Sets the disassemble level to normal mode.
  */
void ComplxFrame::OnNormalDisassemble(wxCommandEvent& event)
{
    ::OnNormalDisassemble(memory);
}

/** OnCDisassemble
  *
  * Sets the disassemble level to C-Like
  */
void ComplxFrame::OnCDisassemble(wxCommandEvent& event)
{
    ::OnCDisassemble(memory);
}

/** OnInstructionHighlight
  *
  * Turns on/off Instruction highlighting
  */
void ComplxFrame::OnInstructionHighlight(wxCommandEvent& event)
{
    ::OnInstructionHighlight(memory, event.IsChecked());
}

/** OnFlipMemory
  *
  * Turns on/off flipped memory display
  */
void ComplxFrame::OnFlipMemory(wxCommandEvent& event)
{
    ::OnFlipMemory(memory, event.IsChecked());
    memory->SelectLocation(state.pc);
}

/** OnNewView
  *
  * Called when the user wants to create a new memory view
  */
void ComplxFrame::OnNewView(wxCommandEvent& event)
{
    MemoryViewFrame* frame = new MemoryViewFrame(this, new MemoryView());
    views.push_back(frame);
    frame->Show();
    frame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ComplxFrame::OnDestroyView), NULL, this);
    frame->Raise();
}

/** OnDestroyView
  *
  * Called when the user destroys a view
  */
void ComplxFrame::OnDestroyView(wxCloseEvent& event)
{
    MemoryViewFrame* frame = static_cast<MemoryViewFrame*>(event.GetEventObject());
    views.erase(std::find(views.begin(), views.end(), frame));
    frame->Disconnect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ComplxFrame::OnDestroyView), NULL, this);
    frame->Destroy();
}

void ComplxFrame::OnSetupReplayString(wxCommandEvent& event)
{
#ifdef ENABLE_LC3_REPLAY
    if (Running())
        return;

    if (reload_options.file.empty())
        OnLoad(event);

    if (reload_options.file.empty())
    {
        wxMessageBox("An assembly file must be loaded to perform this operation", "Error");
        return;
    }

    std::string replay_str = DoAskForReplayString();

    if (replay_str.empty())
        return;

    DoSetupReplayString(replay_str);
#else
    wxMessageBox("Support for this menu function was not enabled.", "Error");
#endif
}
void ComplxFrame::OnReloadReplayString(wxCommandEvent& event)
{
#ifdef ENABLE_LC3_REPLAY
    if (reload_options.replay_string.empty() || reload_options.file.empty())
    {
        OnSetupReplayString(event);
        return;
    }

    DoSetupReplayString(reload_options.replay_string);
#else
    wxMessageBox("Support for this menu function was not enabled.", "Error");
#endif
}
void ComplxFrame::OnDescribeReplayString(wxCommandEvent& event)
{
#ifdef ENABLE_LC3_REPLAY
    std::string replay_str = reload_options.replay_string;
    if (replay_str.empty())
    {
        replay_str = DoAskForReplayString();

        if (replay_str.empty())
        {
            wxMessageBox("No replay string given.", "Error");
            return;
        }
    }

    try
    {
        std::string description = lc3_describe_replay(replay_str);
        wxMessageBox(description);
    }
    catch (std::string err)
    {
        wxMessageBox(err.c_str(), "Error");
        return;
    }
    catch (const char* err)
    {
        wxMessageBox(err, "Error");
        return;
    }
#else
    wxMessageBox("Support for this menu function was not enabled.", "Error");
#endif
}

std::string ComplxFrame::DoAskForReplayString()
{
#ifdef ENABLE_LC3_REPLAY
    wxString replay = wxGetTextFromUser("Enter Replay String", "Testing");
    if (replay.IsEmpty())
    {
        wxMessageBox("No replay string given", "Error");
        return "";
    }
    return replay.ToStdString();
#else
    return "";
#endif
}

void ComplxFrame::DoSetupReplayString(const std::string& replay_string)
{
#ifdef ENABLE_LC3_REPLAY
    try
    {
        std::stringstream input;
        lc3_setup_replay(state, reload_options.file, replay_string, input);
        console->SetInput(input.str());
        reload_options.replay_string = replay_string;
        UpdateRegisters();
        UpdateMemory();
        UpdateStatus();
    }
    catch (std::string err)
    {
        wxMessageBox(err.c_str(), "Error");
        return;
    }
    catch (const char* err)
    {
        wxMessageBox(err, "Error");
        return;
    }
#endif
}

/** OnAbout
  *
  * Displays information about this program
  */
void ComplxFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("Complx");
    aboutInfo.SetVersion(Version::FULLVERSION_STRING);
    aboutInfo.SetDescription(_("LC-3 Simulator\nBug reports, thanks, and feature requests should be sent to Brandon.\nbwhitehead0308@gmail.com"));
    aboutInfo.SetCopyright("(C) 2010-2018");
    aboutInfo.AddDeveloper("Brandon Whitehead bwhitehead0308@gmail.com");
    aboutInfo.SetIcon(wxIcon(icon64_xpm));

    wxAboutBox(aboutInfo);
}

/** OnManual
  *
  * Displays the Complx reference manual.
  */
void ComplxFrame::OnDocs(wxCommandEvent& event)
{
    wxFileName manual(_(EXPAND_AND_STRINGIFY(PREFIX) "/share/doc/complx-tools/complx-tools.pdf"));
    manual.Normalize();
    wxLaunchDefaultBrowser(manual.GetFullPath());
}

/** OnISA
  *
  * Displays the lc3 ISA reference manual.
  */
void ComplxFrame::OnISA(wxCommandEvent& event)
{
    wxFileName manual(_(EXPAND_AND_STRINGIFY(PREFIX) "/share/doc/complx-tools/PattPatelAppA.pdf"));
    manual.Normalize();
    wxLaunchDefaultBrowser(manual.GetFullPath());
}

/** OnChangeLog
  *
  * Displays the change log
  */
void ComplxFrame::OnChangeLog(wxCommandEvent& event)
{
    wxFileName manual(_(EXPAND_AND_STRINGIFY(PREFIX) "/share/doc/complx-tools/ComplxChangeLog.txt"));
    manual.Normalize();
    wxLaunchDefaultBrowser(manual.GetFullPath());
}

void ComplxFrame::OnCreateBugReport(wxCommandEvent& event)
{
    wxFileName manual(_("https://github.com/TricksterGuy/complx/issues/new"));
    wxLaunchDefaultBrowser(manual.GetFullPath());
}

void ComplxFrame::OnFirstTime(wxCommandEvent& event)
{
    wxMessageBox(wxString::Format("Hi! You are currently running version %s of Complx.\n\n"
                                  "Since this is your first time running this program, here are a couple of tips.\n\n"
                                  "Thing 0: Write your assembly code in pluma/gedit (or any text editor) and save it as myfile.asm\n"
                                  "Thing 1: Report any bugs to TAs or me directly (bwhitehead0308@gmail.com).\n\n"
                                  "File issues at https://github.com/TricksterGuy/complx/issues or via the menus at Help > Send Bug Report\n",
                                  Version::FULLVERSION_STRING),
                 "Hi from Brandon", wxICON_INFORMATION | wxOK);
}

void ComplxFrame::OnTips(wxCommandEvent& event)
{
    wxConfigBase* config = wxConfigBase::Get();
    size_t currentTip = config->Read("/currenttip", 0l);
    bool show = true;
    config->Read("/showtips", &show);
    wxTipProvider* tip = wxCreateFileTipProvider(EXPAND_AND_STRINGIFY(PREFIX) "/share/doc/complx-tools/complx-tips.txt", currentTip);
    show = wxShowTip(complxframe, tip, show);

    config->Write("/showtips", show);
    config->Write("/currenttip", (long)tip->GetCurrentTip());
    config->Flush();
    delete tip;
}

/** UpdateMemory
  *
  * Updates the Memory View Tracking the current pc
  */
void ComplxFrame::UpdateMemory(void)
{
    memory->SelectLocation(state.pc);
    for (unsigned int i = 0; i < views.size(); i++)
        views[i]->Refresh();
}

/** UpdateStatus
  *
  * Updates the status bar
  */
void ComplxFrame::UpdateStatus(void)
{
    statusBar->SetStatusText(wxString::Format(_("Executed %d"), state.executions), 1);
    statusBar->SetStatusText(wxString::Format(_("Warnings %d"), state.warnings), 2);
    statusBar->SetStatusText(wxString::Format(_("True Traps %s"), state.true_traps ? "ON" : "OFF"), 3);
    statusBar->SetStatusText(wxString::Format(_("Interrupts %s"), state.interrupt_enabled ? "ON" : "OFF"), 4);
}

/** UpdatePlay
  *
  * Changes the buttons to the play version.
  */
void ComplxFrame::UpdatePlay(void)
{
    runButton->SetLabel(_("&Run"));
    stepButton->SetLabel(_("&Step"));
    backStepButton->SetLabel(_("&Back"));
    nextLineButton->SetLabel(_("&Next Line"));
    prevLineButton->SetLabel(_("&Prev Line"));
    finishButton->SetLabel(_("&Finish"));
    rewindButton->SetLabel(_("Rewin&d"));
}

/** UpdatePhrase
  *
  * Changes the buttons to all say a phrase
  */
void ComplxFrame::UpdatePhrase(const wxString& message)
{
    runButton->SetLabel(message);
    stepButton->SetLabel(message);
    backStepButton->SetLabel(message);
    nextLineButton->SetLabel(message);
    prevLineButton->SetLabel(message);
    finishButton->SetLabel(message);
    rewindButton->SetLabel(message);
}

/** @brief Running
  *
  * @todo: document this function
  */
bool ComplxFrame::Running()
{
    wxCriticalSectionLocker enter(threadCS);
    // If no thread then you aren't running
    if (thread == NULL) return false;
    if (thread->IsPaused() || thread->IsRunning())
    {
        wxMessageBox("ERROR! Pause execution first", "Complx");
        return true;
    }
    return false;
}

/** PrintError
  *
  * Displays an error message for screwing up an expression.
  */
void PrintError(int error)
{
    wxString msg = wxEmptyString;

    switch(error)
    {
    case 1:
        msg = _("Error evaluating expression: Unbalanced parenthesis");
        break;
    case 2:
        msg = _("Error evaluating expression: Invalid Operator");
        break;
    case 3:
        msg = _("Error evaluating expression: Invalid operand");
        break;
    case 4:
        msg = _("Error evaluating expression: Malformed reference");
        break;
    case 5:
        msg = _("Error evaluating expression: Undefined symbol");
        break;
    default:
        msg = _("Error evaluating expression.");
        break;
    }

    wxMessageBox(msg, _("Error"));
}

void InitImages()
{
    /// TODO this code may have to be updated.
    wxMemoryDC painter;

    wxBitmap nully(22, 16);
    wxBitmap breakpoint(22, 16);
    wxBitmap dispoint(22, 16);
    wxBitmap blackbox(22, 16);
    wxBitmap disblackbox(22, 16);
    wxBitmap pc(22, 16);

    wxPoint tri[3];
    tri[0].x = 12;
    tri[0].y = 0;
    tri[1].x = 12 ;
    tri[1].y = 14;
    tri[2].x = 21;
    tri[2].y = 7;

    painter.SelectObject(breakpoint);
    painter.SetBrush(*wxBLUE_BRUSH);
    painter.SetPen(*wxBLUE_PEN);
    painter.DrawRectangle(0, 0, 22, 16);
    painter.SetBrush(*wxRED_BRUSH);
    painter.SetPen(*wxBLACK_PEN);
    painter.DrawRectangle(0, 0, 5, 15);

    painter.SelectObject(wxNullBitmap);
    painter.SelectObject(dispoint);
    painter.SetBrush(*wxBLUE_BRUSH);
    painter.SetPen(*wxBLUE_PEN);
    painter.DrawRectangle(0, 0, 22, 16);
    painter.SetBrush(*wxGREY_BRUSH);
    painter.SetPen(*wxBLACK_PEN);
    painter.DrawRectangle(0, 0, 5, 15);

    painter.SelectObject(wxNullBitmap);
    painter.SelectObject(blackbox);
    painter.SetBrush(*wxBLUE_BRUSH);
    painter.SetPen(*wxBLUE_PEN);
    painter.DrawRectangle(0, 0, 22, 16);
    painter.SetBrush(wxBrush(wxColor(32, 32, 32)));
    painter.SetPen(*wxBLACK_PEN);
    painter.DrawRectangle(6, 0, 5, 15);

    painter.SelectObject(wxNullBitmap);
    painter.SelectObject(disblackbox);
    painter.SetBrush(*wxBLUE_BRUSH);
    painter.SetPen(*wxBLUE_PEN);
    painter.DrawRectangle(0, 0, 22, 16);
    painter.SetBrush(wxBrush(wxColor(64, 64, 64)));
    painter.SetPen(*wxBLACK_PEN);
    painter.DrawRectangle(6, 0, 5, 15);

    painter.SelectObject(wxNullBitmap);
    painter.SelectObject(pc);
    painter.SetBrush(*wxBLUE_BRUSH);
    painter.SetPen(*wxBLUE_PEN);
    painter.DrawRectangle(0, 0, 22, 16);
    painter.SetPen(*wxBLACK_PEN);
    painter.SetBrush(wxBrush(wxColor(48, 176, 112)));
    painter.DrawPolygon(3, tri);

    painter.SelectObject(wxNullBitmap);

    infoImages->Add(nully, *wxBLUE);
    infoImages->Add(pc, *wxBLUE);
    infoImages->Add(breakpoint, *wxBLUE);
    infoImages->Add(dispoint, *wxBLUE);
    infoImages->Add(blackbox, *wxBLUE);
    infoImages->Add(disblackbox, *wxBLUE);
}

void DestroyImages()
{
    infoImages->RemoveAll();
}

bool ComplxFrame::DetectSubroutine(const std::vector<code_range>& ranges)
{
    for (const auto& range : ranges)
    {
        for (unsigned int i = 0; i < range.size; i++)
        {
            unsigned short data = state.mem[range.location + i];
            // JSR and JSRR share the same opcode.
            if (((data >> 12) & 0xF) == JSR_INSTR || data == 0xC1C0)
                return true;
        }
    }
    return false;
}
