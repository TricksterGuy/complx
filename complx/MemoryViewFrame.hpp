#ifndef MEMORY_VIEW_FRAME_HPP
#define MEMORY_VIEW_FRAME_HPP

#include "MemoryViewFrameDecl.h"
#include "MemoryView.hpp"

class MemoryViewFrame : public MemoryViewFrameDecl
{
	public:
		MemoryViewFrame(wxWindow* parent, MemoryView* view);
		~MemoryViewFrame();
        void InitGridSizes(void);
        void OnGoto(wxCommandEvent& event);
	protected:

	private:

};



#endif
