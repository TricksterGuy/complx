#include <lc3_all.hpp>

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

#include <XmlTestParser.hpp>
#include <TextResultEmitter.h>
#include <XmlResultEmitter.h>

int main(int argc, char** argv)
{
    // Typical
    if (argc < 3)
    {
        fprintf(stderr, "Usage: lc3test testfile.xml asmfile.asm -random_seed=int -runs=int -xml\n");
        return EXIT_FAILURE;
    }

    lc3_test_suite suite;
    std::string xmlfile = argv[1];
    std::string asmfile = argv[2];
    int random_seed = -1;
    unsigned int runs = 1;
    bool xml_output = false;

    for (int i = 3; i < argc; i++)
    {
        const std::string& arg = argv[i];
        if (arg.find("-random_seed=") != std::string::npos)
            random_seed = atoi(arg.substr(arg.find("=") + 1).c_str());
        else if (arg.find("-runs=") != std::string::npos)
            runs = (unsigned int) atoi(arg.substr(arg.find("=") + 1).c_str());
        else if (arg.find("-xml") != std::string::npos)
            xml_output = true;
        else
        {
            fprintf(stderr, "Invalid param %s passed in!", arg.c_str());
            return EXIT_FAILURE;
        }
    }

    try
    {
        if (!XmlTestParser().LoadTestSuite(suite, xmlfile))
        {
            fprintf(stderr, "Xml file not found or parse errors found\n");
            return EXIT_FAILURE;
        }
    }
    catch (XmlTestParserException x)
    {
        fprintf(stderr, "%s\n", x.what().c_str());
        return EXIT_FAILURE;
    }

    ResultEmitter *emitter;

    if (xml_output) {
        emitter = new XmlResultEmitter();
    } else {
        emitter = new TextResultEmitter();
    }

    lc3_test_suite test_suite;
    for (unsigned int i = 0; i < runs; i++)
    {
        test_suite = suite;
        try
        {
            lc3_run_test_suite(test_suite, asmfile, random_seed, i);
        }
        catch (const char* x)
        {
            fprintf(stderr, "%s\n", x);
            return EXIT_FAILURE;
        }
        catch (std::string x)
        {
            fprintf(stderr, "%s\n", x.c_str());
            return EXIT_FAILURE;
        }

        emitter->add_suite(test_suite, asmfile);

        if (random_seed != -1) random_seed++;
    }

    emitter->emit_results(std::cout);

    delete emitter;
    return EXIT_SUCCESS;
}
