#ifndef ADVANCED_LOAD_DIALOG_HPP
#define ADVANCED_LOAD_DIALOG_HPP

#include "AdvancedLoadDialogDecl.h"
#include "LoadingOptions.hpp"

class AdvancedLoadDialog : public AdvancedLoadDialogDecl
{
public:
    AdvancedLoadDialog(wxWindow* parent, const LoadingOptions& opts);
    ~AdvancedLoadDialog() {}
    LoadingOptions GetOptions();
	void OnRegChoice(wxCommandEvent& event) override;
    void OnMemChoice(wxCommandEvent& event) override;
};

#endif
