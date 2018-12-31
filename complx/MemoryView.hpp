#ifndef MEMORY_VIEW_HPP
#define MEMORY_VIEW_HPP

#include <functional>
#include <optional>

#include <lc3_all.hpp>

#include <wx/dataview.h>

class MemoryView : public wxDataViewCtrl
{
public:
    MemoryView(wxWindow *parent, wxWindowID id, const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, long style=0, const wxValidator &validator=wxDefaultValidator, const wxString &name=wxDataViewCtrlNameStr);
    ~MemoryView() {}

private:
    std::optional<std::reference_wrapper<lc3_state>> state;
};


#endif

