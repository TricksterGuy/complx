#ifndef ADDRESS_DIALOG_HPP
#define ADDRESS_DIALOG_HPP

#include <wx/window.h>
#include "AddressDialogDecl.h"
#include "lc3.hpp"

extern lc3_state state;

class AddressDialog : public AddressDialogDecl
{
	public:
		AddressDialog(wxWindow* parent, unsigned short addr, bool change_addr = true);
		~AddressDialog();
		int GetAddress(int& error) const;
		int GetValue(int& error) const;
};


#endif
