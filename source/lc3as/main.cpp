#include <lc3.hpp>

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    // Typical
    if (argc < 2)
    {
usage:
        printf("Usage: lc3as [-all_errors] [-disable_plugins] [-hex|-bin|-full] [asmfile] [output_file_prefix]\n");
        return EXIT_FAILURE;
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
        else if (arg == "-full")
            options.output_mode = LC3AssembleOptions::FULL_REPRESENTATION_FILE;
        else if (arg[0] == '-') {
            printf("Invalid option %s given.\n", argv[i]);
            goto usage;
        }
        else
            params.emplace_back(argv[i]);
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
    catch (const LC3AssembleException& e)
    {
        printf("%s\n", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
