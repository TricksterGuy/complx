#ifndef ADDRESS_DIALOG_HPP
#define ADDRESS_DIALOG_HPP

#include "AddressDialogDecl.h"
#include <wx/window.h>

class AddressDialog : public AddressDialogDecl
{
	public:
		AddressDialog(wxWindow* parent, unsigned short addr, bool change_addr = true);
		int GetAddress(int& error) const;
		int GetValue(int& error) const;
};


#endif
