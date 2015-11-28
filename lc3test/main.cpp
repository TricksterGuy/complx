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
        printf("Usage: lc3test testfile.xml asmfile.asm -disable_plugins -random_seed=int\n");
        printf("by default -disable_plugins is false\n");
        return EXIT_FAILURE;
    }

    lc3_test_suite suite;
    std::string xmlfile = argv[1];
    std::string asmfile = argv[2];
    int random_seed = -1;

    for (int i = 3; i < argc; i++)
    {
        const std::string& arg = argv[i];
        if (arg.compare("-random_seed"))
            random_seed = atoi(arg.substr(arg.find("=") + 1).c_str());
    }

    try
    {
        if (!XmlTestParser().LoadTestSuite(suite, xmlfile))
        {
            printf("Xml file not found or parse errors found\n");
            return EXIT_FAILURE;
        }
    }
    catch (XmlTestParserException x)
    {
        printf("%s\n", x.what().c_str());
        return EXIT_FAILURE;
    }

    try
    {
        lc3_run_test_suite(suite, asmfile, random_seed);
    }
    catch (const char* x)
    {
        printf("%s\n", x);
        return EXIT_FAILURE;
    }
    catch (std::string x)
    {
        printf("%s\n", x.c_str());
        return EXIT_FAILURE;
    }

    std::stringstream oss;
    lc3_write_test_report(oss, suite, asmfile);
    std::cout << oss.str();

    return EXIT_SUCCESS;
}
