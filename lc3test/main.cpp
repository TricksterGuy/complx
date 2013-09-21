#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <lc3.hpp>
#include <lc3_test.hpp>
#include <string>
#include <lc3_parser.hpp>
#include <sstream>

#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/wx.h>

#include <XmlTestParser.hpp>

int main(int argc, char** argv)
{
    // Typical
    if (argc < 3)
    {
        printf("Usage: lc3test testfile.xml asmfile.asm -disable_plugins\n");
        printf("by default -disable_plugins is false\n");
        abort();
    }

    std::string xmlfile = argv[1];
    std::string asmfile = argv[2];

    bool disable_plugins = (argc >= 4) ? std::string(argv[3]).compare("-disable_plugins") == 0 : false;

    lc3_test_suite suite;

    /*lc3_test_input test_atom_a;
    test_atom_a.type = TEST_VALUE;
    test_atom_a.address = "A" ;
    test_atom_a.data.value.value = 157;

    lc3_test_input test_atom_b;
    test_atom_b.type = TEST_VALUE;
    test_atom_b.address = "B" ;
    test_atom_b.data.value.value = 257;

    lc3_test_output test_atom_a1;
    test_atom_a1.type = TEST_POINTER;
    test_atom_a1.address = "A1" ;
    test_atom_a1.data.pointer.value = 157+257;

    lc3_test_output test_atom_a2;
    test_atom_a2.type = TEST_POINTER;
    test_atom_a2.address = "A2" ;
    test_atom_a2.data.pointer.value = 157&257;

    lc3_test_output test_atom_a3;
    test_atom_a3.type = TEST_POINTER;
    test_atom_a3.address = "A3" ;
    test_atom_a3.data.pointer.value = -157;

    lc3_test_output test_atom_a4;
    test_atom_a4.type = TEST_POINTER;
    test_atom_a4.address = "A4" ;
    test_atom_a4.data.pointer.value = 2*157+3*257;

    lc3_test test;
    test.name = "Default";
    test.true_traps = 0;
    test.interrupt_enabled = 0;
    test.randomize = 0;
    test.has_max_executions = 0;
    test.input.push_back(test_atom_a);
    test.input.push_back(test_atom_b);
    test.output.push_back(test_atom_a1);
    test.output.push_back(test_atom_a2);
    test.output.push_back(test_atom_a3);
    test.output.push_back(test_atom_a4);

    suite.tests.push_back(test);*/

    try
    {
        if (!XmlTestParser().LoadTestSuite(suite, xmlfile))
        {
            printf("Xml file not found or parse errors found\n");
            abort();
        }
    }
    catch (const char* x)
    {
        printf("%s\n", x);
        abort();
    }


    try
    {
        lc3_run_test_suite(suite, asmfile, disable_plugins);
    }
    catch (const char* x)
    {
        printf("%s\n", x);
        abort();
    }
    catch (std::string x)
    {
        printf("%s\n", x.c_str());
        abort();
    }

    std::stringstream oss;
    lc3_write_test_report(oss, suite, asmfile);
    std::cout << oss.str();

    return EXIT_SUCCESS;
}
