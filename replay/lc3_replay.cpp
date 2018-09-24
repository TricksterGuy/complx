#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <algorithm>
#include <sstream>
#include "lc3_replay.hpp"
#include "BinaryStreamReader.hpp"

void lc3_setup_replay(lc3_state& state, std::istream& file, const std::string& replay_string, std::stringstream& newinput);

enum PreconditionFlag
{
    // True Traps Setting Flag. Default OFF
    TRUE_TRAPS = 1,
    // Interrupts Setting Flag. Default OFF
    INTERRUPTS = 2,
    // Plugins Setting Flag. Default ON
    PLUGINS = 3,
    // Strict Execution Setting Flag. Default ON
    STRICT_EXECUTION = 4,
    // Strategy for Initialization of Memory (0: Fill with Value 1: Seeded Fill 2: Seeded Full Randomization).
    MEMORY_STRATEGY = 5,
    // Parameter for Memory Strategy.
    MEMORY_STRATEGY_VALUE = 6,
    // Breakpoint address for Subroutine Testing.
    BREAK_ADDRESS = 7,

    // 8-16 reserved.
    END_OF_ENVIRONMENT = 16,

    // Set a Register.
    REGISTER = 17,
    // Set the PC.
    PC = 18,
    // Set a value at a label.
    VALUE = 19,
    // Set a value at an address pointed to by label.
    POINTER = 20,
    // Sets a region of memory starting at the address pointed to by label.
    ARRAY = 21,
    // Sets a string starting at the address pointed to by label.
    STRING = 22,
    // Sets console input
    INPUT = 23,
    // Simulate a call to a subroutine (following lc-3 calling convention).
    SUBROUTINE = 24,

    END_OF_INPUT = 0xFF,
};

using namespace boost::archive::iterators;
typedef transform_width<binary_from_base64<remove_whitespace<std::string::const_iterator>>, 8, 6> ItBinaryT;


std::string base64_decode(const std::string& str)
{
    try
    {
        size_t num_pad_chars((4 - str.size() % 4) % 4);
        std::string input = str + std::string(num_pad_chars, '=');

        size_t pad_chars = std::count(input.begin(), input.end(), '=');
        std::replace(input.begin(), input.end(), '=', 'A');

        std::string output(ItBinaryT(input.begin()), ItBinaryT(input.end()));

        output.erase(output.end() - pad_chars, output.end());

        return output;
    }
    catch (std::exception const& e)
    {
        return "";
    }
}

void lc3_setup_replay(lc3_state& state, std::string& filename, const std::string& replay_string, std::stringstream& newinput)
{

    std::ifstream file(filename.c_str());
    if (!file.good())
        throw "Could not open " + filename + " for reading";

    lc3_setup_replay(state, file, replay_string, newinput);
}

void lc3_setup_replay(lc3_state& state, std::istream& file, const std::string& replay_string, std::stringstream& newinput)
{
    std::string decoded = base64_decode(replay_string);
    if (decoded.empty())
        throw "Failed to parse replay string: " + replay_string;

    std::istringstream stream(decoded);
    BinaryStreamReader bstream(stream);

    char memory_strategy = 0;
    unsigned int memory_strategy_value = 0;
    bool true_traps = false;
    bool interrupts = false;
    bool strict_execution = true;
    bool plugins = true;
    unsigned int break_address = -1;

    std::stringstream error;

    while (bstream.Ok())
    {
        unsigned char id;
        unsigned int value;

        bstream >> id;

        if (id == END_OF_ENVIRONMENT)
            break;

        switch (id)
        {
            case TRUE_TRAPS:
            case INTERRUPTS:
            case PLUGINS:
            case STRICT_EXECUTION:
            case MEMORY_STRATEGY:
            case MEMORY_STRATEGY_VALUE:
            case BREAK_ADDRESS:
                bstream >> value;
                break;
            default:
                error << "Unknown tag found id: " << ((int)id);
                throw error.str();
        }

        switch (id)
        {
            case TRUE_TRAPS:
                true_traps = value;
                break;
            case INTERRUPTS:
                interrupts = value;
                break;
            case PLUGINS:
                plugins = value;
                break;
            case STRICT_EXECUTION:
                strict_execution = value;
                break;
            case MEMORY_STRATEGY:
                memory_strategy = value;
                break;
            case MEMORY_STRATEGY_VALUE:
                memory_strategy_value = value;
                break;
            case BREAK_ADDRESS:
                break_address = value;
                break;

        }
    }

    short random_value;
    switch (memory_strategy)
    {
        case 0:
            lc3_init(state, false, false, memory_strategy_value, memory_strategy_value);
            break;
        case 1:
            srand(memory_strategy_value);
            random_value = lc3_random();
            lc3_init(state, false, false, random_value, random_value);
            break;
        case 2:
            srand(memory_strategy_value);
            lc3_init(state);
            break;
    }

    lc3_set_true_traps(state, true_traps);
    state.interrupt_enabled = interrupts;
    state.strict_execution = strict_execution;
    LC3AssembleOptions options;
    options.multiple_errors = false;
    options.warnings_as_errors = false;
    options.process_debug_comments = false;
    options.enable_warnings = false;
    options.disable_plugins = !plugins;
    lc3_assemble(state, file, options);
    if (break_address != 0xFFFFFFFF)
        lc3_add_break(state, break_address);

    while (bstream.Ok())
    {
        unsigned char id;

        std::string label;
        std::vector<short> params;

        bstream >> id;

        if (id == END_OF_INPUT)
            break;

        switch (id)
        {
            case REGISTER:
            case PC:
            case VALUE:
            case POINTER:
            case ARRAY:
            case STRING:
            case INPUT:
            case SUBROUTINE:
                bstream >> label;
                bstream >> params;
                break;
            default:
                error << "Unknown tag found id: " << ((int)id);
                throw error.str();
        }

        unsigned short address;
        int address_calc;
        switch (id)
        {
            case VALUE:
            case POINTER:
            case ARRAY:
            case STRING:
            case SUBROUTINE:
                address_calc = lc3_sym_lookup(state, label);
                if (address_calc == -1)
                {
                    error << "Symbol " << label << " not found, perhaps you don't have the correct file loaded?";
                    throw error.str();
                }
                address = (short) address_calc;
                break;
        }

        switch (id)
        {
            case REGISTER:
                state.regs[label[0] - '0'] = params[0];
                break;
            case PC:
                state.pc = params[0];
                break;
            case VALUE:
                state.mem[address] = params[0];
                break;
            case POINTER:
                state.mem[static_cast<unsigned short>(state.mem[address])] = params[0];
                break;
            case STRING:
                state.mem[static_cast<unsigned short>(state.mem[address] + params.size())] = 0;
            case ARRAY:
                for (unsigned int i = 0; i < params.size(); i++)
                    state.mem[static_cast<unsigned short>(state.mem[address]) + i] = params[i];
                break;
            case INPUT:
                for (unsigned int i = 0; i < params.size(); i++)
                    newinput.put(params[i] & 0xff);
                break;
            case SUBROUTINE:
                state.pc = address;
                state.regs[5] = params[0];
                state.regs[6] = params[1] - (params.size() - 3);
                state.regs[7] = params[2];
                for (unsigned int i = 3; i < params.size(); i++)
                    state.mem[static_cast<unsigned short>(state.regs[6] + (i - 3))] = params[i];
                break;
        }
    }
}

