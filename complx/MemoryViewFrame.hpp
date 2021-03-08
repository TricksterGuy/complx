#ifndef MEMORY_VIEW_FRAME_HPP
#define MEMORY_VIEW_FRAME_HPP

#include "MemoryViewFrameDecl.h"
#include "MemoryGrid.hpp"
#include "MemoryView.hpp"

class MemoryViewFrame : public MemoryViewFrameDecl
{
public:
    MemoryViewFrame(wxWindow* parent) : MemoryViewFrameDecl(parent) {}
    ~MemoryViewFrame() {}
    void SetView(MemoryView* view) { memoryView = view; memory->SetView(view); InitGridSizes(); }
    void InitGridSizes(void);
    void OnGoto(wxCommandEvent& event);
    void OnUpdateHideAddresses(wxCommandEvent& event);
    void OnHideAddressesCustom(wxCommandEvent& event);
    void OnDumbDisassemble(wxCommandEvent& event);
    void OnNormalDisassemble(wxCommandEvent& event);
    void OnCDisassemble(wxCommandEvent& event);
    void OnInstructionHighlight(wxCommandEvent& event);
    void OnFlipMemory(wxCommandEvent& event);
private:
    MemoryView* memoryView = nullptr;
};

#endif
