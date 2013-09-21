#include "RunTestDialog.hpp"
#include <sstream>
#include <wx/msgdlg.h>

extern wxFileName currentFile;

/** @brief TestTreeItem
  *
  * @todo: document this function
  */
TestTreeItem::TestTreeItem(void* _data) : data(_data) {}

RunTestDialog::RunTestDialog(wxWindow* parent, const std::string& _filename, lc3_test_suite& _test) :
 RunTestDialogDecl(parent), suite(_test), filename(_filename)
{
    tspanel = new TestSuiteInfoPanel(right, suite);
	testInfoSizer->Add(tspanel, 1, wxEXPAND|wxALL, 4);
    tcpanel = new TestCaseInfoPanel(right);
	testInfoSizer->Add(tcpanel, 1, wxEXPAND|wxALL, 4);
    cipanel = new CheckInfoPanel(right);
	testInfoSizer->Add(cipanel, 1, wxEXPAND|wxALL, 4);
    UpdateTests();
    tspanel->Show();
    infoSplitter->Layout();
	testInfoSizer->Fit(tspanel);
	infoSplitter->SetSashPosition(infoSplitter->GetSashPosition() + 1);

	wxCommandEvent dummy;
	OnRunTests(dummy);
}

RunTestDialog::~RunTestDialog()
{
}

void RunTestDialog::OnSelectTestItem(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    wxTreeItemId root = testTree->GetRootItem();

    tspanel->Hide();
    tcpanel->Hide();
    cipanel->Hide();

    wxWindow* interest;

    if (id == root)
    {
        interest = tspanel;
        tspanel->Update();
    }
    else if (testTree->GetItemParent(id) == root)
    {
        TestTreeItem* treeItem = reinterpret_cast<TestTreeItem*>(testTree->GetItemData(id));
        lc3_test* test = reinterpret_cast<lc3_test*>(treeItem->data);
        tcpanel->Update(*test);
        interest = tcpanel;
    }
    else
    {
        TestTreeItem* treeItem = reinterpret_cast<TestTreeItem*>(testTree->GetItemData(id));
        lc3_test_output* output = reinterpret_cast<lc3_test_output*>(treeItem->data);
        cipanel->Update(*output);
        interest = cipanel;
    }

    interest->Show();
    interest->Layout();
    right->Layout();
    testInfoSizer->Fit(interest);
    testInfoSizer->Layout();

	//testInfoSizer->Fit( testInfoScroller );
}

void RunTestDialog::OnRunTest(wxCommandEvent& event)
{
    wxTreeItemId id = testTree->GetSelection();
    wxTreeItemId root = testTree->GetRootItem();

    if (!id.IsOk()) return;
    if (id == root) return;

    if (testTree->GetItemParent(id) != testTree->GetRootItem())
    {
        id = testTree->GetItemParent(id);
    }

    if (testTree->GetItemParent(id) != testTree->GetRootItem())
    {
        // Shouldn't happen
        return;
    }

    testTree->Unselect();
    testTree->ToggleItemSelection(id);

    TestTreeItem* treeItem = reinterpret_cast<TestTreeItem*>(testTree->GetItemData(id));
    lc3_test* test = reinterpret_cast<lc3_test*>(treeItem->data);

    try
    {
        lc3_run_test_case(*test, currentFile.GetFullPath().ToStdString(), false);
    }
    catch (const char* x)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", x), "Run Tests Failed");
        return;
    }
    catch (std::string x)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", x), "Run Tests Failed");
        return;
    }

    tspanel->Update();
    tcpanel->Update();
    cipanel->Update();
}

void RunTestDialog::OnRunTests(wxCommandEvent& event)
{
    try
    {
        lc3_run_test_suite(suite, currentFile.GetFullPath().ToStdString(), false);
    }
    catch (const char* x)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", x), "Run Tests Failed");
        return;
    }
    catch (std::string x)
    {
        wxMessageBox(wxString::Format("BAD STUDENT! %s", x), "Run Tests Failed");
        return;
    }
    tspanel->Update();
    tcpanel->Update();
    cipanel->Update();
}

void RunTestDialog::UpdateTests()
{
    testTree->DeleteAllItems();
    wxTreeItemId root = testTree->AddRoot(filename);
    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_test& test = suite.tests[i];
        wxTreeItemId testId = testTree->AppendItem(root, test.name, -1, -1, new TestTreeItem(&test));
        //printf("Test case %d - %s - %s %d/%d %s\n", i + 1, test.name.c_str(), test.passed ? "Passed" : "Failed", test.points, test.max_points, test.has_halted ? "" : "(Did not finish!)");
        for (unsigned int j = 0; j < test.output.size(); j++)
        {
            lc3_test_output& output = test.output[j];
            testTree->AppendItem(testId, lc3_test_output_string(output), -1, -1, new TestTreeItem(&output));
        }
    }
}

void RunTestDialog::OnReport(wxCommandEvent& event)
{
    std::stringstream oss;

    lc3_write_test_report(oss, suite, currentFile.GetFullName().ToStdString());

    TestReportDialog* dialog = new TestReportDialog(this, currentFile.GetFullName(), oss.str());

    if (dialog->ShowModal() == wxID_OK)
    {
        delete dialog;
        return;
    }

    delete dialog;
}

void RunTestDialog::OnTestReport(wxCommandEvent& event)
{
    wxTreeItemId id = testTree->GetSelection();
    wxTreeItemId root = testTree->GetRootItem();

    if (!id.IsOk()) return;
    if (id == root) return;

    if (testTree->GetItemParent(id) != testTree->GetRootItem())
    {
        id = testTree->GetItemParent(id);
    }

    if (testTree->GetItemParent(id) != testTree->GetRootItem())
    {
        // Shouldn't happen
        return;
    }

    testTree->Unselect();
    testTree->ToggleItemSelection(id);

    TestTreeItem* treeItem = reinterpret_cast<TestTreeItem*>(testTree->GetItemData(id));
    lc3_test* test = reinterpret_cast<lc3_test*>(treeItem->data);

    std::stringstream oss;

    int tweedle_dee = 0;
    int tweedle_dum = 0;
    lc3_write_test_report(oss, *test, tweedle_dee, tweedle_dum);

    TestReportDialog* dialog = new TestReportDialog(this, test->name, oss.str());

    if (dialog->ShowModal() == wxID_OK)
    {
        delete dialog;
        return;
    }

    delete dialog;
}

/** @brief TestSuiteInfoPanel
  *
  * @todo: document this function
  */
TestSuiteInfoPanel::TestSuiteInfoPanel(wxWindow* parent, lc3_test_suite& _suite) : TestSuiteInfoPanelDecl(parent), suite(_suite)
{
    Update();
    Hide();
}


/** @brief ~TestSuiteInfoPanel
  *
  * @todo: document this function
  */
 TestSuiteInfoPanel::~TestSuiteInfoPanel()
{

}

/** @brief Update
  *
  * @todo: document this function
  */
void TestSuiteInfoPanel::Update()
{
    tests->Clear();
    passed->SetLabel(wxString::Format("%s", suite.passed ? _("Yes") : _("No")));
    pointsText->Hide();
    points->Hide();

    int pass_count = 0;
    int total_tests = 0;
    int minipass_count = 0;
    int total_minitests = 0;

    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_test& test = suite.tests[i];
        if (test.passed) pass_count++;

        for (unsigned int j = 0; j < test.output.size(); j++)
        {
            const lc3_test_output& output = test.output[j];
            if (output.passed) minipass_count++;
            total_minitests++;
        }

        tests->Append(wxString::Format("(%s) %s", test.passed ? "Passed" : "Failed", test.name), &test);
        total_tests++;
    }

    testsPassed->SetLabel(wxString::Format("%d / %d", pass_count, total_tests));
    checksPassed->SetLabel(wxString::Format("%d / %d", minipass_count, total_minitests));
    if (suite.max_points)
    {
        pointsText->Show();
        points->Show();
        points->SetLabel(wxString::Format("%d / %d", suite.points, suite.max_points));
    }

    Layout();
}

/** @brief TestCaseInfoPanel
  *
  * @todo: document this function
  */
 TestCaseInfoPanel::TestCaseInfoPanel(wxWindow* parent) : TestCaseInfoPanelDecl(parent), testcase(NULL)
{
    Hide();
}

/** @brief ~TestCaseInfoPanel
  *
  * @todo: document this function
  */
 TestCaseInfoPanel::~TestCaseInfoPanel()
{

}

/** @brief Update
  *
  * @todo: document this function
  */
void TestCaseInfoPanel::Update()
{
    if (testcase == NULL) return;

    Update(*testcase);
}

/** @brief Update
  *
  * @todo: document this function
  */
void TestCaseInfoPanel::Update(lc3_test& testcase)
{
    this->testcase = &testcase;

    testName->SetLabel(testcase.name);
    int pass_count = 0;

    testOutputs->Clear();
    testInputs->Clear();
    testInfo->Clear();
    halted->Hide();
    warning->Hide();
    warningButton->Hide();
    pointsText->Hide();
    points->Hide();

    // Set up output list and calculate stats
    for (unsigned int j = 0; j < testcase.output.size(); j++)
    {
        lc3_test_output& output = testcase.output[j];
        if (output.passed) pass_count++;

        wxString address = (output.type != TEST_IO && output.type != TEST_PC) ? output.address : "";
        testOutputs->Append(wxString::Format("(%s) %s", output.passed ? "Passed" : "Failed", lc3_test_output_string(output)));
    }

    // Set up input list
    for (unsigned int j = 0; j < testcase.input.size(); j++)
    {
        lc3_test_input& input = testcase.input[j];
        testInputs->Append(lc3_test_input_string(input));
    }

    // Set up info list
    testInfo->Append(wxString::Format("True Traps Enabled: %s", testcase.true_traps ? "Yes" : "No"));
    testInfo->Append(wxString::Format("Interrupts Enabled: %s", testcase.interrupt_enabled ? "Yes" : "No"));
    testInfo->Append(wxString::Format("Disable Plugins: %s", testcase.disable_plugins ? "Yes" : "No"));
    testInfo->Append(wxString::Format("Randomize Memory: %s", testcase.randomize ? "Yes" : "No"));
    if (testcase.has_max_executions)
        testInfo->Append(wxString::Format("Maximum Instructions: %lu", testcase.max_executions));

    // Set up other
    executions->SetLabel(wxString::Format("%lu Instructions", testcase.executions));
    if (!testcase.has_halted) halted->Show();
    if (testcase.warnings > 0)
    {
        warning->Show();
        warningButton->Show();
    }

    checksPassed->SetLabel(wxString::Format("%d / %d", pass_count, testcase.output.size()));

    if (testcase.max_points)
    {
        pointsText->Show();
        points->Show();
        points->SetLabel(wxString::Format("%d / %d", testcase.points, testcase.max_points));
    }

    Layout();
}

void TestCaseInfoPanel::OnWarnings(wxCommandEvent& event)
{
    if (testcase == NULL) return;

    wxMessageBox(testcase->warning, wxString::Format("Test Finished with %lu warnings", testcase->warnings));
}

/** @brief CheckInfoPanel
  *
  * @todo: document this function
  */
 CheckInfoPanel::CheckInfoPanel(wxWindow* parent) : CheckInfoPanelDecl(parent), output(NULL)
{
    Hide();
}

/** @brief ~CheckInfoPanel
  *
  * @todo: document this function
  */
 CheckInfoPanel::~CheckInfoPanel()
{

}

/** @brief Update
  *
  * @todo: document this function
  */
void CheckInfoPanel::Update()
{
    if (output == NULL) return;

    Update(*output);
}

/** @brief Update
  *
  * @todo: document this function
  */
void CheckInfoPanel::Update(lc3_test_output& output)
{
    this->output = &output;
    pointsText->Hide();
    points->Hide();
    passed->SetLabel(output.passed ? "Yes" : "No");
    condition->SetLabel(lc3_test_output_string(output));
    expected->SetLabel(output.expected);
    actual->SetLabel(output.actual);

    if (output.points)
    {
        pointsText->Show();
        points->Show();
        points->SetLabel(wxString::Format("%d / %d", output.passed ? output.points : 0, output.points));
    }

    Layout();
}

/** @brief TestReportDialog
  *
  * @todo: document this function
  */
TestReportDialog::TestReportDialog(wxWindow* parent, wxString name, wxString report) : TestReportDialogDecl(parent)
{
    this->name->SetLabel(name);
    this->report->SetValue(report);
}

/** @brief ~TestReportDialog
  *
  * @todo: document this function
  */
TestReportDialog::~TestReportDialog()
{

}
