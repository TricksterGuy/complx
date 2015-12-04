#include "CallStackDialog.hpp"
#include <lc3_all.hpp>

extern lc3_state state;

/** CallStackDialog
  *
  *
  */
CallStackDialog::CallStackDialog(wxWindow* parent) : CallStackDialogDecl(parent)
{
    SetupList();
}

/** GetFrame
  *
  *
  */
int CallStackDialog::GetFrame() const
{
    return frame.IsEmpty() ? -1 : frame[0];
}

/** GetRewindId
  *
  *
  */
int CallStackDialog::GetRewindId() const
{
    return rewindButton->GetId();
}

/** GetViewStackId
  *
  *
  */
int CallStackDialog::GetViewStackId() const
{
    return viewStackButton->GetId();
}

/** OnRewindToCall
  *
  *
  */
void CallStackDialog::OnRewindToCall(wxCommandEvent& event)
{
    if (Validate() && TransferDataFromWindow())
        EndModal(event.GetId());
}

/** OnViewStackFrame
  *
  *
  */
void CallStackDialog::OnViewStackFrame(wxCommandEvent& event)
{
    if (Validate() && TransferDataFromWindow())
        EndModal(event.GetId());
}

/** SetupList
  *
  *
  */
void CallStackDialog::SetupList()
{
    callList->Clear();
    for (unsigned int i = 0; i < state.call_stack.size(); i++)
    {
        lc3_subroutine_call call = state.call_stack[i];
        if (state.subroutines.find(call.address) == state.subroutines.end())
        {
            callList->Append(wxString::Format("%s%s", call.is_trap ? "TRAP " : "", lc3_sym_rev_lookup(state, call.address)));
        }
        else
        {
            unsigned short r6 = call.r6;
            std::stringstream params_str;
            // YES SWEET METADATA
            lc3_subroutine_info& info = state.subroutines[call.address];
            for (int j = 0; j < info.num_params; j++)
            {
                if (!info.params.empty())
                    params_str << info.params[j] << " = ";
                params_str << state.mem[r6 + j];
                if (j != info.num_params - 1)
                    params_str << ", ";
            }
            std::string name = info.name.empty() ? lc3_sym_rev_lookup(state, call.address) : info.name;
            callList->Append(wxString::Format("%s%s(%s)", call.is_trap ? "TRAP " : "", name, params_str.str()));
        }
    }
}
