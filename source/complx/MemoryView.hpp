#ifndef MEMORY_VIEW_HPP
#define MEMORY_VIEW_HPP

#include <functional>
#include <optional>

#include <lc3.hpp>

#include <wx/dataview.h>

enum
{
    MemoryMenuBreakpoint = 2000,
    MemoryMenuTemporary,
    MemoryMenuAdvanced,
    MemoryMenuWatchpoint,
    MemoryMenuBlackbox,
    MemoryMenuPCHere,
    MemoryMenuGoto
};

class MemoryView : public wxDataViewCtrl
{
public:
    MemoryView(wxWindow* parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxValidator& validator = wxDefaultValidator, const wxString& name = wxDataViewCtrlNameStr);
    ~MemoryView();

    /// Gets the address currently selected or -1 if nothing selected.
    int GetSelectedAddress() const;
    void ScrollTo(unsigned short address);

    // Context Menu Event Handlers
    void OnContextMenu(wxDataViewEvent& event);
    void OnTemporaryBreakpoint(wxCommandEvent& event);
    void OnBreakpoint(wxCommandEvent& event);
    void OnWatchpoint(wxCommandEvent& event);
    void OnBlackbox(wxCommandEvent& event);
    void OnAdvancedBreakpoint(wxCommandEvent& event);
    void OnSetPcHere(wxCommandEvent& event);
    void OnGotoAddress(wxCommandEvent& event);

    /** Updates the reference wrapper. Usually done after loading a new assembly file.
        All views referring to this data model must be refreshed after a call to this function.
     */
    void UpdateRef(std::reference_wrapper<lc3_state> new_value);

private:
    std::optional<std::reference_wrapper<lc3_state>> state_ref;
};

#endif

