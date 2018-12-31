#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP

#include <lc3_all.hpp>

#include <vector>

#include "gen/ComplxFrameDecl.h"
#include "data/MemoryViewDataModel.hpp"
#include "data/RegisterProperty.hpp"
#include "MemoryView.hpp"

class ComplxFrame : public ComplxFrameDecl
{
public:
    ComplxFrame();
    ~ComplxFrame();

    // State Event Handling
    void OnStateChange(wxPropertyGridEvent& event) override;

private:
    wxObjectDataPtr<MemoryViewDataModel> memoryViewModel;
    std::list<RegisterProperty*> registerProperties;
    lc3_state state;
};


#endif
