#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP

#include <lc3_all.hpp>

#include <vector>

#include "gen/ComplxFrameDecl.h"
#include "data/MemoryViewDataModel.hpp"
#include "data/RegisterProperty.hpp"
#include "data/ProcessStatusRegisterProperty.hpp"
#include "MemoryView.hpp"

class ComplxFrame : public ComplxFrameDecl
{
public:
    ComplxFrame();
    ~ComplxFrame();

    // State Event Handling
    void OnStateChange(wxPropertyGridEvent& event) override;

private:
    wxObjectDataPtr<MemoryViewDataModel> memory_view_model;
    RegisterProperty* pc_property;
    ProcessStatusRegisterProperty* cc_property;
    std::list<RegisterProperty*> register_properties;
    lc3_state state;
};


#endif
