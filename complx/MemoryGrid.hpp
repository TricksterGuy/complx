#ifndef MEMORY_GRID_HPP
#define MEMORY_GRID_HPP

#include "MemoryView.hpp"
#include <wx/event.h>
#include <wx/gdicmn.h>
#include <wx/window.h>
#include <wx/grid.h>
#include <wx/tipwin.h>
#include <wx/timer.h>

enum
{
    MemoryMenuBreakpoint = 2000,
    MemoryMenuTemppoint,
    MemoryMenuAdvancedpoint,
    MemoryMenuWatchpoint,
    MemoryMenuBlackbox,
    MemoryMenuPCHere,
    MemoryToolTipTimer
};

class MemoryGrid : public wxGrid
{
	public:
		MemoryGrid(wxWindow *parent, wxWindowID id, const wxPoint &pos, const wxSize &size, long style);
		~MemoryGrid();
        void InitGridSizes(bool exact_column_sizing = false);
        void SelectLocation(unsigned short location);
        void OnContextMenu(wxGridEvent& event);
        void OnBreakpoint(wxCommandEvent& event);
        void OnTemppoint(wxCommandEvent& event);
        void OnWatchpoint(wxCommandEvent& event);
        void OnBlackbox(wxCommandEvent& event);
        void OnAdvancedpoint(wxCommandEvent& event);
        void OnPCHere(wxCommandEvent& event);
        void SetView(MemoryView* view, bool exact_column_sizes = false);
        bool AcceptsFocusFromKeyboard() const;

        void SetDisassembleLevel(int level);
        void SetHighlight(bool highlight);
        void SetUnsignedMode(bool unsigned_mode);
        void SetFlippedMode(bool flipped_mode);
	private:
        int last_address;
        wxTimer timer;
        bool highlight;
        int GetSelectedAddress() const;
        void OnGridChanged(wxGridEvent& event);
        void OnActivate(wxActivateEvent& event);
        void OnMotion(wxMouseEvent& event);
        void OnShowToolTip(wxTimerEvent& event);
        wxTipWindow* tipWindow;
};

#endif
