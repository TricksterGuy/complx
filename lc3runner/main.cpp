#include <lc3.hpp>
#include <string>

lc3_state state;



int main(int argc, char** argv)
{
    // Typical
    if (argc < 2)
    {
        fprintf(stderr, "Usage: lc3runner file.asm -zeroed -input=file -count=num\n");
        fprintf(stderr, "Parameters preceded with a - are optional!\n");
        fprintf(stderr, "-input=file specifies the file containing user input\n");
        fprintf(stderr, "-zeroed zeroes out lc3 memory (default is no for randomized)\n");
        fprintf(stderr, "-count=num specifies how long to run the program (Default is -1 for infinite)\n");
        return EXIT_FAILURE;
    }

    // Read assembly file
    std::string asm_file(argv[1]);
    bool from_file = false;
    std::string input_file;
    int times = -1;
    bool randomize = true;


    for (int i = 2; i < argc; i++)
    {
        std::string param = argv[i];
        if (param.find("-input=") != std::string::npos)
        {
            from_file = true;
            input_file = param.substr(7);
        }
        else if (param.find("-count=") != std::string::npos)
        {
            times = atoi(param.substr(7).c_str());
        }
        else if (param.find("-zeroed") != std::string::npos)
        {
            randomize = false;
        }
        else if (param.find("-i=") != std::string::npos)
        {
            from_file = true;
            input_file = param.substr(3);
        }
        else if (param.find("-c=") != std::string::npos)
        {
            times = atoi(param.substr(3).c_str());
        }
        else if (param.find("-z") != std::string::npos)
        {
            randomize = false;
        }
    }

    //printf("%s %s %d\n", asm_file.c_str(), input_file.c_str(), times);
    fprintf(stderr, "[INFO] Assembling your code\n");


    lc3_init(state, false);
    if (randomize) lc3_randomize(state);

    try
    {
        lc3_assemble(state, asm_file);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "[ERROR] %s failed to assemble. %s\n", asm_file.c_str(), e.what().c_str());
        abort();
    }

    std::ifstream first_file(input_file.c_str(), std::ios::binary);
    if (!first_file.good() && from_file)
    {
        fprintf(stderr, "[ERROR] Unable to open file %s\n", input_file.c_str());
        first_file.close();
        abort();
    }
    if (from_file) state.input = &first_file;

    // Run their code.
    lc3_run(state, times);

    if (!state.halted) fprintf(stderr, "[WARNING] Code did not halt or infinite loop!\n");
    if (state.warnings > 0) fprintf(stderr, "[WARNING] Code generates warnings\n");
    first_file.close();
}
