#include "SetupTestDialog.hpp"
#include <XmlTestParser.hpp>

bool TryLoadTests(lc3_test_suite& suite, const std::string& path)
{
    try
    {
        if (!XmlTestParser().LoadTestSuite(suite, path))
        {
            wxMessageBox(_("ERROR! Xml file not found or parse errors found"), _("Error"));
            return true;
        }
    }
    catch (XmlTestParserException x)
    {
        wxMessageBox(wxString::Format("ERROR %s\n", x.what().c_str()), _("Error"));
    }

    return false;
}

SetupTestDialog::SetupTestDialog(const std::string& test_suite_path, wxWindow* parent) : SetupTestDialogDecl(parent), filename(test_suite_path)
{
    PopulateTests();
}

bool SetupTestDialog::GetSelectedTest(lc3_test& test)
{
    if (tests->GetSelection() == wxNOT_FOUND)
        return false;

    test = *reinterpret_cast<lc3_test*>(tests->GetClientData(tests->GetSelection()));
    return true;
}

void SetupTestDialog::OnChooseTest(wxCommandEvent& event)
{
    void* data = event.GetClientData();
    if (data == nullptr)
        return;
    lc3_test* test = reinterpret_cast<lc3_test*>(data);
    PopulateConditions(*test);
}

void SetupTestDialog::OnReloadXml(wxCommandEvent& event)
{
    PopulateTests();
}

void SetupTestDialog::PopulateTests()
{
    if (TryLoadTests(suite, filename))
        return;

    tests->SetSelection(wxNOT_FOUND);
    tests->Clear();
    for (auto& test : suite.tests)
        tests->Append(test.name, reinterpret_cast<void*>(&test));
}

void SetupTestDialog::PopulateConditions(const lc3_test& test)
{
    preconditions->Clear();
    postconditions->Clear();

    for (const auto& input : test.input)
        preconditions->Append(lc3_test_input_string(input));
    for (const auto& output : test.output)
        postconditions->Append(lc3_test_output_string(output));

}
