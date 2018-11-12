#include <wx/textctrl.h>
#include <wx/string.h>
#include <wx/app.h>
#include <sstream>
#include "LC3Console.hpp"
#include "LC3RunThread.hpp"
#include "ComplxApp.hpp"
#include <lc3_all.hpp>

wxTextCtrl* stdinput;
std::stringstream oss;
std::streambuf* oldcout;

extern wxCriticalSection threadCS;
extern lc3_state state;

/** complx_reader
  *
  * Reads text from the textbox.
  */
int complx_reader(lc3_state& state, std::istream& file)
{
    char c = file.peek();

    if (!file.good()) wxQueueEvent(complxframe, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_NOIO));
    while (!file.good())
    {
        if (LC3RunThread::This()->TestDestroy())
            return -1;
        LC3RunThread::This()->Yield();
        c = file.peek();
    }
    file.read(&c, 1);
    wxQueueEvent(complxframe, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_IO));

    return c;
}

/** complx_peek
  *
  * Peeks at text from the textbox.
  */
int complx_peek(lc3_state& state, std::istream& file)
{
    char c = file.peek();

    if (!file.good()) wxQueueEvent(complxframe, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_NOIO));
    while (!file.good())
    {
        if (LC3RunThread::This()->TestDestroy())
            return -1;
        LC3RunThread::This()->Yield();
        c = file.peek();
        //printf("TESTING TESTING TESTING\n");
    }

    //Do not send an io event here.
    //Sending an io event "consumes a character"
    //No io will be unable by typing text and is handled elsewhere.

    return c;
}

/** complx_peek_noblock
  *
  * Peeks at text from the textbox without blocking
  */
int complx_peek_noblock(std::istream& file)
{
    char c;

    if (!file.good()) return -1;

    c = file.peek();

    return c;
}

/** complx_writer
  *
  *
  */
int complx_writer(lc3_state& state, std::ostream& file, int chr)
{
    wxCriticalSectionLocker enter(threadCS);
    if (!file.good()) return -1;
    file.put((char) chr);
    wxQueueEvent(complxframe, new wxThreadEvent(wxEVT_COMMAND_RUNTHREAD_OUTPUT));
    return (!file.good()) ? -1 : 0;
}

/** complx_step
  *
  *
  */
void complx_step(lc3_state& state)
{
    wxCriticalSectionLocker enter(threadCS);
    // If we aren't executing a keyboard interrupt
    if (state.interrupt_vector != 0x80)
    {
        //printf("noblock %d %d %x\n", complx_peek_noblock(*state.input), state.interrupt_enabled, state.mem[0xFE00]);
        // If interrupts are enabled for keyboard and interrupts are enabled and there is a character
        if (((state.mem[0xFE00] >> 14) & 1) && state.interrupt_enabled && complx_peek_noblock(*state.input) != -1)
        {
            int rr = (rand() % 16 < 5);
            //printf("randresult %d\n", rr);
            if (rr) lc3_keyboard_interrupt(state);
            //if (rr) printf("interrupt\n");
        }
    }
}

/** LC3Console
  *
  * Constructor
  */
LC3Console::LC3Console(wxWindow* parent) : LC3ConsoleDecl(parent)
{
    stdinput = input_text;
    output->SetValue("The output will appear in this textbox.\nYou may specify input in the textbox below.\nYou should ideally enter your text beforehand.\nIf there is no text the program will wait for your input and all buttons will say \"NO IO\".\n-------\n");
    //stotxt = new wxStreamToTextRedirector(output, &std::cout);

    oldcout = std::cout.rdbuf();
    std::cout.rdbuf(oss.rdbuf());
}

/** ~LC3Console
  *
  * Destructor
  */
LC3Console::~LC3Console()
{
    //delete stotxt;
    std::cout.rdbuf(oldcout);
}

void LC3Console::OnClose(wxCloseEvent& event)
{
    if (event.CanVeto())
        event.Veto();
    else
        Destroy();
}

///** OnChar
//  *
//  * Called when the user types a character.  Adds it to the buffer
//  */
//void LC3Console::OnChar(wxKeyEvent& event)
//{
//    int ascii = event.GetKeyCode();
//    if (!isascii(ascii)) return;
//    *input_text << (char)ascii;
//    inputStream.clear();
//    inputStream.put((char)ascii);
//    complxframe->OnGetIo();
//}

/** OnText
  *
  * Called when the text changes
  */
void LC3Console::OnText(wxCommandEvent& event)
{
    wxCriticalSectionLocker enter(threadCS);
    inputStream.clear();
    inputStream.seekg(0);
    inputStream.str("");
    inputStream.str(event.GetString().ToStdString());
    complxframe->OnGetIo();
}

/** @brief OnEnterKey
  *
  * @todo: document this function
  */
void LC3Console::OnEnterKey(wxCommandEvent& event)
{
    wxCriticalSectionLocker enter(threadCS);
    inputStream.clear();
    inputStream.seekg(0);
    inputStream.str("");
    wxString newstr = event.GetString() + "\n";
    inputStream.str(newstr.ToStdString());
    input_text->ChangeValue(newstr);
    input_text->SetInsertionPointEnd();
    complxframe->OnGetIo();
}

/** @brief OnReadChar
  *
  * @todo: document this function
  */
void LC3Console::OnReadChar()
{
    wxCriticalSectionLocker enter(threadCS);
    wxString value = input_text->GetValue();

    if (value.IsEmpty()) return;

    input_text->ChangeValue(value.Mid(1));
}

/** Update
  *
  * Called when the console needs to update the text or periodically when running.
  */
void LC3Console::Update()
{
    wxCriticalSectionLocker enter(threadCS);
    if (oss.tellp() != 0)
    {
        (*output) << oss.str();
        oss.seekp(0);
        oss.str("");
        if (!IsShown())
            Raise();
    }
}

/** ClearOutput
  *
  * Clears the console output
  */
void LC3Console::ClearOutput()
{
    wxCriticalSectionLocker enter(threadCS);
    output->SetValue(wxEmptyString);
}

void LC3Console::SetInput(const std::string& str)
{
    wxCriticalSectionLocker enter(threadCS);
    input_text->ChangeValue(str);
    inputStream.seekg(0);
    inputStream.str(str);
}
