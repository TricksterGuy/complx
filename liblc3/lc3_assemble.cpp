#include "lc3_assemble.hpp"
#include "lc3_parser.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <cerrno>

#ifdef LINUX
#include <netinet/in.h>
#include <arpa/inet.h>
#else
#ifdef WINDOWS
#include <winsock2.h>
#endif
#endif

// For non line comments.
struct code_line
{
    std::string line;
    unsigned int location;

    code_line(const std::string& line, unsigned int location)
    {
        this->line = line;
        this->location = location;
    }
};

// For comments with debugging information
struct debug_statement
{
    std::string line;
    unsigned int location;
    unsigned short address;

    debug_statement(const std::string& line, unsigned int location, unsigned short address)
    {
        this->line = line;
        this->location = location;
        this->address = address;
    }
};

unsigned short lc3_assemble_one(lc3_state& state, LC3AssembleContext& context);

void process_debug_info(lc3_state& state, const debug_statement& statement);
void process_plugin_info(lc3_state& state, const LC3AssembleContext& context);
void parse_params(const std::string& line, std::map<std::string, std::string>& params);

/** LC3AssembleException
  *
  * Generates a new assemble exception.
  */
LC3AssembleException::LC3AssembleException(const std::string& line, const std::vector<std::string>& params, int id, int lineno) throw()
{
    this->line = line;
    this->id = id;
    this->lineno = lineno;
    this->params = params;
}

/** LC3AssembleException
  *
  * Generates a new assemble exception.
  */
LC3AssembleException::LC3AssembleException(const std::string& line, const std::string& param, int id, int lineno) throw()
{
    this->line = line;
    this->id = id;
    this->lineno = lineno;
    params.push_back(param);
}

/** ~LC3AssembleException
  *
  * Destructor
  */
LC3AssembleException::~LC3AssembleException() throw()
{

}

/** what
  *
  * Error message.
  */
std::string LC3AssembleException::what() const throw()
{
    std::string ret;
    if (lineno != -1)
    {
        switch(id)
        {
            case SYNTAX_ERROR:
                snprintf(what_str, 1023, "Syntax Error on line %d: %s", lineno, line.c_str());
                break;
            case ORIG_MATCHUP:
                snprintf(what_str, 1023, "No matching .end found on line %d for %s", lineno, line.c_str());
                break;
            case ORIG_OVERLAP:
                snprintf(what_str, 1023, "Code sections %s and %s overlap", params[0].c_str(), params[1].c_str());
                break;
            case STRAY_END:
                snprintf(what_str, 1023, "No matching .orig found for .end on line %d", lineno);
                break;
            case STRAY_DATA:
                snprintf(what_str, 1023, "Stray data found on line %d: %s", lineno, line.c_str());
                break;
            case UNDEFINED_SYMBOL:
                snprintf(what_str, 1023, "Undefined symbol %s found on line %d", params[0].c_str(), lineno);
                break;
            case DUPLICATE_SYMBOL:
                snprintf(what_str, 1023, "Duplicate symbol %s found on line %d", params[0].c_str(), lineno);
                break;
            case MULTIPLE_SYMBOL:
                snprintf(what_str, 1023, "Multiple symbol %s and %s found for address 0x%s on line %d", params[0].c_str(), params[1].c_str(), params[2].c_str(), lineno);
                break;
            case INVALID_SYMBOL:
                snprintf(what_str, 1023, "Invalid symbol %s found on line %d", params[0].c_str(), lineno);
                break;
            case INVALID_REGISTER:
                snprintf(what_str, 1023, "Invalid register %s found on line %d", params[0].c_str(), lineno);
                break;
            case INVALID_INSTRUCTION:
                snprintf(what_str, 1023, "Invalid instruction %s found on line %d", params[0].c_str(), lineno);
                break;
            case INVALID_DIRECTIVE:
                snprintf(what_str, 1023, "Invalid assembler directive %s found on line %d", params[0].c_str(), lineno);
                break;
            case INVALID_FLAGS:
                snprintf(what_str, 1023, "Invalid condition code flags %s found on line %d", params[0].c_str(), lineno);
                break;
            case INVALID_CHARACTER:
                snprintf(what_str, 1023, "Invalid character constant found on line %d: %s", lineno, params[0].c_str());
                break;
            case INVALID_NUMBER:
                snprintf(what_str, 1023, "Found signed number expecting unsigned number on line %d: %s", lineno, params[0].c_str());
                break;
            case NUMBER_OVERFLOW:
                snprintf(what_str, 1023, "%s is too big for an immediate value expected %s bits got %s bits found on line %d", params[0].c_str(), params[1].c_str(), params[2].c_str(), lineno);
                break;
            case OFFSET_OVERFLOW:
                snprintf(what_str, 1023, "%s is too far away for an offset expected %s bits got %s bits found on line %d", params[0].c_str(), params[1].c_str(), params[2].c_str(), lineno);
                break;
            case MEMORY_OVERFLOW:
                snprintf(what_str, 1023, "Can't add by %s found on line %d", params[0].c_str(), lineno);
                break;
            case SCAN_OVERFLOW:
                snprintf(what_str, 1023, "I'm at the end of memory (xFFFF) and I refuse to wrap around! found on line %d", lineno);
                break;
            case FILE_ERROR:
                snprintf(what_str, 1023, "Could not open %s for reading\nAre you sure the file is in your current working directory?\n", params[0].c_str());
                break;
            case UNTERMINATED_STRING:
                snprintf(what_str, 1023, "Unterminated string on line %d: %s", lineno, params[0].c_str());
                break;
            case MALFORMED_STRING:
                snprintf(what_str, 1023, "Malformed string on line %d: %s", lineno, params[0].c_str());
                break;
            case EXTRA_INPUT:
                snprintf(what_str, 1023, "Extra input found at end of line %d: %s", lineno, line.c_str());
                break;
            case PLUGIN_FAILED_TO_LOAD:
                snprintf(what_str, 1023, "Plugin %s failed to load at line %d", params[0].c_str(), lineno);
                break;
            case UNKNOWN_ERROR:
            default:
                snprintf(what_str, 1023, "Unknown error found on line %d: %s", lineno, line.c_str());
                break;
        }
    }
    else
    {
        switch(id)
        {
            case SYNTAX_ERROR:
                snprintf(what_str, 1023, "Syntax Error %s", line.c_str());
                break;
            case ORIG_MATCHUP:
                snprintf(what_str, 1023, "No matching .end found for %s", line.c_str());
                break;
            case ORIG_OVERLAP:
                snprintf(what_str, 1023, "Code sections %s and %s overlap", params[0].c_str(), params[1].c_str());
                break;
            case STRAY_END:
                snprintf(what_str, 1023, "No matching .orig found for .end");
                break;
            case STRAY_DATA:
                snprintf(what_str, 1023, "Stray data %s", line.c_str());
                break;
            case UNDEFINED_SYMBOL:
                snprintf(what_str, 1023, "Undefined symbol %s", params[0].c_str());
                break;
            case DUPLICATE_SYMBOL:
                snprintf(what_str, 1023, "Duplicate symbol %s", params[0].c_str());
                break;
            case MULTIPLE_SYMBOL:
                snprintf(what_str, 1023, "Multiple symbol %s and %s found for address 0x%s", params[0].c_str(), params[1].c_str(), params[2].c_str());
                break;
            case INVALID_DIRECTIVE:
                snprintf(what_str, 1023, "Invalid assembler directive %s", params[0].c_str());
                break;
            case INVALID_SYMBOL:
                snprintf(what_str, 1023, "Invalid symbol %s", params[0].c_str());
                break;
            case INVALID_REGISTER:
                snprintf(what_str, 1023, "Invalid register %s", params[0].c_str());
                break;
            case INVALID_INSTRUCTION:
                snprintf(what_str, 1023, "Invalid instruction %s", params[0].c_str());
                break;
            case INVALID_FLAGS:
                snprintf(what_str, 1023, "Invalid condition code flags %s", params[0].c_str());
                break;
            case INVALID_CHARACTER:
                snprintf(what_str, 1023, "Invalid character constant %s", params[0].c_str());
                break;
            case INVALID_NUMBER:
                snprintf(what_str, 1023, "Found signed number expecting unsigned number %s", params[0].c_str());
                break;
            case NUMBER_OVERFLOW:
                snprintf(what_str, 1023, "%s is too big for an immediate value expected %s bits got %s bits", params[0].c_str(), params[1].c_str(), params[2].c_str());
                break;
            case OFFSET_OVERFLOW:
                snprintf(what_str, 1023, "%s is too far away for an offset expected %s bits got %s bits", params[0].c_str(), params[1].c_str(), params[2].c_str());
                break;
            case MEMORY_OVERFLOW:
                snprintf(what_str, 1023, "Can't add by %s", params[0].c_str());
                break;
            case SCAN_OVERFLOW:
                snprintf(what_str, 1023, "I'm at the end of memory (xFFFF) and I refuse to wrap around! found on line %d", lineno);
                break;
            case FILE_ERROR:
                snprintf(what_str, 1023, "Could not open %s for reading\nAre you sure the file is in your current working directory?", params[0].c_str());
                break;
            case UNTERMINATED_STRING:
                snprintf(what_str, 1023, "Unterminated string %s", params[0].c_str());
                break;
            case MALFORMED_STRING:
                snprintf(what_str, 1023, "Malformed string %s on line %d", params[0].c_str(), lineno);
                break;
            case EXTRA_INPUT:
                snprintf(what_str, 1023, "%s Extra input found at end of line %d", line.c_str(), lineno);
                break;
            case PLUGIN_FAILED_TO_LOAD:
                snprintf(what_str, 1023, "Plugin %s failed to load", params[0].c_str());
                break;
            case UNKNOWN_ERROR:
            default:
                snprintf(what_str, 1023, "Unknown error: %s", line.c_str());
                break;
        }
    }
    ret = what_str;
    return ret;
}

/** lc3_assemble_one
  *
  * Assembles one instruction.
  */
unsigned short lc3_assemble_one(lc3_state& state, unsigned short address, const std::string& line, int lineno, bool multiple, bool werror, bool warn, bool disable_plugins)
{
    //std::vector<std::string> tokens;
    //tokenize(line, tokens, " \t,");

    LC3AssembleContext context;
    context.line = line;
    //context.tokens = tokens;
    context.lineno = lineno;
    context.state = &state;
    context.address = address;
    context.multiple = multiple;
    context.werror = werror;
    context.warn = warn;
    context.disable_plugins = disable_plugins;

    unsigned short ret = lc3_assemble_one(state, context);

    if (context.multiple && !context.exceptions.empty())
        throw context.exceptions;

    return ret;
}

/** lc3_assemble_one
  *
  * Assembles one instruction given in the LC3AssembleContext
  */
unsigned short lc3_assemble_one(lc3_state& state, LC3AssembleContext& context)
{

    /*LC3AssembleContext context;
    context.line = line;
    context.tokens = tokens;
    context.lineno = lineno;
    context.state = state;
    context.address = address;*/

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

    /*int specialop;
    int opcode = get_opcode(tokens[0], specialop, context, false);
    if (opcode == -1 && specialop == -1)
    {
        tokens.erase(tokens.begin());
        opcode = get_opcode(tokens[0], specialop, context);
    }*/

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
            sr2_imm = get_imm(tokens[2], 6, true, true, context);
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

/** lc3_assemble
  *
  * Assembles a file pointed to by filename to the lc3_state given
  */
void lc3_assemble(lc3_state& state, const std::string& filename, bool multiple, bool werror, bool debug, bool warn, bool disable_plugins)
{
    std::vector<code_range> ranges;
    lc3_assemble(state, filename, ranges, multiple, werror, debug, warn, disable_plugins);
}

/** lc3_assemble
  *
  * Assembles a file pointed to by filename to the lc3_state given
  * @note this overload is not meant to be used externally use the version without the vector.
  */
void lc3_assemble(lc3_state& state, const std::string& filename, std::vector<code_range>& ranges, bool multiple, bool werror, bool debug, bool warn, bool disable_plugins)
{
    std::ifstream file(filename.c_str());
    if (!file.good())
        throw LC3AssembleException("", filename, FILE_ERROR);

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
    context.multiple = multiple;
    context.werror = werror;
    context.warn = warn;
    context.debug = debug;
    context.disable_plugins = disable_plugins;

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
        if (comment.size() > 2 && comment.substr(1, 7) == std::string("@plugin") && !context.disable_plugins)
        {
            process_plugin_info(state, context);
        }
        else if (context.debug && comment.size() > 2 && comment[1] == '@')
        {
            debugging.push_back(debug_statement(comment.substr(2), context.lineno, context.address));
        }
        else if (!comment.empty() && in_orig)
        {
            comments << comment;
            comments << "\n";
        }

        if (line.empty()) continue;

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
            int op = get_opcode(tokens[0], specialop, context, false);
            if (!context.multiple)
                assert(!(op == -1 && specialop == -1));
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

                for (int i = 0; i < size - 1; i++)
                    state.mem[context.address + i] = processed[i];
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
        process_debug_info(state, statement);
    }


    if (context.multiple && !context.exceptions.empty())
    {
        throw context.exceptions;
    }
}

/** lc3_assemble
  *
  * Assembles a file to an .obj and .sym file
  */
bool lc3_assemble(const std::string& filename, const std::string& output_prefix, bool multiple, bool werror, bool debug, bool warn, bool disable_plugins)
{
    lc3_state state;
    lc3_init(state);
    std::vector<code_range> ranges;
    lc3_assemble(state, filename, ranges, multiple, werror, debug, warn, disable_plugins);
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

    std::string obj_file = prefix + ".obj";
    std::ofstream obj(obj_file.c_str(), std::ios::binary);
    if (!obj.good()) return false;

    for (unsigned int i = 0; i < ranges.size(); i++)
    {
        code_range range = ranges[i];
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

    if (!lc3_install_plugin(state, params["filename"], params))
        THROW(LC3AssembleException(line, params["filename"], PLUGIN_FAILED_TO_LOAD, context.lineno));

}

bool lc3_add_break(const std::string& symbol, const std::string& label = "", const std::string& condition = "1", int times = -1);
bool lc3_add_break(unsigned short addr, const std::string& label = "", const std::string& condition = "1", int times = -1);
bool lc3_add_watch(bool is_reg, unsigned short data, const std::string& condition, const std::string& label = "", int times = -1);
bool lc3_add_watch(const std::string& symbol, const std::string& condition, const std::string& label = "", int times = -1);

/** process_debug_info
  *
  * Process debug statements adding appropriate debugging things as necessary
  */
void process_debug_info(lc3_state& state, const debug_statement& statement)
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
    dummy.debug = false;
    dummy.line = "";
    dummy.lineno = -1;
    dummy.multiple = true;
    dummy.state = &state;
    dummy.werror = false;

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
    if (type == std::string("break"))
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
                case 3:
                    condition = pieces[2];
                case 2:
                    name = pieces[1];
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
    else if (type == std::string("watch"))
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
                case 3:
                    name = pieces[1];
                case 2:
                    condition = pieces[2];
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
            std::vector<std::string> params;

            std::vector<std::string> pieces;
            tokenize(debug_params, pieces, " \t");
            if (pieces.size() > 3) pieces.resize(3);

            switch (pieces.size())
            {
                case 3:
                    params_str = pieces[2];
                case 2:
                    name = pieces[1];
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
                tokenize(params_str, params, ",");
                lc3_add_subroutine(state, address, name, params);
            }
        }
    }
    else if (type == std::string("black"))
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
                case 2:
                    name = pieces[1];
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
