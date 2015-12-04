#ifndef RUNFOR_DIALOG_HPP
#define RUNFOR_DIALOG_HPP

#include "RunForDialogDecl.h"
#include <wx/window.h>

class RunForDialog : public RunForDialogDecl
{
	public:
		RunForDialog(wxWindow* parent, int value);
		int GetValue() const;
};


#endif
