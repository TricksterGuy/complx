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
#include <XmlTestParser.hpp>
#include <lc3.hpp>
#include <lc3_assemble.hpp>
#include "icon64.xpm"

#include "ComplxFrame.hpp"
#include "MemoryGrid.hpp"
#include "CallStackDialog.hpp"
#include "DebugInfoDialog.hpp"
#include "WatchpointDialog.hpp"
#include "AddressDialog.hpp"
#include "RunForDialog.hpp"
#include "RunTestDialog.hpp"
#include "LC3RunThread.hpp"
#include "version.h"

lc3_state state;
lc3_test test;

extern ComplxFrame* complxframe;
int runforRunTime = 0;
wxFileName currentFile;
wxFileName currentTestFile;
wxCriticalSection threadCS;
LC3RunThread* thread = NULL;
bool changed = false;
bool refreshednoio = false;

int complx_reader(lc3_state& state, std::istream& file);
int complx_peek(lc3_state& state, std::istream& file);
int complx_writer(lc3_state& state, std::ostream& file, int chr);
void complx_step(void);

// R0-R7, PC, CC
int register_display[10] = {BASE_10, BASE_10, BASE_10, BASE_10, BASE_10, BASE_16, BASE_16, BASE_16, BASE_16, BASE_CC};

wxImageList* infoImages = new wxImageList(22, 16, true);
void InitImages(void);
void DestroyImages(void);
void PrintError(int error);

/** ComplxFrame
  *
  * Constructor
  */
ComplxFrame::ComplxFrame(long disassemble, long stack_size, long call_stack_size, long true_traps, long interrupts, long highlight,
                         wxString address_str, wxArrayString files) : ComplxFrameDecl(NULL), console(NULL), memoryView(NULL)
{
    InitImages();

    menuStateTrueTraps->Check(true_traps == 1);
    menuStateInterrupts->Check(interrupts == 1);
    menuViewInstructionHighlighting->Check(highlight != 0);

    this->stack_size = stack_size;
    this->call_stack_size = call_stack_size;
    OnInit();

    if (!address_str.IsEmpty())
    {
        int addr;
        lc3_calculate(state, address_str.ToStdString(), addr);
        state.pc = addr;
    }

    if (files.size() > 0) DoLoadFile(wxFileName(files[0]));

    memoryView = new MemoryView();
    memory->SetView(memoryView);
    memory->SetDisassembleLevel(disassemble > 2 ? 2 : disassemble);
    memory->SetUnsignedMode(false);
    memory->SetHighlight(highlight > 1 ? 1 : highlight);

    int widths[3] = {-7, -3, -3};
    int styles[3] = {wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL};
    statusBar->SetStatusWidths(3, widths);
    statusBar->SetStatusStyles(3, styles);

    UpdateMemory();
    UpdateStatus();

    Connect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_COMPLETED, wxThreadEventHandler(ComplxFrame::OnRunComplete));
    Connect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_UPDATE, wxThreadEventHandler(ComplxFrame::OnRunUpdate));
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
    complxframe = NULL;
    Disconnect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_COMPLETED, wxThreadEventHandler(ComplxFrame::OnRunComplete));
    Disconnect(wxID_ANY, wxEVT_COMMAND_RUNTHREAD_UPDATE, wxThreadEventHandler(ComplxFrame::OnRunUpdate));
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
    this->Disconnect(wxEVT_IDLE, wxIdleEventHandler(ComplxFrame::OnIdle));
    UpdateRegisters();
    UpdateMemory();
}

/** OnRandomizeAndLoad
  *
  * Randomizes memory and loads a file
  */
void ComplxFrame::OnRandomizeAndLoad(wxCommandEvent& event)
{
    if (Running()) return;
    wxFileDialog* dialog = new wxFileDialog(NULL, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxFileName filename(dialog->GetPath());
        OnInit();
        lc3_randomize(state);
        DoLoadFile(filename);
    }
}

/** OnRandomizeAndReload
  *
  * Called when the user wants to reload the last file.
  */
void ComplxFrame::OnRandomizeAndReload(wxCommandEvent& event)
{
    if (Running()) return;
    if (currentFile.GetFullName().IsEmpty())
    {
        OnLoad(event);
        return;
    }

    OnInit();
    lc3_randomize(state);
    DoLoadFile(currentFile);
}

/** OnLoad
  *
  * Called when a file needs to be loaded (and machine to be initialized)
  */
void ComplxFrame::OnLoad(wxCommandEvent& event)
{
    if (Running()) return;
    wxFileDialog* dialog = new wxFileDialog(NULL, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxFileName filename(dialog->GetPath());
        OnInit();
        DoLoadFile(filename);
    }

    delete dialog;
}

/** OnInit
  *
  * Called when the machine needs to be initialized
  */
void ComplxFrame::OnInit(void)
{
    static bool first = true;
    lc3_init(state, true);

    if (console != NULL) delete console;
    console = new LC3Console(this);

    state.input = &console->inputStream;
    state.reader = complx_reader;
    state.peek = complx_peek;
    state.writer = complx_writer;
    // For keyboard interrupts
    state.interrupt_test.push_back(complx_step);

    //console = new LC3Console(this);
    console->Show();
    //console->Iconize();
    //state.input = &console->inputStream;

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

    lc3_set_true_traps(state, menuStateTrueTraps->IsChecked());
    state.interrupt_enabled = menuStateInterrupts->IsChecked();
    state.max_stack_size = stack_size;
    state.max_call_stack_size = call_stack_size;
}

/** OnReload
  *
  * Called when the user wants to reload the last file.
  */
void ComplxFrame::OnReload(wxCommandEvent& event)
{
    if (Running()) return;
    if (currentFile.GetFullName().IsEmpty())
    {
        OnLoad(event);
        return;
    }

    OnInit();
    DoLoadFile(currentFile);
}

/** OnLoadOver
  *
  * Called when the user wants to load a new file over the current state
  */
void ComplxFrame::OnLoadOver(wxCommandEvent& event)
{
    if (Running()) return;
    wxFileDialog* dialog = new wxFileDialog(NULL, _("Load .asm file"), wxEmptyString, wxEmptyString, _("LC-3 Assembly Files (*.asm)|*.asm"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (dialog->ShowModal() == wxID_OK)
    {
        wxFileName filename(dialog->GetPath());
        DoLoadFile(filename);
    }

    delete dialog;
}

/** OnReloadOver
  *
  * Called when the user wants to reload the last file over the current state.
  */
void ComplxFrame::OnReloadOver(wxCommandEvent& event)
{
    if (Running()) return;
    if (currentFile.GetFullName().IsEmpty())
    {
        OnLoadOver(event);
        return;
    }

    DoLoadFile(currentFile);
}

/** DoLoadFile
  *
  * Handles loading an assembly file into the simulator
  */
void ComplxFrame::DoLoadFile(const wxFileName& filename)
{
    //CleanUp();
    lc3_state dummy_state;
    lc3_init(dummy_state);

    // Save the symbols
    std::map<std::string, unsigned short> symbol_table = state.symbols;
    std::map<unsigned short, std::string> rev_symbol_table = state.rev_symbols;

    state.symbols.clear();
    state.rev_symbols.clear();
    lc3_remove_plugins(state);

    try
    {
        lc3_assemble(dummy_state, filename.GetFullPath().ToStdString(), false);
        lc3_assemble(state, filename.GetFullPath().ToStdString());

    }
    catch (LC3AssembleException e)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", e.what()), _("Loading ") + filename.GetFullName() + _(" Failed"));
        goto merge;
    }
    catch (std::vector<LC3AssembleException> e)
    {
        std::stringstream oss;
        for (unsigned int i = 0; i < e.size(); i++)
            oss << e[i].what() << std::endl;
        wxMessageBox(wxString::Format("BAD STUDENT! %s", oss.str()), _("Loading ") + filename.GetFullName() + _(" Failed"));
        goto merge;
    }

    //if (DoAssemble(filename)) return;
    currentFile = filename;
    SetTitle(wxString::Format("Complx - %s", filename.GetFullPath()));
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

    //DoLoad(filename);
    UpdateStatus();
    UpdateRegisters();
    UpdateMemory();

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
        mode = BASE_2;
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
    console->Clear();
}

/** OnClearConsoleInput
  *
  * Clears the console input.
  */
void ComplxFrame::OnClearConsoleInput(wxCommandEvent& event)
{
    console->ClearInput();
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
    changed = false;
    if (thread != NULL)
    {
        if (thread->IsPaused()) return;
        if (thread->IsRunning())
        {
            thread->Delete();
            thread = NULL;
            changed = true;
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


void ComplxFrame::OnRunUpdate(wxThreadEvent& event)
{
    static int i = 0;

    i = (i + 1) % (/*1024 * */(!changed ? 3 : 9));

    if (i == 0 && !changed)
    {
        UpdatePhrase("&Stop");
        changed = true;
    }

    if (i == 0)
        console->Update();
}

void ComplxFrame::OnRunComplete(wxThreadEvent& event)
{
    std::cout.flush();
    console->Update();
    if (changed)
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
    //printf("There is no time\n");
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
        UpdatePlay();
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
            text->SetValue(wxString::Format(_("0x%04X"), (unsigned short)value));
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
                wxMessageBox(_("BAD STUDENT! You can't have two watchpoints refer to the same target"), _("Error"));
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

/** OnUndoStack
  *
  * Sets the undo stack's size
  */
void ComplxFrame::OnCallStack(wxCommandEvent& event)
{
    if (Running()) return;

    if (state.call_stack.empty())
    {
        wxMessageBox("BAD STUDENT! You are not even in a subroutine.", "Complx");
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


/** OnReinitialize
  *
  * Reinitializes the machine.
  */
void ComplxFrame::OnReinitialize(wxCommandEvent& event)
{
    OnInit();
    UpdateRegisters();
    UpdateMemory();
    UpdateStatus();
}

/** @brief OnRandomize
  *
  * Randomizes the machine
  */
void ComplxFrame::OnRandomize(wxCommandEvent& event)
{
    lc3_randomize(state);
    UpdateRegisters();
    UpdateMemory();
    UpdateStatus();
}

/** OnGoto
  *
  * Called when the user wants to goto a certain address
  */
void ComplxFrame::OnGoto(wxCommandEvent& event)
{
    wxString wxaddress = wxGetTextFromUser(_("Input Address"), _("Goto Address"), wxEmptyString, this);
    if (wxaddress.IsEmpty()) return;
    int addr;
    int error = lc3_calculate(state, wxaddress.ToStdString(), addr);

    if (error)
    {
        PrintError(error);
        return;
    }

    memory->SelectLocation((unsigned short) addr > 0xFFF0 ? 0xFFFF : 0);
    memory->SelectLocation((unsigned short) addr);
    memory->Refresh();
}

/** OnUpdateHideAddresses
  *
  * Called when the user wants change the hide memory address setting
  */
void ComplxFrame::OnUpdateHideAddresses(wxCommandEvent& event)
{
    ///TODO Implement
}

/** OnGoto
  *
  * Called when the user wants customize what memory addresses are shown
  */
void ComplxFrame::OnHideAddressesCustom(wxCommandEvent& event)
{
    ///TODO Implement
}

/** OnDumbDisassemble
  *
  * Sets the mode to just straight disassembling
  * Does not take into account symbols.
  */
void ComplxFrame::OnDumbDisassemble(wxCommandEvent& event)
{
    memory->SetDisassembleLevel(0);
    memory->Refresh();
}

/** OnNormalDisassemble
  *
  * Sets the disassemble level to normal mode.
  */
void ComplxFrame::OnNormalDisassemble(wxCommandEvent& event)
{
    memory->SetDisassembleLevel(1);
    memory->Refresh();
}

/** OnCDisassemble
  *
  * Sets the disassemble level to C-Like
  */
void ComplxFrame::OnCDisassemble(wxCommandEvent& event)
{
    memory->SetDisassembleLevel(2);
    memory->Refresh();
}

/** OnInstructionHighlight
  *
  * Turns on/off Instruction highlighting
  */
void ComplxFrame::OnInstructionHighlight(wxCommandEvent& event)
{
    memory->SetHighlight(event.IsChecked());
    memory->Refresh();
}

/** OnNewView
  *
  * Called when the user wants to create a new memory view
  */
void ComplxFrame::OnNewView(wxCommandEvent& event)
{
    MemoryViewFrame* frame = new MemoryViewFrame(this, memoryView);
    views.push_back(frame);
    //frame->InitGridSizes();
    frame->Show();

    frame->Connect(wxEVT_CLOSE_WINDOW, wxCloseEventHandler(ComplxFrame::OnDestroyView), NULL, this);
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
}

/** OnRunTests
  *
  *
  */
void ComplxFrame::OnRunTests(wxCommandEvent& event)
{
    if (Running()) return;

    if (!currentFile.IsOk())
        OnLoad(event);

    if (!currentFile.IsOk())
    {
        wxMessageBox("BAD STUDENT! An assembly file must be loaded to perform this operation", "Error");
        return;
    }

    wxFileDialog* dialog = new wxFileDialog(NULL, _("Load .xml file"), wxEmptyString, wxEmptyString, _("LC-3 Test Files (*.xml)|*.xml"), wxFD_OPEN|wxFD_FILE_MUST_EXIST|wxFD_CHANGE_DIR);
    if (dialog->ShowModal() != wxID_OK)
    {
        delete dialog;
        return;
    }

    wxFileName filename(dialog->GetPath());
    delete dialog;

    lc3_test_suite suite;
    if (TryLoadTests(suite, filename.GetFullPath()))
        return;

    currentTestFile = filename;

    RunTestDialog* rtdialog = new RunTestDialog(this, filename.GetFullName().ToStdString(), suite);
    rtdialog->ShowModal();
}

/** On RerunTests
  *
  *
  */
void ComplxFrame::OnRerunTests(wxCommandEvent& event)
{
    if (currentFile.GetFullName().IsEmpty())
    {
        OnRunTests(event);
        return;
    }

    lc3_test_suite suite;
    if (TryLoadTests(suite, currentTestFile.GetFullPath()))
        return;

    RunTestDialog* rtdialog = new RunTestDialog(this, currentTestFile.GetFullName().ToStdString(), suite);
    rtdialog->ShowModal();
}

bool ComplxFrame::TryLoadTests(lc3_test_suite& suite, const wxString& path)
{
    try
    {
        if (!XmlTestParser().LoadTestSuite(suite, path.ToStdString()))
        {
            wxMessageBox(_("BAD STUDENT! Xml file not found or parse errors found"), _("Error"));
            return true;
        }
    }
    catch (const char* c)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", c), _("Error"));
        return true;
    }
    catch (std::string c)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", c), _("Error"));
        return true;
    }

    return false;
}

/** OnAbout
  *
  * Displays information about this program
  */
void ComplxFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo aboutInfo;
    aboutInfo.SetName("Complx");
    aboutInfo.SetVersion(AutoVersion::FULLVERSION_STRING);
    aboutInfo.SetDescription(_("LC-3 Simulator\nBug reports, thanks, and feature requests should be sent to Brandon.\nbwhitehead0308@gmail.com"));
    aboutInfo.SetCopyright("(C) 2010-2015");
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
    wxFileName manual(_(EXPAND_AND_STRINGIFY(PREFIX) "/share/complx-tools/Complx.pdf"));
    manual.Normalize();
    wxLaunchDefaultBrowser(manual.GetFullPath());
}

/** OnISA
  *
  * Displays the lc3 ISA reference manual.
  */
void ComplxFrame::OnISA(wxCommandEvent& event)
{
    wxFileName manual(_(EXPAND_AND_STRINGIFY(PREFIX) "/share/complx-tools/PattPatelAppA.pdf"));
    manual.Normalize();
    wxLaunchDefaultBrowser(manual.GetFullPath());
}

/** OnChangeLog
  *
  * Displays the change log
  */
void ComplxFrame::OnChangeLog(wxCommandEvent& event)
{
    wxFileName manual(_(EXPAND_AND_STRINGIFY(PREFIX) "/share/complx-tools/ComplxChangeLog.txt"));
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
                                  "Since this is your first time running this program, here are a couple of things.\n\n"
                                  "Thing 0: Write your assembly code in pluma/gedit (or any text editor) and save it as myfile.asm\n"
                                  "Thing 1: Be thankful! I am the author of everything you are using.\n"
                                  "Thing 2: DO NOT start these homework assignments on the day it's due.\n"
                                  "Thing 3: Report any bugs to TAs or me directly (bwhitehead0308@gmail.com).\n\n"
                                  "  Or you may file an issue at https://github.com/TricksterGuy/complx/issues",
                                  AutoVersion::FULLVERSION_STRING),
                                  "Hi from Brandon", wxICON_INFORMATION | wxOK);
}

void ComplxFrame::OnTips(wxCommandEvent& event)
{
    wxConfigBase* config = wxConfigBase::Get();
    size_t currentTip = config->Read("/currenttip", 0l);
    bool show = true;
    config->Read("/showtips", &show);
    wxTipProvider* tip = wxCreateFileTipProvider(EXPAND_AND_STRINGIFY(PREFIX) "/share/complx-tools/complx-tips.txt", currentTip);
    show = wxShowTip(complxframe, tip, show);

    config->Write("/showtips", show);
    config->Write("/currenttip", tip->GetCurrentTip());
    config->Flush();
    delete tip;
}

/** UpdateMemory
  *
  * Updates the Memory View Tracking the current pc
  */
void ComplxFrame::UpdateMemory(void)
{
    memory->SelectLocation(0);
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
}

/** UpdatePlay
  *
  * Changes the buttons to the play version.
  */
void ComplxFrame::UpdatePlay(void)
{
    runButton->SetLabel(_("&Run"));
    runForButton->SetLabel(_("R&un For"));
    stepButton->SetLabel(_("&Step"));
    backStepButton->SetLabel(_("&Back"));
    nextLineButton->SetLabel(_("&Next Line"));
    prevLineButton->SetLabel(_("&Prev Line"));
    finishButton->SetLabel(_("&Finish"));
    rewindButton->SetLabel(_("Re&wind"));
}

/** UpdatePhrase
  *
  * Changes the buttons to all say a phrase
  */
void ComplxFrame::UpdatePhrase(const wxString& message)
{
    runButton->SetLabel(message);
    runForButton->SetLabel(message);
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
        wxMessageBox("BAD STUDENT! Pause execution first", "Complx");
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
            msg = _("BAD STUDENT! Error evaluating expression: Unbalanced parethesis");
            break;
        case 2:
            msg = _("BAD STUDENT! Error evaluating expression: Invalid Operator");
            break;
        case 3:
            msg = _("BAD STUDENT! Error evaluating expression: Invalid operand");
            break;
        case 4:
            msg = _("BAD STUDENT! Error evaluating expression: Malformed reference");
            break;
        case 5:
            msg = _("BAD STUDENT! Error evaluating expression: Undefined symbol");
            break;
        default:
            msg = _("BAD STUDENT! Error evaluating expression.");
            break;
    }

    wxMessageBox(msg, _("Error"));
}

void InitImages()
{
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
