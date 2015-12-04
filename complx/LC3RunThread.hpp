#ifndef LC3RUNTHREAD_HPP
#define LC3RUNTHREAD_HPP

#include <wx/thread.h>
#include "ComplxFrame.hpp"

wxDECLARE_EVENT(wxEVT_COMMAND_RUNTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_RUNTHREAD_UPDATE, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_RUNTHREAD_IO, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_RUNTHREAD_NOIO, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_RUNTHREAD_OUTPUT, wxThreadEvent);

enum
{
    RUNMODE_RUN = 0,
    RUNMODE_RUNFOR = 1,
    RUNMODE_STEP = 2,
    RUNMODE_BACK = 3,
    RUNMODE_NEXTLINE = 4,
    RUNMODE_PREVLINE = 5,
    RUNMODE_FINISH = 6,
    RUNMODE_REWIND = 7,
};

class LC3RunThread : public wxThread
{
	public:
        LC3RunThread(ComplxFrame* complx, int mode = RUNMODE_RUN, int run_for = 1) : frame(complx), run_mode(mode), runtime(run_for) {}
        virtual ~LC3RunThread();
		virtual void* Entry();
	private:
        ComplxFrame* frame;
        int run_mode;
        int runtime;

};

#endif
