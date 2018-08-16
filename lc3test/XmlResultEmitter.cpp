//
// Created by Cem Gokmen on 8/12/18.
//

#include "XmlResultEmitter.h"
#include "tinyformat.h"

XmlResultEmitter::XmlResultEmitter() {
    doc = new tinyxml2::XMLDocument();
    root = doc->NewElement("testsuites");
    doc->InsertEndChild(root);

    runs = 0;
}

XmlResultEmitter::~XmlResultEmitter() {
    delete doc;
}

void XmlResultEmitter::add_suite(lc3_test_suite &suite, const std::string &filename) {
    tinyxml2::XMLElement* suite_node = doc->NewElement("testsuite");
    suite_node->SetAttribute("name", tfm::format("run-%d", ++runs).c_str());
    suite_node->SetAttribute("tests", (int) suite.tests.size());

    // Run through the tests now
    tinyxml2::XMLElement* test_node;
    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_test& test = suite.tests[i];
        test_node = add_test(test);
        suite_node->InsertEndChild(test_node);
    }

    root->InsertEndChild(suite_node);
}

tinyxml2::XMLElement* XmlResultEmitter::add_test(lc3_test& test) {
    tinyxml2::XMLElement* test_node = doc->NewElement("testcase");

    test_node->SetAttribute("name", test.name.c_str());

    // Look at "did not finish" and "terminated abnormally" here
    if (!test.has_halted || !test.has_halted_normally) {
        tinyxml2::XMLElement* halt_node = doc->NewElement("failure");
        halt_node->SetText(!test.has_halted ? "Did not finish!" : "Terminated abnormally!"); // mutually exclusive

        test_node->InsertEndChild(halt_node);
    }

    // Go through the actual outputs
    std::stringstream oss;

    for (unsigned int j = 0; j < test.output.size(); j++)
    {
        lc3_test_output& output = test.output[j];
        std::string type = lc3_test_output_string(output);

        std::string result = tfm::format("%d (%s) - %s\n  expected: %s\n  actual: %s\n",
                                         j + 1, (output.passed ? 'P' : 'F'), type, output.expected, output.actual);

        if (!output.extra_output.empty())
            result += output.extra_output;

        // Passed outputs' information go into the stdout stream. Rest go to failure nodes.
        if (output.passed)
        {
            oss << result << "\n";
        }
        else
        {
            tinyxml2::XMLElement* output_node = doc->NewElement("failure");
            output_node->SetText(result.c_str());

            test_node->InsertEndChild(output_node);
        }
    }

    // Put the outputs in the output stream node.
    tinyxml2::XMLElement* stdout_node = doc->NewElement("system-out");
    tinyxml2::XMLText* stdout_text = doc->NewText(oss.str().c_str());
    stdout_text->SetCData(true);

    stdout_node->InsertFirstChild(stdout_text);
    test_node->InsertFirstChild(stdout_node);

    // Note that we put this here before the warnings, so that actual failures and warning-failures can be told apart.
    if (!test.passed && test_node->FirstChildElement("failure") == nullptr) {
        // This means the test was marked a failure but we did not get the failure from the outputs, so we put in
        // a dummy failure node.
        tinyxml2::XMLElement* fail_node = doc->NewElement("failure");
        fail_node->SetText("Tests failed.");

        test_node->InsertEndChild(fail_node);
    }

    // Finally, go through warnings (add as a single failure)
    if (test.warnings) {
        tinyxml2::XMLElement* warn_node = doc->NewElement("failure");
        warn_node->SetText(test.warning.c_str());

        test_node->InsertEndChild(warn_node);
    }

    return test_node;
}

void XmlResultEmitter::emit_results(std::ostream &os) {
    tinyxml2::XMLPrinter printer;
    doc->Print(&printer);
    os << printer.CStr();
}