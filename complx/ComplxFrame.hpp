#ifndef COMPLX_FRAME_HPP
#define COMPLX_FRAME_HPP

#include <lc3_all.hpp>

#include <vector>

#include "MemoryView.hpp"
#include "LoadingOptions.hpp"
#include "data/MemoryViewDataModel.hpp"
#include "data/RegisterProperty.hpp"
#include "data/ProcessStatusRegisterProperty.hpp"
#include "gen/ComplxFrameDecl.h"

class ComplxFrame : public ComplxFrameDecl
{
public:
    ComplxFrame();
    ~ComplxFrame();

    // File Menu Event Handlers
    void OnLoad(wxCommandEvent& event) override;
    void OnReload(wxCommandEvent& event) override;

    void OnExit(wxCommandEvent& event) override;

    // Control Menu Event Handlers
    void OnStep(wxCommandEvent& event) override;
    void OnBack(wxCommandEvent& event) override;

    // State Event Handling
    void OnStateChange(wxPropertyGridEvent& event) override;

private:
    // Initialization
    void InitializeLC3State();
    void InitializeMemoryView();
    void InitializeStatePropGrid();

    /** Do the work of assembling a file. */
    bool DoLoadFile(const LoadingOptions& options);
    /** Updates all objects referring to the now stale lc3_state object */
    void PostLoadFile();
    /** Called when the display needs to be updated after executing instructions */
    void PostExecute();

    std::unique_ptr<lc3_state> state;

    /** Options used when reloading assembly files */
    LoadingOptions reload_options;

    /** Backing data for Memory VIew */
    wxObjectDataPtr<MemoryViewDataModel> memory_view_model;

    /** State Property Grid Properties */
    RegisterProperty* pc_property;
    ProcessStatusRegisterProperty* cc_property;
    std::array<RegisterProperty*, 8> register_properties;
};


#endif
