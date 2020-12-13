#include <lc3_replay/lc3_replay.hpp>
#include <lc3_replay/BinaryStreamReader.hpp>

#include <algorithm>
#include <iomanip>
#include <sstream>

#include <boost/archive/iterators/base64_from_binary.hpp>
#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/insert_linebreaks.hpp>
#include <boost/archive/iterators/remove_whitespace.hpp>
#include <boost/archive/iterators/transform_width.hpp>



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
    // LC3 Version
    LC3_VERSION = 8,

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
    // Simulate a call to a subroutine or trap using pass by regs.
    PASS_BY_REGS = 25,
    // Set an address to a value
    DIRECT_SET = 26,
    // Directly set a string in memory.
    DIRECT_STRING = 27,
    // Directly set an array in memory.
    DIRECT_ARRAY = 28,
    // Set a node in memory.
    // Format (data) -> Leaf node (of linked list)
    // (next, data) -> Linked List
    // (left, right, data) -> Tree Node
    NODE = 29,
    // An arbitrary struct/record/tuple.
    DATA = 30,

    END_OF_INPUT = 0xFF,
};


enum class PostconditionFlag
{
    // Halted or Returned from Subroutine Flag.
    END_STATE = 1,
    // Checks a register.
    REGISTER = 2,
    // Checks the PC value.
    PC = 3,
    // Checks a value at a label.
    VALUE = 4,
    // Check a value at an address pointed to by a label.
    POINTER = 5,
    // Checks a region starting at the address pointed to by label.
    ARRAY = 6,
    // Checks a string starting at the address pointed to by label.
    STRING = 7,
    // Checks console output.
    OUTPUT = 8,
    // Check a value at a memory address.
    DIRECT_VALUE = 9,
    // Check a string starting at a memory address.
    DIRECT_STRING = 10,
    // Check a sequence of values starting at a memory address.
    DIRECT_ARRAY = 11,
    // Check a node at a memory address
    NODE = 12,
    // Checks arbitrary data structure at a memory address.
    DATA = 13,
    // Checks return value for subroutine.
    RETURN_VALUE = 14,
    // Checks if registers are unchanged.
    REGISTERS_UNCHANGED = 15,
    // Checks if calling convention was followed.
    CALLING_CONVENTION_FOLLOWED = 16,
    // Checks for subroutine calls.
    SUBROUTINE_CALL = 17,
    // Checks for a subroutine/trap using pass by register.
    PASS_BY_REGS = 18,

    END_OF_POSTCONDITIONS = 0xff,
};

enum class DataItem
{
    // End of Data.
    EndData = 0,
    // A single 16 bit integer representing a value, or memory address.
    Number = 1,
    // A null terminated string which each char being 16 bits.
    String = 2,
    // An array of 16 bit integers.
    Array = 3,
    // An embedded data item.
    Data = 0xFF,
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

std::pair<uint16_t, int> write_data(lc3_state& state, uint16_t address, const std::vector<int16_t>& params, int i = 0)
{
    while (static_cast<size_t>(i) < params.size())
    {
        auto type = static_cast<DataItem>(params[i]);
        int16_t length = 0;
        std::pair<uint16_t, int> address_i;
        switch(type)
        {
            case DataItem::Number:
                state.mem[address] = params[i + 1];
                address++;
                i += 2;
                break;
            case DataItem::String:
                length = params[i + 1];
                for (int j = 0; j < length; j++)
                    state.mem[address + j] = params[i + 2 + j];
                state.mem[address + length] = 0;
                address += length + 1;
                i += 2 + length;
                break;
            case DataItem::Array:
                length = params[i + 1];
                for (int j = 0; j < length; j++)
                    state.mem[address + j] = params[i + 2 + j];
                address += length;
                i += 2 + length;
                break;
            case DataItem::Data:
                address_i = write_data(state, address, params, i + 1);
                address = address_i.first;
                i = address_i.second + 1;
                break;
            case DataItem::EndData:
                return std::make_pair(address, i);
            default:
                break;
        }
    }
    return std::make_pair(-1, -1);
}


std::pair<std::string, int> describe_data(const std::vector<int16_t>& params, int i = 0)
{
    std::stringstream out;

    out << "(";
    while (static_cast<size_t>(i) < params.size())
    {
        auto type = static_cast<DataItem>(params[i]);
        int16_t length = 0;
        std::pair<std::string, int> str_i;
        switch(type)
        {
            case DataItem::Number:
                out << params[i + 1];
                i += 2;
                break;
            case DataItem::String:
                length = params[i + 1];
                out << "\"";
                for (int j = 0; j < length; j++)
                    out << static_cast<char>(params[i + 2 + j]);
                out << "\"";
                i += 2 + length;
                break;
            case DataItem::Array:
                length = params[i + 1];
                out << "[";
                for (int j = 0; j < length; j++)
                {
                    out << params[i + 2 + j];
                    if (j != length - 1)
                        out << ", ";
                }
                out << "]";
                i += 2 + length;
                break;
            case DataItem::Data:
                str_i = describe_data(params, i + 1);
                out << str_i.first;
                i = str_i.second + 1;
                break;
            case DataItem::EndData:
                out << ")";
                return std::make_pair(out.str(), i);
            default:
                break;
        }
        if (static_cast<size_t>(i) != params.size() && static_cast<DataItem>(params[i]) != DataItem::EndData)
            out << ", ";
    }
    out << ")";
    return std::make_pair(out.str(), -1);
}

std::string describe_node(const std::vector<int16_t>& params)
{
    if (static_cast<DataItem>(params[0]) != DataItem::Array)
        return "invalid-node";
    std::stringstream out;
    int length = params[1];
    if (static_cast<DataItem>(params[2 + length]) != DataItem::Data)
        return "invalid-node";
    out << "next: [";
    for (int i = 0; i < length; i++)
    {
        out << "x" << std::hex << std::setw(4) << std::setfill('0') << params[2 + i];
        if (i != length - 1)
            out << ", ";
    }
    out << "] data: ";
    out << describe_data(params, length + 3).first;
    return out.str();
}

void lc3_setup_replay(lc3_state& state, const std::string& filename, const std::string& replay_string, std::stringstream& newinput)
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
    bstream.SetMaxStringSize(65536);
    bstream.SetMaxVectorSize(65536);

    char memory_strategy = 0;
    unsigned int memory_strategy_value = 0;
    bool true_traps = false;
    bool interrupts = false;
    bool strict_execution = true;
    bool plugins = true;
    unsigned int break_address = -1;
    int version = 1;

    std::stringstream error;

    while (bstream.Ok())
    {
        unsigned char raw_id;
        unsigned int value;

        bstream >> raw_id;
        auto id = static_cast<PreconditionFlag>(raw_id);

        if (!bstream.Ok())
            throw "Error reading replay string.";

        if (id == PreconditionFlag::END_OF_ENVIRONMENT)
            break;

        bstream >> value;
        if (!bstream.Ok())
            throw "Error reading replay string.";

        switch (id)
        {
            case PreconditionFlag::TRUE_TRAPS:
                true_traps = value;
                break;
            case PreconditionFlag::INTERRUPTS:
                interrupts = value;
                break;
            case PreconditionFlag::PLUGINS:
                plugins = value;
                break;
            case PreconditionFlag::STRICT_EXECUTION:
                strict_execution = value;
                break;
            case PreconditionFlag::MEMORY_STRATEGY:
                memory_strategy = value;
                break;
            case PreconditionFlag::MEMORY_STRATEGY_VALUE:
                memory_strategy_value = value;
                break;
            case PreconditionFlag::BREAK_ADDRESS:
                break_address = value;
                break;
            case PreconditionFlag::LC3_VERSION:
                version = value;
                break;
            default:
                error << "Unknown tag found id: " << static_cast<int>(id);
                throw error.str();
        }
    }

    int16_t random_value;
    switch (memory_strategy)
    {
        case 0:
            lc3_init(state, false, false, memory_strategy_value, memory_strategy_value);
            break;
        case 1:
            srand(memory_strategy_value);
            random_value = lc3_random(state);
            lc3_init(state, false, false, random_value, random_value);
            break;
        case 2:
            srand(memory_strategy_value);
            lc3_init(state);
            break;
        default:
            // Shouldn't happen.
	    break;
    }

    if (version < 0 || version > 1)
    {
        error << "Invalid LC-3 Version found version: " << version;
        throw error.str();
    }

    lc3_set_true_traps(state, true_traps);
    state.interrupt_enabled = interrupts;
    state.strict_execution = strict_execution;
    lc3_set_version(state, version);
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
        unsigned char raw_id;

        std::string label;
        std::vector<int16_t> params;

        bstream >> raw_id;
        auto id = static_cast<PreconditionFlag>(raw_id);

        if (!bstream.Ok())
            throw "Error reading replay string.";

        if (id == PreconditionFlag::END_OF_INPUT)
            break;

        bstream >> label;
        if (!bstream.Ok())
            throw "Error reading replay string.";
        bstream >> params;
        if (!bstream.Ok())
            throw "Error reading replay string.";

        uint16_t address;
        int address_calc;
        switch (id)
        {
            case PreconditionFlag::VALUE:
            case PreconditionFlag::POINTER:
            case PreconditionFlag::ARRAY:
            case PreconditionFlag::STRING:
            case PreconditionFlag::SUBROUTINE:
            case PreconditionFlag::PASS_BY_REGS:
                address_calc = lc3_sym_lookup(state, label);
                if (address_calc == -1)
                {
                    error << "Symbol " << label << " not found, perhaps you don't have the correct file loaded?";
                    throw error.str();
                }
                address = static_cast<uint16_t>(address_calc);
                break;
            case PreconditionFlag::DIRECT_SET:
            case PreconditionFlag::DIRECT_STRING:
            case PreconditionFlag::DIRECT_ARRAY:
            case PreconditionFlag::NODE:
            case PreconditionFlag::DATA:
                address_calc = strtoul(label.c_str(), nullptr, 16);
                if (address_calc > 0x10000 || address_calc < 0)
                {
                    error << "Address " << label << " not inside range for an address.";
                    throw error.str();
                }
                address = static_cast<uint16_t>(address_calc);
                break;
            default:
                address = 0;
		break;
        }

        switch (id)
        {
            case PreconditionFlag::REGISTER:
                state.regs[label[0] - '0'] = params[0];
                break;
            case PreconditionFlag::PC:
                state.pc = params[0];
                break;
            case PreconditionFlag::VALUE:
                state.mem[address] = params[0];
                break;
            case PreconditionFlag::POINTER:
                state.mem[static_cast<uint16_t>(state.mem[address])] = params[0];
                break;
            case PreconditionFlag::STRING:
                state.mem[static_cast<uint16_t>(state.mem[address] + params.size())] = 0;
                // fall through
            case PreconditionFlag::ARRAY:
                for (unsigned int i = 0; i < params.size(); i++)
                    state.mem[static_cast<uint16_t>(state.mem[address]) + i] = params[i];
                break;
            case PreconditionFlag::INPUT:
                for (const auto& param : params)
                    newinput.put(param & 0xff);
                break;
            case PreconditionFlag::SUBROUTINE:
                state.pc = address;
                state.regs[5] = params[0];
                state.regs[6] = params[1] - (params.size() - 3);
                state.regs[7] = params[2];
                for (unsigned int i = 3; i < params.size(); i++)
                    state.mem[static_cast<uint16_t>(state.regs[6] + (i - 3))] = params[i];
                break;
            case PreconditionFlag::PASS_BY_REGS:
                state.pc = address;
                for (unsigned int i = 0; i < params.size(); i += 2)
                    state.regs[params[i] & 0x7] = params[i + 1];
                break;
            case PreconditionFlag::DIRECT_SET:
                state.mem[address] = params[0];
                break;
            case PreconditionFlag::DIRECT_STRING:
                state.mem[address + params.size()] = 0;
                // fall through
            case PreconditionFlag::DIRECT_ARRAY:
                for (unsigned int i = 0; i < params.size(); i++)
                    state.mem[address + i] = params[i];
                break;
            case PreconditionFlag::NODE:
            case PreconditionFlag::DATA:
                write_data(state, address, params);
                break;
            default:
                error << "Unknown tag found id: " << static_cast<int>(id);
                throw error.str();
        }
    }
}

std::string lc3_describe_replay(const std::string& replay_string)
{
    std::string decoded = base64_decode(replay_string);
    if (decoded.empty())
        throw "Failed to parse replay string: " + replay_string;

    std::istringstream stream(decoded);
    BinaryStreamReader bstream(stream);
    std::stringstream description;

    std::stringstream error;

    while (bstream.Ok())
    {
        unsigned char raw_id;
        unsigned int value;

        bstream >> raw_id;
        auto id = static_cast<PreconditionFlag>(raw_id);

        if (id == PreconditionFlag::END_OF_ENVIRONMENT)
            break;

        bstream >> value;

        switch (id)
        {
            case PreconditionFlag::TRUE_TRAPS:
                description << "true_traps: " << (value ? "on" : "off") << std::endl;
                break;
            case PreconditionFlag::INTERRUPTS:
                description << "interrupts: " << (value ? "on" : "off") << std::endl;
                break;
            case PreconditionFlag::PLUGINS:
                description << "plugins: " << (value ? "on" : "off") << std::endl;
                break;
            case PreconditionFlag::STRICT_EXECUTION:
                description << "strict_execution: " << (value ? "on" : "off") << std::endl;
                break;
            case PreconditionFlag::MEMORY_STRATEGY:
                description << "memory_strategy: " << ((value == 0) ? "fill_with_value" : ((value == 1) ? "random_fill_with_seed" : "completely_random_with_seed")) << std::endl;
                break;
            case PreconditionFlag::MEMORY_STRATEGY_VALUE:
                description << "memory_strategy_value: " << value << std::endl;
                break;
            case PreconditionFlag::BREAK_ADDRESS:
                description << "breakpoint: x" << std::hex << std::setw(4) << std::setfill('0') << value << std::endl;
                break;
            case PreconditionFlag::LC3_VERSION:
                description << "LC-3 version: " << value << std::endl;
                break;
            default:
                error << "Unknown tag found id: " << static_cast<int>(id);
                throw error.str();
        }
    }

    description << "\n";

    while (bstream.Ok())
    {
        unsigned char raw_id;

        std::string label;
        std::vector<int16_t> params;

        bstream >> raw_id;
        auto id = static_cast<PreconditionFlag>(raw_id);

        if (id == PreconditionFlag::END_OF_INPUT)
            break;

        bstream >> label;
        bstream >> params;

        switch (id)
        {
            case PreconditionFlag::REGISTER:
                description << "R" << label << " = (" << std::dec << params[0] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[0] << ")" << std::endl;
                break;
            case PreconditionFlag::PC:
                description << "PC = x" << std::hex << std::setw(4) << std::setfill('0') << params[0] << std::endl;
                break;
            case PreconditionFlag::VALUE:
                description << "MEM[" << label << "] = (" << std::dec << params[0] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[0] << ")" << std::endl;
                break;
            case PreconditionFlag::POINTER:
                description << "MEM[MEM[" << label << "]] = (" << std::dec << params[0] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[0] << ")" << std::endl;
                break;
            case PreconditionFlag::STRING:
                description << "String at MEM[" << label << "] = \"";
                for (const auto& param : params)
                    description << static_cast<char>(param);
                description << "\"" << std::endl;
                break;
            case PreconditionFlag::ARRAY:
                description << "Array at MEM[" << label << "] = ";
                for (unsigned int i = 0; i < params.size(); i++)
                {
                    description << "(" << std::dec << params[i] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[i] << ")";
                    if (i != params.size() - 1)
                        description << ",";
                }
                description << std::endl;
                break;
            case PreconditionFlag::INPUT:
                description << "Console Input ";
                for (const auto& param : params)
                    description << static_cast<char>(param);
                description << std::endl;
                break;
            case PreconditionFlag::SUBROUTINE:
                description << "Call Subroutine " << label << " params: ";
                for (unsigned int i = 3; i < params.size(); i++)
                {
                    description << "(" << std::dec << params[i] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[i] << ")";
                    if (i != params.size() - 1)
                        description << ",";
                }
                description << " R5 = x" << std::hex << std::setw(4) << std::setfill('0') << params[0] << " ";
                description << "R6 = x" << std::hex << std::setw(4) << std::setfill('0') << params[1] << " ";
                description << "R7 = x" << std::hex << std::setw(4) << std::setfill('0') << params[2] << std::endl;
                break;
            case PreconditionFlag::PASS_BY_REGS:
                description << "Call Subroutine " << label;
                for (unsigned int i = 0; i < params.size(); i += 2)
                {
                    description << " R" << params[i] << " = ";
                    description << "(" << std::dec << params[i + 1] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[i + 1] << ")";
                }
                break;
            case PreconditionFlag::DIRECT_SET:
                description << "MEM[x" << label << "] = (" << std::dec << params[0] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[0] << ")" << std::endl;
                break;
            case PreconditionFlag::DIRECT_STRING:
                description << "MEM[x" << label << "] = \"";
                for (const auto& param : params)
                    description << static_cast<char>(param);
                description << "\"" << std::endl;
                break;
            case PreconditionFlag::DIRECT_ARRAY:
                description << "MEM[x" << label << "] = [";
                for (unsigned int i = 0; i < params.size(); i++)
                {
                    description << "(" << std::dec << params[i] << " x" << std::hex << std::setw(4) << std::setfill('0') << params[i] << ")";
                    if (i != params.size() - 1)
                        description << ", ";
                }
                description << "]" << std::endl;
                break;
            case PreconditionFlag::NODE:
                description << "Node at MEM[x" << label << "] ";
                description << describe_node(params) << std::endl;
                break;
            case PreconditionFlag::DATA:
                description << "MEM[x" << label << "] = ";
                description << describe_data(params).first << std::endl;
                break;
            default:
                error << "Unknown tag found id: " << static_cast<int>(id);
                throw error.str();
        }
    }

    return description.str();
}
