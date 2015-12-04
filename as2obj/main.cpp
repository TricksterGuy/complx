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
        printf("Usage: as2obj file.asm [-all_errors] [-disable_plugins] [outfile] \n");
        abort();
    }

    try
    {
        const char* filename = argv[1];
        bool nonfatal_errors = false;
        bool disable_plugins = false;
        const char* outfile = "";
        /// @todo write this better
        if (argc == 3)
        {
            if (std::string(argv[2]) == "-all_errors")
                nonfatal_errors = true;
            else if (std::string(argv[2]) == "-disable_plugins")
                disable_plugins = true;
            else if (argv[2][0] == '-')
                goto usage;
            else
                outfile = argv[2];
        }
        else if (argc == 4)
        {
            nonfatal_errors = std::string(argv[2]) == "-all_errors" || std::string(argv[3]) == "-all_errors";
            disable_plugins = std::string(argv[2]) == "-disable_plugins" || std::string(argv[3]) == "-disable_plugins";
            outfile = argv[2][0] == '-' ? argv[3] : argv[2];
            if (outfile[0] == '-') goto usage;
        }
        else if (argc == 5)
        {
            nonfatal_errors = std::string(argv[2]) == "-all_errors" || std::string(argv[3]) == "-all_errors" || std::string(argv[4]) == "-all_errors";
            disable_plugins = std::string(argv[2]) == "-disable_plugins" || std::string(argv[3]) == "-disable_plugins" || std::string(argv[4]) == "-disable_plugins";
            outfile = argv[4][0] == '-' ? (argv[3][0] == '-' ? argv[2] : argv[3]) : argv[4];
            if (outfile[0] == '-') goto usage;
        }
        LC3AssembleOptions options;
        options.multiple_errors = nonfatal_errors;
        options.warnings_as_errors = false;
        options.process_debug_comments = true;
        options.enable_warnings = true;
        options.disable_plugins = disable_plugins;
        lc3_assemble(filename, outfile, options);
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
