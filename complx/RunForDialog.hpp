#ifndef RUNFOR_DIALOG_HPP
#define RUNFOR_DIALOG_HPP

#include <wx/window.h>
#include "RunForDialogDecl.h"
#include "lc3.hpp"

extern lc3_state state;

class RunForDialog : public RunForDialogDecl
{
	public:
		RunForDialog(wxWindow* parent, int value);
		~RunForDialog();
		int GetValue() const;
};


#endif
