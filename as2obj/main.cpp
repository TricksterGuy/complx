#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <lc3_all.hpp>
#include <string>

int main(int argc, char** argv)
{
    // Typical
    if (argc < 2)
    {
usage:
        printf("Usage: as2obj [-all_errors] [-disable_plugins] [-hex|-bin] [asmfile] [output_file_prefix]\n");
        abort();
    }

    std::string filename;
    std::string outfile_prefix;
    LC3AssembleOptions options;
    std::vector<std::string> params;

    for (int i = 1; i < argc; i++)
    {
        const std::string& arg = argv[i];
        if (arg == "-all_errors")
            options.multiple_errors = true;
        else if (arg == "-disable_plugins")
            options.disable_plugins = true;
        else if (arg == "-hex")
            options.output_mode = LC3AssembleOptions::HEXADECIMAL_FILE;
        else if (arg == "-bin")
            options.output_mode = LC3AssembleOptions::BINARY_FILE;
        else if (arg[0] == '-') {
            printf("Invalid option %s given.\n", argv[i]);
            goto usage;
        }
        else
            params.push_back(argv[i]);
    }

    if (params.empty())
    {
        printf("No asm file given.\n");
        goto usage;
    }
    else if (params.size() > 2)
    {
        printf("Too many parameters given.\n");
        goto usage;
    }

    filename = params[0];
    if (params.size() == 2)
        outfile_prefix = params[1];

    try
    {
        lc3_assemble(filename, outfile_prefix, options);
    }
    catch (std::vector<LC3AssembleException> e)
    {
        for (unsigned int i = 0; i < e.size(); i++)
            std::cout << e[i].what() << std::endl;
        abort();
    }
    catch (LC3AssembleException e)
    {
        std::cout << "what: " << e.what() << std::endl;
        abort();
    }


    return EXIT_SUCCESS;
}
