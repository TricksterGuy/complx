#ifndef SETUP_TEST_DIALOG_HPP
#define SETUP_TEST_DIALOG_HPP

#include "SetupTestDialogDecl.h"
#include "lc3_test.hpp"

class SetupTestDialog : public SetupTestDialogDecl
{
	public:
		SetupTestDialog(const std::string& test_suite_path, wxWindow* parent);
		~SetupTestDialog() {}
		bool GetSelectedTest(lc3_test& test);
		bool IsValid() const {return valid;}
	protected:
		void OnChooseTest(wxCommandEvent& event) override;
		void OnReloadXml(wxCommandEvent& event) override;
    private:
        void PopulateTests();
        void PopulateConditions(const lc3_test& test);
        std::string filename;
        lc3_test_suite suite;
        bool valid;
};

#endif
