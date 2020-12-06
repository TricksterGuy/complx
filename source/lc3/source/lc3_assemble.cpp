#include "lc3/lc3_assemble.hpp"

#include <algorithm>
#include <bitset>
#include <cassert>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifdef __linux__
#include <arpa/inet.h>
#include <netinet/in.h>
#else
#ifdef _WIN32
#include <winsock2.h>
#endif
#endif

#include "lc3/lc3_debug.hpp"
#include "lc3/lc3_parser.hpp"
#include "lc3/lc3_plugin.hpp"
#include "lc3/lc3_runner.hpp"
#include "lc3/lc3_symbol.hpp"

// For non line comments.
struct code_line
{
    code_line(const std::string& line_str, unsigned int loc) : line(line_str), location(loc) {}
    std::string line;
    unsigned int location;
};

// For comments with debugging information
struct debug_statement
{
    debug_statement(const std::string& lineno, unsigned int loc, unsigned short addr) :
        line(lineno), location(loc), address(addr) {}
    std::string line;
    unsigned int location;
    unsigned short address;
};

unsigned short lc3_assemble_one(lc3_state& state, LC3AssembleContext& context);

void process_debug_info(lc3_state& state, const debug_statement& statement, bool enable_debug_statements);
void process_plugin_info(lc3_state& state, const LC3AssembleContext& context);
void process_version_info(lc3_state& state, const LC3AssembleContext& context);
void parse_params(const std::string& line, std::map<std::string, std::string>& params);

std::string LC3AssembleException::what() const throw()
{
    std::stringstream stream;
    std::string line_str = "";

    if (lineno != -1)
    {
        std::stringstream line_stream;
        line_stream << " on line " << lineno;
        line_str = line_stream.str() ;
    }

    switch(id)
    {
        case SYNTAX_ERROR:
            stream << "Syntax Error" << line_str << ": " << line;
            break;
        case ORIG_MATCHUP:
            stream << "No matching .end" << line_str << ": " << line;
            break;
        case ORIG_OVERLAP:
            stream << "Code sections " << params[0] << " and " << params[1] << "overlap";
            break;
        case STRAY_END:
            stream << "No matching .orig found for .end" << line_str;
            break;
        case STRAY_DATA:
            stream << "Stray data found" << line_str << ": " << line;
            break;
        case UNDEFINED_SYMBOL:
            stream << "Undefined symbol " << params[0] << line_str;
            break;
        case DUPLICATE_SYMBOL:
            stream << "Duplicate symbol " << params[0] << line_str;
            break;
        case MULTIPLE_SYMBOL:
            stream << "Multiple symbols " << params[0] << " and " << params[1] << " found at address x" << params[2] << line_str;
            break;
        case INVALID_SYMBOL:
            stream << "Invalid symbol " << params[0] << line_str;
            break;
        case INVALID_REGISTER:
            stream << "Invalid register " << params[0] << line_str;
            break;
        case INVALID_INSTRUCTION:
            stream << "Invalid instruction \"" << params[0] << "\"" << line_str;
            break;
        case INVALID_DIRECTIVE:
            stream << "Invalid assembler directive " << params[0] << line_str;
            break;
        case INVALID_FLAGS:
            stream << "Undefined condition code flags " << params[0] << line_str;
            break;
        case INVALID_CHARACTER:
            stream << "Invalid character constant " << params[0] << line_str;
            break;
        case INVALID_NUMBER:
            stream << "Found signed number " << params[0] << " but was expecting an unsigned number" << line_str;
            break;
        case INVALID_BYTES:
            stream << "Invalid byte " << static_cast<int>(params[0][0]) << " found" << line_str << ".\nUse a text editor that can display non ASCII / unprintable characters and remove them.";
            break;
        case NUMBER_OVERFLOW:
            stream << params[0] << "is too big for an immediate value. Expected " << params[1] << " bits got " << params[2] << " bits" << line_str;
            break;
        case OFFSET_OVERFLOW:
            stream << params[0] << "is too big for an offset. Expected " << params[1] << " bits got " << params[2] << " bits" << line_str;
            break;
        case MEMORY_OVERFLOW:
            stream << "Can't add by " << params[0] << line_str;
            break;
        case SCAN_OVERFLOW:
            stream << "End of memory (xFFFF) reached and I will not wrap around" << line_str;
            break;
        case FILE_ERROR:
            stream << "Could not open " << params[0] << " for reading.\nAre you sure the file is in your current working directory?";
            break;
        case UNTERMINATED_STRING:
            stream << "Unterminated string " << params[0] << line_str;
            break;
        case MALFORMED_STRING:
            stream << "Malformed string " << params[0] << line_str;
            break;
        case EXTRA_INPUT:
            stream << "Extra input found " << params[0] << line_str;
            break;
        case PLUGIN_FAILED_TO_LOAD:
            stream << "Plugin failed to load" << line_str << "\n" << params[0];
            break;
        case INVALID_LC3_VERSION:
            stream << "Invalid LC3 Version " << params[0] << line_str;
            break;
        case UNKNOWN_ERROR:
            // fall through
        default:
            stream << "Unknown error found" << line;
            break;
    }
    return stream.str();
}

unsigned short lc3_assemble_one(lc3_state& state, unsigned short address, const std::string& line, int lineno, const LC3AssembleOptions& options)
{
    LC3AssembleContext context;
    context.line = line;
    context.lineno = lineno;
    context.state = &state;
    context.address = address;
    context.options = options;

    unsigned short ret = lc3_assemble_one(state, context);

    if (context.options.multiple_errors && !context.exceptions.empty())
        throw context.exceptions;

    return ret;
}

unsigned short lc3_assemble_one(lc3_state& state, LC3AssembleContext& context)
{
    size_t pos = context.line.find_first_of(" \t");
    std::string opcode = context.line.substr(0, pos);
    trim(opcode);
    std::string line = (pos == std::string::npos) ? "" : context.line.c_str() + pos + 1;
    trim(line);
    tokenize(context.line, context.tokens, " \t,");

    std::vector<std::string> tokens;
    tokenize(line, tokens, ",");
    // Remove spaces
    for (unsigned int i = 0; i < tokens.size(); i++)
        trim(tokens[i]);

    int specialop;
    int opcode_id = get_opcode(opcode, specialop, context);
    int dr, sr1, sr2_imm;
    bool is_reg;
    bool n, z, p;
    unsigned short instruction = (opcode_id << 12);
    unsigned int params = 0;

    switch(opcode_id)
    {
    case ADD_INSTR:
    case AND_INSTR:
        params = 3;
        break;
    case NOT_INSTR:
        params = 2;
        break;
    case BR_INSTR:
        params = 1;
        break;
    case JMP_INSTR:
        params = specialop ? 0 : 1;
        break;
    case JSR_INSTR:
        params = 1;
        break;
    case RTI_INSTR:
        params = 0;
        break;
    case LEA_INSTR:
    case LD_INSTR:
    case LDI_INSTR:
    case ST_INSTR:
    case STI_INSTR:
        params = 2;
        break;
    case LDR_INSTR:
    case STR_INSTR:
        params = 3;
        break;
    case TRAP_INSTR:
        // Special op contains trap number if trapname.
        params = specialop ? 0 : 1;
        break;
    case ERROR_INSTR:
        // Let the plugin handle it
        params = tokens.size();
        break;
    default: /* Just in case */
        break;
    }

    if (tokens.size() < params)
    {
        THROWANDDO(LC3AssembleException(context.line, "", SYNTAX_ERROR, context.lineno), return 0);
    }
    else if (tokens.size() > params)
    {
        THROW(LC3AssembleException(context.line, "", EXTRA_INPUT, context.lineno));
    }

    switch(opcode_id)
    {
    case ADD_INSTR:
    case AND_INSTR:
        dr = get_register(tokens[0], context);
        sr1 = get_register(tokens[1], context);
        sr2_imm = get_register_imm5(tokens[2], is_reg, context);
        instruction |= (dr << 9) | (sr1 << 6) | (!is_reg << 5) | sr2_imm;
        break;
    case NOT_INSTR:
        dr = get_register(tokens[0], context);
        sr1 = get_register(tokens[1], context);
        instruction |= (dr << 9) | (sr1 << 6) | 0x3F;
        break;
    case BR_INSTR:
        get_cc_flags(opcode, n, z, p, context);
        dr = get_offset(tokens[0], 9, context);
        instruction |= (n << 11) | (z << 10) | (p << 9) | dr;
        break;
    case JMP_INSTR:
        // Special op contains 1 if RET
        dr = specialop ? 7 : get_register(tokens[0], context);
        instruction |= dr << 6;
        break;
    case JSR_INSTR:
        instruction |= (specialop << 11);
        // special op contains a 1 if JSR
        if (specialop)
        {
            dr = get_offset(tokens[0], 11, context);
            instruction |= dr;
        }
        else
        {
            dr = get_register(tokens[0], context);
            instruction |= dr << 6;
        }
        break;
    case RTI_INSTR:
        break;
    case LEA_INSTR:
    case LD_INSTR:
    case LDI_INSTR:
    case ST_INSTR:
    case STI_INSTR:
        dr = get_register(tokens[0], context);
        sr1 = get_offset(tokens[1], 9, context);
        instruction |= (dr << 9) | sr1;
        break;
    case LDR_INSTR:
    case STR_INSTR:
        dr = get_register(tokens[0], context);
        sr1 = get_register(tokens[1], context);
        sr2_imm = get_imm(tokens[2], 6, false, true, context);
        instruction |= (dr << 9) | (sr1 << 6) | sr2_imm;
        break;
    case TRAP_INSTR:
        // Special op contains trap number if trapname.
        // get_imm with false because its unsigned
        dr = specialop ? specialop : get_imm(tokens[0], 8, true, false, context);
        instruction |= dr;
        break;
    case ERROR_INSTR:
        // If there is an instruction defined to replace this then call the plugin's one.
        if (state.instructionPlugin)
            instruction = state.instructionPlugin->DoAssembleOne(state, context);
        else
            THROWANDDO(LC3AssembleException(line, opcode, INVALID_INSTRUCTION, context.lineno), return 0);
        break;
    default: /* Just in case */
        THROWANDDO(LC3AssembleException(line, opcode, INVALID_INSTRUCTION, context.lineno), return 0);
        break;
    }

    return instruction;
}

void lc3_assemble(lc3_state& state, const std::string& filename, const LC3AssembleOptions& options)
{
    std::vector<code_range> ranges;
    lc3_assemble(state, filename, ranges, options);
}

void lc3_assemble(lc3_state& state, const std::string& filename, std::vector<code_range>& ranges, const LC3AssembleOptions& options)
{
    std::ifstream file(filename.c_str());
    if (!file.good())
        throw LC3AssembleException("", filename, FILE_ERROR);

    lc3_assemble(state, file, ranges, options);
}

void lc3_assemble(lc3_state& state, std::istream& file, std::vector<code_range>& ranges, const LC3AssembleOptions& options)
{
    std::vector<code_line> code;
    std::vector<debug_statement> debugging;
    std::stringstream comments;
    code_range current_location(0, 0);
    code_line last_orig("", 0);

    LC3AssembleContext context;
    context.address = 0x0000;
    context.line = "";
    context.lineno = 0;
    context.state = &state;
    context.options = options;
    bool in_orig = false;

    // First pass get all symbols.
    while (!file.eof())
    {
        unsigned short last_addr = context.address;
        std::string line;
        std::string comment = "";
        std::getline(file, line);
        context.lineno++;
        context.line = line;
        remove_comments(line, comment);

        // Plugins are mission critical especially if they are instruction plugins that modify the assembling process!
        if (comment.size() > 2 && comment.substr(1, 7) == std::string("@plugin") && !context.options.disable_plugins)
        {
            process_plugin_info(state, context);
        }
        else if (comment.size() > 2 && comment.substr(1, 8) == std::string("@version"))
        {
            process_version_info(state, context);
        }
        else if (comment.size() > 2 && comment[1] == '@')
        {
            debugging.push_back(debug_statement(comment.substr(2), context.lineno, context.address));
        }
        else if (!comment.empty() && in_orig)
        {

            std::string trimmed = comment.substr(1, std::string::npos);
            trim(trimmed);
            comments << trimmed;
            comments << "\n";
        }

        if (line.empty()) continue;

        for (const char& c : line)
        {
            if (static_cast<unsigned char>(c) >= 0x7F || !(isgraph(c) || isspace(c)))
                THROWANDDO(LC3AssembleException(context.line, std::string(1, c), INVALID_BYTES, context.lineno), continue);
        }

        if (in_orig && !comments.str().empty())
        {
            state.comments[context.address] = comments.str();
            comments.str("");
        }

        code.push_back(code_line(line, context.lineno));

        std::vector<std::string> tokens;
        tokenize(line, tokens, " \t,");
        context.tokens = tokens;

        std::string symbol = "";

        // If not an assembler pseudoop
        if (tokens[0][0] != '.')
        {
            bool validate_failed = false;
            symbol = tokens[0];

            //if (symbol[symbol.size() - 1] == ':')
            //symbol = symbol.substr(0, symbol.size() - 1);
            if (symbol.size() >= 20)
            {
                THROWANDDO(LC3AssembleException(context.line, symbol, INVALID_SYMBOL, context.lineno), goto symbolcheckdone);
            }
            if (symbol.size() == 2 && (symbol[0] == 'r' || symbol[0] == 'R') && (symbol[1] >= '0' && symbol[1] <= '7'))
            {
                THROWANDDO(LC3AssembleException(context.line, symbol, INVALID_SYMBOL, context.lineno), goto symbolcheckdone);
            }
            if (is_hex(symbol) || is_binary(symbol))
            {
                THROWANDDO(LC3AssembleException(context.line, symbol, INVALID_SYMBOL, context.lineno), goto symbolcheckdone);
            }


            // Validate symbol should only have alphanumeric
            // And should start with an alpha character
            if (!isalpha(symbol[0]) && symbol[0] != '_')
            {
                THROWANDDO(LC3AssembleException(context.line, symbol, INVALID_SYMBOL, context.lineno), validate_failed = true);
            }

            for (unsigned int i = 1; i <  symbol.size(); i++)
            {
                if (!isalnum(symbol[i]) && symbol[i] != '_')
                {
                    if (!validate_failed)
                        THROWANDDO(LC3AssembleException(context.line, symbol, INVALID_SYMBOL, context.lineno), validate_failed = true);
                }
            }

symbolcheckdone:
            // Ugly cases like A.fill will be treated as a symbol.
            if (validate_failed)
                continue;

            int specialop;
            int op = get_opcode(symbol, specialop, context, false);
            //printf("symbol: %s, %d\n", symbol.c_str(), op);
            // They could do RET: so if getopcode returns a valid opcode and the : was removed then error.
            if (op != -1 && symbol != tokens[0])
            {
                THROWANDDO(LC3AssembleException(context.line, symbol, INVALID_SYMBOL, context.lineno), continue);
            }
            if (op == -1 && specialop == -1)
            {

                // A symbol was already declared for this address
                // Example .asm file snippet to get this
                // HELLO
                // WORLD ADD R0, R0, R0
                tokens.erase(tokens.begin());
                if (!in_orig)
                {
                    THROWANDDO(LC3AssembleException(context.line, "", STRAY_DATA, context.lineno), continue);
                }
                else
                {
                    // I don't like this
                    if (!lc3_sym_rev_lookup(*context.state, context.address).empty())
                    {
                        std::vector<std::string> params;
                        std::stringstream oss;
                        oss << std::hex << context.address;
                        params.push_back(symbol);
                        params.push_back(lc3_sym_rev_lookup(*context.state, context.address));
                        params.push_back(oss.str());
                        WARN(LC3AssembleException("", params, MULTIPLE_SYMBOL, context.lineno));
                    }
                    if (!lc3_sym_add(*context.state, symbol, context.address))
                    {
                        THROW(LC3AssembleException("", symbol, DUPLICATE_SYMBOL, context.lineno));
                    }
                }

                // Check for this case
                // HELLO WORLD ADD R0, R0, R0
                // Two symbols on one line
                //printf("CHECK %d %s %s\n", tokens.size(), symbol.c_str(), context.line.c_str());
                while (tokens.size() >= 1 && tokens[0][0] != '.')
                {
                    std::string symbol2 = tokens[0];
                    op = get_opcode(symbol2, specialop, context, false);
                    //printf("SYMBOL2 %d %s\n", op, symbol2.c_str());
                    if (op == -1 && specialop == -1)
                    {
                        tokens.erase(tokens.begin());
                        std::vector<std::string> params;
                        std::stringstream oss;
                        oss << std::hex << context.address;
                        params.push_back(symbol);
                        params.push_back(symbol2);
                        params.push_back(oss.str());
                        WARN(LC3AssembleException("", params, MULTIPLE_SYMBOL, context.lineno));
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        if (tokens.empty()) continue;

        // If assembler directive
        if (tokens[0][0] == '.')
        {
            unsigned int paramcheck = 2;
            std::string directive = tokens[0];
            std::transform(directive.begin(), directive.end(), directive.begin(), (int (*)(int))std::tolower);
            std::string param = tokens.size() > 1 ? tokens[1] : "";

            if (directive.compare(".orig") == 0)
            {
                if (current_location.size != 0)
                    THROW(LC3AssembleException(last_orig.line, "",  ORIG_MATCHUP, last_orig.location));

                current_location.location = get_imm(param, 16, true, false, context);
                current_location.size = 0;
                last_orig.line = context.line;
                last_orig.location = context.lineno;
                context.address = current_location.location;
                in_orig = true;
            }
            else if (directive.compare(".end") == 0)
            {
                if (current_location.location == 0 && current_location.size == 0)
                    THROW(LC3AssembleException("", "", STRAY_END, context.lineno));
                else if (current_location.size != 0)
                    ranges.push_back(current_location);

                current_location.location = 0;
                current_location.size = 0;
                context.address = 0;
                paramcheck = 1;
                in_orig = false;
            }
            else if (directive.compare(".stringz") == 0)
            {
                size_t start, end;
                start = line.find("\"");
                end = line.rfind("\"");

                if (start == std::string::npos)
                {
                    // Bad string ex. .stringz lol"lol"
                    THROW(LC3AssembleException(context.line, "", MALFORMED_STRING, context.lineno));
                }

                std::string str = line.substr(start, end - start + 1);
                int size = process_str(str, context).size() + 1;
                if (context.address + size > 0xFFFF)
                {
                    std::stringstream oss;
                    oss << size;
                    THROW(LC3AssembleException("", oss.str(), MEMORY_OVERFLOW, context.lineno));
                }
                context.address += size;
                current_location.size += size;
                paramcheck = (unsigned int) -1;
            }
            else if (directive.compare(".fill") == 0)
            {
                context.address += 1;
                current_location.size += 1;
                paramcheck = (unsigned int) -1;
            }
            else if (directive.compare(".blkw") == 0)
            {
                unsigned short locs = get_imm(param, 16, true, false, context);
                // No, no, no ex .orig xFFFF .blkw 2 .end
                if (context.address + locs > 0xFFFF)
                    THROW(LC3AssembleException("", param, MEMORY_OVERFLOW, context.lineno));
                context.address += locs;
                current_location.size += locs;
            }
            else
            {
                // Using an assembler directive I don't know of ex. .fillme 24
                THROW(LC3AssembleException("", tokens[0], INVALID_DIRECTIVE, context.lineno));
            }

            if (tokens.size() > paramcheck)
            {
                // Extra input found after assembler directive (ex. .blkw 18 24)
                THROW(LC3AssembleException(context.line, "", EXTRA_INPUT, context.lineno));
            }
        }
        else
        {
            // Should have a valid instruction here.
            int specialop;
            get_opcode(tokens[0], specialop, context, false);
            /*int op = get_opcode(tokens[0], specialop, context, false);
            if (!context.multiple)
                assert(!(op == -1 && specialop == -1));*/
            context.address += 1;
            current_location.size += 1;
        }

        if (!in_orig && last_addr != context.address && current_location.size != 0)
        {
            // Caught having data outside a .orig/.end pair
            THROW(LC3AssembleException(context.line, "", STRAY_DATA, context.lineno));
        }

        // Can't wrap around memory ex. .orig xFFFF .fill xFFFF .fill x0000 .end
        //// This is bad anyway you slice it
        if (current_location.location + current_location.size > 65536)
        {
            THROW(LC3AssembleException(context.line, "", SCAN_OVERFLOW, context.lineno));
        }
    }


    if (in_orig)
    {
        THROW(LC3AssembleException(last_orig.line, "",  ORIG_MATCHUP, last_orig.location));
    }

    for (unsigned int i = 0; i < ranges.size(); i++)
    {
        code_range rangei = ranges[i];
        for (unsigned int j = i + 1; j < ranges.size(); j++)
        {
            code_range rangej = ranges[j];
            if (rangei.location + rangei.size >= rangej.location && rangej.location + rangej.size >= rangei.location)
            {
                std::vector<std::string> params;
                std::stringstream oss;
                std::stringstream oss2;
                oss << std::hex << rangei.location;
                oss2 << std::hex << rangej.location;
                params.push_back(oss.str());
                params.push_back(oss2.str());
                THROW(LC3AssembleException("", params, ORIG_OVERLAP, -1));
            }
        }
    }
    context.address = 0x0000;
    context.line = "";
    context.lineno = 0;
    context.state = &state;

    // Second pass actually do things now.
    for (unsigned int i = 0; i < code.size(); i++)
    {
        std::string line = code[i].line;
        context.lineno = code[i].location;
        context.line = line;
        std::vector<std::string> symbols;

        std::vector<std::string> tokens;
        tokenize(line, tokens, " \t,");

        context.tokens = tokens;

        std::string symbol = "";
        //printf("-------\n");
        while (!tokens.empty() && !tokens[0].empty() && tokens[0][0] != '.')
        {
            symbol = tokens[0];
            // If a Register or an immediate value then we have gone too far.
            if (is_register_or_imm(symbol))
            {
                // undo last and break
                if (!symbols.empty())
                {
                    symbol = symbols.back();
                    symbols.pop_back();
                    tokens.insert(tokens.begin(), symbol);
                }
                break;
            }
            //printf("symbol: %s ", symbol.c_str());

            int specialop;
            int op = get_opcode(symbol, specialop, context, false);
            //printf("op: %d specialop: %d\n", op, specialop);
            if (op == -1 && specialop == -1)
            {
                tokens.erase(tokens.begin());
                symbols.push_back(symbol);
            }
            else
            {
                break;
            }
        }

        if (tokens.empty()) continue;
        // If assembler directive
        if (tokens[0][0] == '.')
        {
            std::string param = tokens.size() > 1 ? tokens[1] : "";
            std::string directive = tokens[0];
            std::string rest = line.substr(line.find(directive) + directive.size());
            trim(rest);
            std::transform(directive.begin(), directive.end(), directive.begin(), (int (*)(int))std::tolower);

            if (directive.compare(".orig") == 0)
            {
                current_location.location = get_imm(param, 16, true, false, context);
                context.address = current_location.location;
            }
            else if (directive.compare(".stringz") == 0)
            {
                std::string processed = process_str(rest, context);
                int size = processed.size() + 1;

                for (int j = 0; j < size - 1; j++)
                    state.mem[context.address + j] = processed[j];
                state.mem[context.address + size - 1] = 0;

                context.address += size;
            }
            else if (directive.compare(".fill") == 0)
            {
                state.mem[context.address] = get_fill_value(rest, context);
                context.address += 1;
            }
            else if (directive.compare(".blkw") == 0)
            {
                unsigned short locs = get_imm(param, 16, true, false, context);
                // blkw should not emit anything to memory
                //memset(state.mem + context.address, 0, locs * sizeof(short));
                context.address += locs;
            }
        }
        else
        {
            //printf("context.line: %s tokens[0] %s\n", context.line.c_str(), symbols.empty() ? "" : symbols[0].c_str());
            size_t index = symbols.empty() ? 0 : context.line.find(symbols[symbols.size() - 1]) + symbols[symbols.size() - 1].size();
            std::string instruction = context.line.substr(index);
            trim(instruction);

            context.line = instruction;
            context.tokens.clear();
            // Should have a valid instruction here.
            state.mem[context.address] = lc3_assemble_one(*context.state, context);
            context.address += 1;
        }
    }

    // Process all debug statements you have received during first pass
    // @break[point]
    // @watch[point]
    // @black[box]
    for (unsigned int i = 0; i < debugging.size(); i++)
    {
        debug_statement statement = debugging[i];
        process_debug_info(state, statement, context.options.process_debug_comments);
    }


    if (context.options.multiple_errors && !context.exceptions.empty())
    {
        throw context.exceptions;
    }
}

void lc3_assemble(lc3_state& state, std::istream& file, const LC3AssembleOptions& options)
{
    std::vector<code_range> ranges;
    lc3_assemble(state, file, ranges, options);
}

bool lc3_assemble_object_writer(const std::string& filename, const lc3_state& state, const std::vector<code_range>& ranges)
{
    std::string obj_file = filename + ".obj";
    std::ofstream obj(obj_file.c_str(), std::ios::binary);
    if (!obj.good()) return false;

    for (const auto& range : ranges)
    {
        if (range.size == 0) continue;

        short lol = htons(range.location);
        obj.write((char*)(&lol), sizeof(short));
        lol = htons(range.size);
        obj.write((char*)(&lol), sizeof(short));
        for (unsigned int j = 0; j < range.size; j++)
        {
            lol = htons(state.mem[range.location + j]);
            obj.write((char*)(&lol), sizeof(short));
        }
    }

    return true;
}

bool lc3_assemble_binary_writer(const std::string& filename, const lc3_state& state, const std::vector<code_range>& ranges)
{
    std::string obj_file = filename + ".bin";
    std::ofstream obj(obj_file.c_str(), std::ios::binary);
    if (!obj.good())
        return false;

    for (const auto& range : ranges)
    {
        if (range.size == 0)
            continue;

        obj << "x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << range.location  << std::endl << std::nouppercase;
        obj << std::dec << range.size << std::endl;

        for (unsigned int j = 0; j < range.size; j++)
            obj << std::bitset<16>(state.mem[range.location + j]) << std::endl;

        obj << std::endl;
    }

    return true;
}

bool lc3_assemble_hexadecimal_writer(const std::string& filename, const lc3_state& state, const std::vector<code_range>& ranges)
{
    std::string obj_file = filename + ".hex";
    std::ofstream obj(obj_file.c_str(), std::ios::binary);
    if (!obj.good())
        return false;

    for (const auto& range : ranges)
    {
        if (range.size == 0)
            continue;

        obj << "x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << range.location  << std::endl << std::nouppercase;
        obj << std::dec << range.size << std::endl;
        for (unsigned int j = 0; j < range.size; j++)
        {
            obj << "x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << state.mem[range.location + j] << std::endl << std::nouppercase;
        }
        obj << std::endl;
    }

    return true;
}

bool lc3_assemble_full_writer(const std::string& filename, lc3_state& state, const std::vector<code_range>& ranges)
{
    std::string obj_file = filename + ".txt";
    std::ofstream obj(obj_file.c_str(), std::ios::binary);
    if (!obj.good())
        return false;

    unsigned short pc = state.pc;
    for (const auto& range : ranges)
    {
        if (range.size == 0)
            continue;

        for (unsigned int j = 0; j < range.size; j++)
        {
            unsigned short address = range.location + j;
            state.pc = address + 1;
            short data = state.mem[range.location + j];
            unsigned short udata = static_cast<unsigned short>(data);
            obj << "x" << std::hex << std::uppercase << std::setw(4) << std::setfill('0') << address << std::nouppercase << "\t";
            obj << "x" << std::hex << std::uppercase << std::setw(4) << udata << std::dec << std::nouppercase << std::setfill(' ') << "\t";
            obj << std::setw(6) << data << "\t";
            obj << std::bitset<16>(udata) << "\t";
            obj << lc3_sym_rev_lookup(const_cast<lc3_state&>(state), address) << "\t";
            obj << lc3_disassemble(const_cast<lc3_state&>(state), udata, 1) << std::endl;
        }
    }
    state.pc = pc;

    return true;
}

bool lc3_assemble(const std::string& filename, const std::string& output_prefix, const LC3AssembleOptions& options)
{
    lc3_state state;
    lc3_init(state);
    std::vector<code_range> ranges;
    lc3_assemble(state, filename, ranges, options);
    std::string prefix = output_prefix;
    if (output_prefix.empty())
        prefix = filename.substr(0, filename.rfind('.'));

    if (!state.symbols.empty())
    {
        std::string sym_file = prefix + ".sym";
        std::ofstream sym(sym_file.c_str());
        if (!sym.good()) return false;
        std::map<unsigned short, std::string>::const_iterator i;
        for (i = state.rev_symbols.begin(); i != state.rev_symbols.end(); i++)
        {
            sym << std::hex << i->first << std::dec << "\t" << i->second << std::endl;
        }
    }

    switch (options.output_mode)
    {
        case LC3AssembleOptions::OBJECT_FILE:
            return lc3_assemble_object_writer(prefix, state, ranges);
        case LC3AssembleOptions::BINARY_FILE:
            return lc3_assemble_binary_writer(prefix, state, ranges);
        case LC3AssembleOptions::HEXADECIMAL_FILE:
            return lc3_assemble_hexadecimal_writer(prefix, state, ranges);
        case LC3AssembleOptions::FULL_REPRESENTATION_FILE:
            return lc3_assemble_full_writer(prefix, state, ranges);
        default:
            return lc3_assemble_object_writer(prefix, state, ranges);
    }
}

/** process_plugin_info
  *
  * Process plugin statements adding appropriate plugins things as necessary
  */
void process_plugin_info(lc3_state& state, const LC3AssembleContext& context)
{
    // plugin filename=??? vector=x00 address=xFFFF interrupt=x1FF
    std::string line = context.line.substr(2);
    size_t index = line.find_first_of(" \t");
    std::string plugin_params = (index == std::string::npos) ? "" : context.line.substr(index + 1);

    std::map<std::string, std::string> params;
    parse_params(plugin_params, params);

    if (params.find("filename") == params.end())
    {
        THROW(LC3AssembleException(line, "No plugin filename given", PLUGIN_FAILED_TO_LOAD, context.lineno));
        return;
    }

    std::string filename = params.at("filename");
    params.erase("filename");

    try
    {
        lc3_install_plugin(state, filename, params);
    }
    catch (const LC3PluginException& e)
    {
        THROW(LC3AssembleException(line, e.what(), PLUGIN_FAILED_TO_LOAD, context.lineno));
    }
}

/** process_version_info
  *
  * Process version statements,
  */
void process_version_info(lc3_state& state, const LC3AssembleContext& context)
{
    // version <version>
    std::string line = context.line.substr(2);
    size_t index = line.find_first_of(" \t");
    std::string version = (index == std::string::npos) ? "" : line.substr(index + 1);
    trim(version);

    if (version == "1")
        lc3_set_version(state, 1);
    else if (version == "0")
        lc3_set_version(state, 0);
    else
        THROW(LC3AssembleException(line, version, INVALID_LC3_VERSION, context.lineno));
}

bool lc3_add_break(const std::string& symbol, const std::string& label = "", const std::string& condition = "1", int times = -1);
bool lc3_add_break(unsigned short addr, const std::string& label = "", const std::string& condition = "1", int times = -1);
bool lc3_add_watch(bool is_reg, unsigned short data, const std::string& condition, const std::string& label = "", int times = -1);
bool lc3_add_watch(const std::string& symbol, const std::string& condition, const std::string& label = "", int times = -1);

/** process_debug_info
  *
  * Process debug statements adding appropriate debugging things as necessary
  */
void process_debug_info(lc3_state& state, const debug_statement& statement, bool enable_debug_statements)
{
    // break[point] address=address name=label condition=1 times=-1
    // break[point] address name condition times
    // watch[point] target=??? name=label condition="0" times=-1
    // watch[point] target name condition times
    // black[box] target=???
    // subro[utine] address=address name=name num_params=0
    // subro[utine] address name num_params
    std::string type = statement.line.substr(0, 5);
    std::transform(type.begin(), type.end(), type.begin(), (int(*)(int))std::tolower);

    LC3AssembleContext dummy;
    dummy.address = 0;
    dummy.line = "";
    dummy.lineno = -1;
    dummy.state = &state;
    dummy.options.process_debug_comments = false;
    dummy.options.multiple_errors = true;
    dummy.options.warnings_as_errors = false;

    size_t index = statement.line.find_first_of(" \t");
    std::string debug_params = (index == std::string::npos) ? "" : statement.line.substr(index + 1);
    std::map<std::string, std::string> params;
    bool equal_sign_mode = statement.line.find('=') != std::string::npos;
    if (equal_sign_mode)
        parse_params(debug_params, params);

    // break[point] address=address name=label condition=1 times=-1
    // break[point] address name condition times
    // watch[point] target=??? name=label condition="0" times=-1
    // watch[point] target name condition times
    // blackbox address=address name=label condition=1
    // subroutine address=address name=label num_params=0
    if (type == std::string("break") && enable_debug_statements)
    {
        if (debug_params.empty())
        {
            if (statement.address != 0)
                lc3_add_break(state, statement.address);
            return;
        }
        // If using = sign mode
        if (equal_sign_mode)
        {
            unsigned short address;
            std::string name = params.find("name") == params.end() ? "" : params["name"];
            std::string condition = params.find("condition") == params.end() ? "1" : params["condition"];
            int times = params.find("times") == params.end() ? -1 : atoi(params["times"].c_str());

            // Address calculation
            if (params.find("address") == params.end())
            {
                if (statement.address == 0) return;
                address = statement.address;
            }
            else
            {
                address = get_sym_imm(params["address"], 16, dummy, true);
            }
            lc3_add_break(state, address, name, condition, times);
        }
        // If using function call mode
        else
        {
            unsigned short address;
            std::string name = "";
            std::string condition = "1";
            int times = -1;

            std::vector<std::string> pieces;
            tokenize(debug_params, pieces, " \t");
            if (pieces.size() > 4) pieces.resize(4);

            switch (pieces.size())
            {
            case 4:
                times = atoi(pieces[3].c_str());
                // fall through
            case 3:
                condition = pieces[2];
                // fall through
            case 2:
                name = pieces[1];
                // fall through
            case 1:
                address = get_sym_imm(pieces[0], 16, dummy, true);
                break;
            case 0:
                if (statement.address == 0) return;
                address = statement.address;
                break;
            default: // shouldn't happen
                return;
            }

            lc3_add_break(state, address, name, condition, times);
        }
    }
    else if (type == std::string("watch") && enable_debug_statements)
    {
        if (debug_params.empty())
        {
            if (statement.address != 0)
                lc3_add_watch(state, false, statement.address, "1");
            return;
        }
        // If using = sign mode
        if (equal_sign_mode)
        {
            bool is_reg;
            unsigned short data;
            std::string condition = params.find("condition") == params.end() ? "0" : params["condition"];
            std::string name = params.find("name") == params.end() ? "" : params["name"];
            int times = params.find("times") == params.end() ? -1 : atoi(params["times"].c_str());

            // Address calculation
            if (params.find("target") == params.end())
            {
                if (statement.address == 0) return;
                data = statement.address;
                is_reg = false;
            }
            else if (params["target"].size() == 2 && ((params["target"][0] == 'R' || params["target"][0] == 'r') &&
                     params["target"][1] >= '0' && params["target"][1] <= '7'))
            {
                is_reg = true;
                data = params["target"][1] - '0';
            }
            else
            {
                is_reg = false;
                data = get_sym_imm(params["target"], 16, dummy, true);
            }
            lc3_add_watch(state, is_reg, data, condition, name, times);
        }
        // If using function call mode
        else
        {
            bool is_reg;
            unsigned short data = 0;
            std::string condition = "0";
            std::string name = "";
            int times = -1;

            std::vector<std::string> pieces;
            tokenize(debug_params, pieces, " \t");
            if (pieces.size() > 4) pieces.resize(4);

            switch (pieces.size())
            {
            case 4:
                times = atoi(pieces[3].c_str());
                // fall through
            case 3:
                name = pieces[1];
                // fall through
            case 2:
                condition = pieces[2];
                // fall through
            case 1:
                if (pieces[0].size() == 2 && ((pieces[0][0] == 'R' || pieces[0][0] == 'r') &&
                                              pieces[0][1] >= '0' && pieces[0][1] <= '7'))
                {
                    data = pieces[0][1] - '0';
                    is_reg = true;
                }
                else
                {
                    data = get_sym_imm(pieces[0], 16, dummy, true);
                    is_reg = false;
                }
                break;
            case 0:
                if (statement.address == 0) return;
                data = statement.address;
                is_reg = false;
                condition = "1";
                break;
            default: // shouldn't happen
                return;
            }

            lc3_add_watch(state, is_reg, data, condition, name, times);
        }
    }
    else if (type == std::string("subro"))
    {
        if (debug_params.empty())
        {
            if (statement.address != 0)
                lc3_add_subroutine(state, statement.address);
            return;
        }
        // If using = sign mode
        if (equal_sign_mode)
        {
            unsigned short address;
            std::string name = params.find("name") == params.end() ? "" : params["name"];
            int num_params = params.find("num_params") == params.end() ? 0 : atoi(params["num_params"].c_str());
            std::string params_str = params.find("params") == params.end() ? "" : params["params"];
            std::vector<std::string> subr_params;
            tokenize(params_str, subr_params, ",");

            // Address calculation
            if (params.find("address") == params.end())
            {
                if (statement.address == 0) return;
                address = statement.address;
            }
            else
            {
                address = get_sym_imm(params["address"], 16, dummy, true);
            }
            if (params_str.empty())
                lc3_add_subroutine(state, address, name, num_params);
            else
                lc3_add_subroutine(state, address, name, subr_params);
        }
        // If using function call mode
        else
        {
            unsigned short address;
            std::string name = "";
            std::string params_str = "";

            std::vector<std::string> pieces;
            tokenize(debug_params, pieces, " \t");
            if (pieces.size() > 3) pieces.resize(3);

            switch (pieces.size())
            {
            case 3:
                params_str = pieces[2];
                // fall through
            case 2:
                name = pieces[1];
                // fall through
            case 1:
                address = get_sym_imm(pieces[0], 16, dummy, true);
                break;
            case 0:
                if (statement.address == 0) return;
                address = statement.address;
                break;
            default: // shouldn't happen
                return;
            }

            if (params_str.empty())
                lc3_add_subroutine(state, address, name, 0);
            else if (params_str[0] >= '0' && params_str[0] <= '9')
                lc3_add_subroutine(state, address, name, atoi(params_str.c_str()));
            else
            {
                std::vector<std::string> subro_params;
                tokenize(params_str, subro_params, ",");
                lc3_add_subroutine(state, address, name, subro_params);
            }
        }
    }
    else if (type == std::string("black") && enable_debug_statements)
    {
        if (debug_params.empty())
        {
            if (statement.address != 0)
                lc3_add_blackbox(state, statement.address);
            return;
        }
        // If using = sign mode
        if (equal_sign_mode)
        {
            unsigned short address;
            std::string name = params.find("name") == params.end() ? "" : params["name"];
            std::string condition = params.find("condition") == params.end() ? "1" : params["condition"];

            // Address calculation
            if (params.find("address") == params.end())
            {
                if (statement.address == 0) return;
                address = statement.address;
            }
            else
            {
                address = get_sym_imm(params["address"], 16, dummy, true);
            }
            lc3_add_blackbox(state, address, name, condition);
        }
        // If using function call mode
        else
        {
            unsigned short address;
            std::string name = "";
            std::string condition = "1";

            std::vector<std::string> pieces;
            tokenize(debug_params, pieces, " \t");
            if (pieces.size() > 3) pieces.resize(3);

            switch (pieces.size())
            {
            case 3:
                condition = pieces[2];
                // fall through
            case 2:
                name = pieces[1];
                // fall through
            case 1:
                address = get_sym_imm(pieces[0], 16, dummy, true);
                break;
            case 0:
                if (statement.address == 0) return;
                address = statement.address;
                break;
            default: // shouldn't happen
                return;
            }

            lc3_add_blackbox(state, address, name, condition);
        }
    }
}

/** parse_params
  *
  * Converts a string of key=value pairs into a map of string=>string
  */
void parse_params(const std::string& line, std::map<std::string, std::string>& params)
{
    std::vector<std::string> pieces;
    tokenize(line, pieces, " \t");
    for (unsigned int i = 0; i < pieces.size(); i++)
    {
        std::string piece = pieces[i];
        size_t index = piece.find('=');
        if (index == std::string::npos) continue;
        std::string key = piece.substr(0, index);
        std::string value = piece.substr(index+1);
        params[key]=value;
    }
}
