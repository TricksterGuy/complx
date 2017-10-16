#include "AddressDialog.hpp"
#include <lc3_all.hpp>
#include <wx/string.h>

extern lc3_state state;

/** AddressDialog
  *
  * Constructor
  */
AddressDialog::AddressDialog(wxWindow* parent, unsigned short addr, bool change_addr) : AddressDialogDecl(parent)
{
    std::string sym = lc3_sym_rev_lookup(state, addr);
    address = sym.empty() ? wxString::Format(_("x%04X"), addr) : wxString::FromUTF8(sym.c_str());

    value = wxString::Format(_("%d"), state.mem[addr]);

    if (!change_addr)
        address_text->Disable();
}

/** GetAddress
  *
  * Gets the address or returns -1 on error
  */
int AddressDialog::GetAddress(int& error) const
{
    int ret = -1;
    error = lc3_calculate(state, std::string(address.mb_str()), ret);
    return ret;
}

/** GetValue
  *
  * Gets the value or returns -1 on error
  */
int AddressDialog::GetValue(int& error) const
{
    int ret = -1;
    error = lc3_calculate(state, std::string(value.mb_str()), ret);
    return ret;
}
