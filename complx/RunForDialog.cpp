#include <wx/string.h>
#include "RunForDialog.hpp"

/** RunForDialog
  *
  * Constructor
  */
RunForDialog::RunForDialog(wxWindow* parent, int invalue) : RunForDialogDecl(parent)
{
    value = invalue;
}

/** GetValue
  *
  * Gets the value or returns -1 on error
  */
int RunForDialog::GetValue() const
{
    return value;
}
