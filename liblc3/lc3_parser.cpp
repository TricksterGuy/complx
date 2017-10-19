#include "lc3_parser.hpp"
#include "lc3_plugin.hpp"
#include "lc3_symbol.hpp"
#include <algorithm>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cassert>
#include <cstring>
#include <cerrno>


/** remove_comments
  *
  * Removes all comments.
  */
void remove_comments(std::string& line, std::string& comment)
{
    int width = 0;
    /// @todo meh make this code better.
    // Handling lines with only comments
    bool word = false;
    for (size_t i = 0; i < line.size(); i++)
    {
        if (line[i] == ';' || line[i] == '\n' || line[i] == '\r')
            break;
        if (isalpha(line[i]))
            word = true;
        width++;
    }

    size_t comment_begin = line.find(';');
    if (comment_begin != std::string::npos)
        comment = line.substr(comment_begin);
    line.resize(word ? width : 0);
}

/** trim
  *
  * Removes all whitespace from beginning and end.
  */
void trim(std::string& line)
{
    size_t start = line.find_first_not_of("\t ");
    size_t end = line.find_last_not_of("\t ");

    if (start == end && start == std::string::npos)
        line = "";
    else if (start != std::string::npos)
        line = line.substr(start, end - start + 1);
}

/** process_str
  *
  * Unescapes and removes quotes from string
  */
std::string process_str(std::string str, const LC3AssembleContext& context)
{
    if (str[0] != '"' && str[0] != '\'')
    {
        THROW(LC3AssembleException("", str, MALFORMED_STRING, context.lineno));
    }
    if ((str[str.size() - 1] != '"' && str[str.size() - 1] != '\'') || str.size() == 1)
    {
        THROW(LC3AssembleException("", str, UNTERMINATED_STRING, context.lineno));
    }

    std::stringstream out;
    for (size_t i = 1; i < str.size() - 1; i++)
    {
        if (str[i] == '\\')
        {
            i++;
            // hmm
            if (i >= str.size() - 1)
            {
                THROWANDDO(LC3AssembleException("", str, MALFORMED_STRING, context.lineno), goto endstringprocessing);
            }
            char num = 0;
            char buf[4];
            switch(str[i])
            {
            case '\'':
                out << '\'';
                break;
            case '"':
                out << '"';
                break;
            case '\\':
                out << '\\';
                break;
            case 'a':
                out << '\a';
                break;
            case 'b':
                out << '\b';
                break;
            case 'f':
                out << '\f';
                break;
            case 'n':
                out << '\n';
                break;
            case 'r':
                out << '\r';
                break;
            case 't':
                out << '\t';
                break;
            case 'v':
                out << '\v';
                break;
            case 'x':
                i++;
                if (i < str.size() && isxdigit(str[i]))
                {
                    memset(buf, 0, 4);
                    buf[0] = str[i];

                    if (i + 1 < str.size() && isxdigit(str[i + 1]))
                    {
                        i++;
                        buf[1] = str[i];
                    }
                    num = strtol(buf, NULL, 16);
                }
                else
                {
                    THROW(LC3AssembleException("", str, MALFORMED_STRING, context.lineno));
                }
                out << num;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                memset(buf, 0, 4);
                buf[0] = str[i];

                if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '7')
                {
                    i++;
                    buf[1] = str[i];
                    if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '7')
                    {
                        i++;
                        buf[2] = str[i];
                    }
                }
                num = strtol(buf, NULL, 8);
                out << num;
                break;
            default:
                THROW(LC3AssembleException("", str, MALFORMED_STRING, context.lineno));
                break;
            }
        }
        else
        {
            out << str[i];
        }
    }

endstringprocessing:

    return out.str();
}

/** get_opcode
  *
  * Returns the opcode for the string -1 on error.
  */
int get_opcode(const std::string& opinstr, int& specialop, const LC3AssembleContext& context, bool opcode_required)
{
    std::string instr = opinstr;
    int opcode;
    specialop = 0;

    std::transform(instr.begin(), instr.end(), instr.begin(), (int(*)(int)) std::toupper);
    std::string ccflags = instr.size() >= 2 ? instr.substr(2) : "";
    std::string isbr = instr.substr(0, 2);


    if (instr == std::string("ADD"))
        opcode = ADD_INSTR;
    else if (instr == std::string("AND"))
        opcode = AND_INSTR;
    else if (instr == std::string("NOT"))
        opcode = NOT_INSTR;
    else if (isbr == std::string("BR") && ccflags.size() <= 3 && ccflags.find_first_not_of("NZP") == std::string::npos)
        opcode = BR_INSTR;
    else if (instr == std::string("JMP"))
        opcode = JMP_INSTR;
    else if (instr == std::string("RET"))
    {
        opcode = RET_INSTR;
        specialop = 1;
    }
    else if (instr == std::string("RTI"))
        opcode = RTI_INSTR;
    else if (instr == std::string("JSR"))
    {
        opcode = JSR_INSTR;
        specialop = 1;
    }
    else if (instr == std::string("JSRR"))
        opcode = JSRR_INSTR;
    else if (instr == std::string("LD"))
        opcode = LD_INSTR;
    else if (instr == std::string("LDI"))
        opcode = LDI_INSTR;
    else if (instr == std::string("LEA"))
        opcode = LEA_INSTR;
    else if (instr == std::string("ST"))
        opcode = ST_INSTR;
    else if (instr == std::string("STI"))
        opcode = STI_INSTR;
    else if (instr == std::string("LDR"))
        opcode = LDR_INSTR;
    else if (instr == std::string("STR"))
        opcode = STR_INSTR;
    else if (instr == std::string("TRAP") || instr == std::string("GETC") || instr == std::string("OUT") ||
             instr == std::string("PUTS") || instr == std::string("IN") || instr == std::string("PUTSP") ||
             instr == std::string("HALT"))
    {
        opcode = TRAP_INSTR;
        if (instr == std::string("GETC"))
            specialop = 0x20;
        else if (instr == std::string("OUT"))
            specialop = 0x21;
        else if (instr == std::string("PUTS"))
            specialop = 0x22;
        else if (instr == std::string("IN"))
            specialop = 0x23;
        else if (instr == std::string("PUTSP"))
            specialop = 0x24;
        else if (instr == std::string("HALT"))
            specialop = 0x25;
    }
    else if (context.state->instructionPlugin && instr == context.state->instructionPlugin->GetOpcode())
    {
        opcode = ERROR_INSTR;
    }
    else
    {
        // Hey test for trap special names
        for (std::map<unsigned char, TrapFunctionPlugin*>::const_iterator i = context.state->trapPlugins.begin(); i != context.state->trapPlugins.end(); ++i)
        {
            if (i->second != NULL && instr == i->second->GetTrapName())
            {
                // Good Good You are a trap...
                specialop = i->second->GetTrapVector();
                return TRAP_INSTR;
            }
        }
        if (opcode_required)
        {
            THROWANDDO(LC3AssembleException(context.line, instr, INVALID_INSTRUCTION, context.lineno), opcode = -1);
        }
        else
        {
            specialop = -1;
            return -1;
        }

    }

    return opcode;
}

/** get_register
  *
  * Parses a registers value
  */
short get_register(const std::string& regstr, const LC3AssembleContext& context)
{
    std::string reg = regstr;
    std::transform(reg.begin(), reg.end(), reg.begin(), (int(*)(int))std::toupper);
    int regnum = -1;

    if (reg[0] == 'R' && isdigit(reg[1]) && reg.size() == 2)
    {
        regnum = reg[1] - '0';
        if (regnum > 7) regnum = -1;
    }

    if (regnum == -1)
    {
        THROWANDDO(LC3AssembleException(context.line, regstr, INVALID_REGISTER, context.lineno), regnum = 0);
    }

    return regnum;
}

/** get_register_imm5
  *
  * Parses a register or a 5 bit immediate value
  */
short get_register_imm5(const std::string& regstr, bool& is_reg, const LC3AssembleContext& context)
{
    int regnum;
    is_reg = (regstr[0] == 'R' || regstr[0] == 'r') && regstr.size() == 2 && isdigit(regstr[1]);

    if (is_reg)
        regnum = get_register(regstr, context);
    else
        regnum = get_imm(regstr, 5, true, true, context);

    return regnum;
}

/** get_imm
  *
  * Parses a immediate value to a number of bits
  */
int get_imm(const std::string& value, int bits, bool is_num, bool is_signed, const LC3AssembleContext& context)
{
    std::string effValue = value;
    int base;
    bool negate;

    get_number(effValue, base, negate);

    char* errstr;
    errno = 0;
    long d = strtol(effValue.c_str(), &errstr, base);
    if (negate) d *= -1;
    // Failed for some reason they could have done 0xCAFEG or something
    if (*errstr)
    {
        THROW(LC3AssembleException(context.line, value, SYNTAX_ERROR, context.lineno));
    }
    if (errno != 0 || d > 0xFFFF) // Can only be ERANGE
    {
        std::vector<std::string> params;
        std::stringstream ebitstr;
        std::stringstream gbitstr;

        ebitstr << bits;
        gbitstr << (unsigned int)(log2(abs(d)) + 2);
        params.push_back(value);
        params.push_back(ebitstr.str());
        params.push_back(gbitstr.str());

        THROW(LC3AssembleException(context.line, params, is_num ? NUMBER_OVERFLOW : OFFSET_OVERFLOW, context.lineno));
    }

    if (bits != 16 || base == 10)
        d = check_value(d, bits, is_num, is_signed, context);

    return d;
}

/** get_symbol_offset
  *
  * Converts a symbol into an offset that is a certain number of bits
  */
int get_symbol_offset(const std::string& value, int bits, const LC3AssembleContext& context)
{
    int offset;
    int location = lc3_sym_lookup(*context.state, value);
    if (location == -1)
    {
        THROWANDDO(LC3AssembleException(context.line, value, UNDEFINED_SYMBOL, context.lineno), return -1);
    }
    offset = location - (context.address + 1);
    offset = check_value(offset, bits, false, true, context);

    return offset;
}

/** get_number
  *
  * Trims a string so that it contains only numbers also returns the base of the number
  */
void get_number(std::string& num, int& base, bool& negate)
{
    base = 10;
    negate = num[0] == '-';
    if (negate || num[0] == '+') num = num.substr(1);

    if (/*(num[0] == '0' && (num.size() > 1 && num[1] == 'x')) ||*/ num[0] == 'x')
    {
        num = num.substr(num.find('x')+1);
        base = 16;
    }
    else if (/*(num[0] == '0' && (num.size() > 1 && num[1] == 'b')) ||*/ num[0] == 'b')
    {
        num = num.substr(num.find('b')+1);
        base = 2;
    }
    else if (num[0] == '0' && num.size() > 1)
    {
        num = num.substr(1);
        base = 8;
    }
    else if (num[0] == '#')
        num = num.substr(1);
}

/** check_value
  *
  * Checks the number given to it to see if it will fit in a certain number of bits.
  */
short check_value(long value, int bits, bool is_num, bool signed_check, const LC3AssembleContext& context)
{
    int min = -(1 << (bits - 1));
    int max = (1 << (bits - 1)) - 1;

    if (signed_check && (value < min || value > max))
    {
        std::vector<std::string> params;
        std::stringstream ebitstr;
        std::stringstream gbitstr;

        ebitstr << bits;
        if (!value) value++;
        // Calculate number of bits remember this calculation is for 2's complement
        gbitstr << (unsigned int)(log2(abs(value)) + 2);
        params.push_back(context.tokens[context.tokens.size() - 1]);
        params.push_back(ebitstr.str());
        params.push_back(gbitstr.str());
        THROW(LC3AssembleException(context.line, params, is_num ? NUMBER_OVERFLOW : OFFSET_OVERFLOW, context.lineno));
    }
    else if (!signed_check && value < 0)
    {
        THROW(LC3AssembleException(context.line, context.tokens[context.tokens.size() - 1], INVALID_NUMBER, context.lineno));
    }
    else if (!signed_check && (value > (max + 1) << 1))
    {
        std::vector<std::string> params;
        std::stringstream ebitstr;
        std::stringstream gbitstr;

        ebitstr << bits;

        if (!value) value++;

        gbitstr << (unsigned int)(log2(abs(value)) + 1);
        params.push_back(context.tokens[context.tokens.size() - 1]);
        params.push_back(ebitstr.str());
        params.push_back(gbitstr.str());
        THROW(LC3AssembleException(context.line, params, is_num ? NUMBER_OVERFLOW : OFFSET_OVERFLOW, context.lineno));
    }

    return (short) (value & ((1 << bits) - 1));
}

/** get_cc_flags
  *
  * Gets the control code flags for BR.
  */
void get_cc_flags(const std::string& opcode, bool& n, bool& z, bool& p, const LC3AssembleContext& context)
{
    std::string flags = opcode.substr(2);
    std::transform(flags.begin(), flags.end(), flags.begin(), (int(*)(int)) std::toupper);

    n = z = p = false;

    if (flags == std::string("N"))
        n = true;
    else if (flags == std::string("NZ"))
        n = z = true;
    else if (flags == std::string("NP"))
        n = p = true;
    else if (flags == std::string("NZP") || flags == std::string(""))
        n = z = p = true;
    else if (flags == std::string("Z"))
        z = true;
    else if (flags == std::string("ZP"))
        z = p = true;
    else if (flags == std::string("P"))
        p = true;
    else
    {
        THROW(LC3AssembleException(context.line, flags, INVALID_FLAGS, context.lineno));
    }
}

/** get_offset
  *
  * Gets a x bit offset.
  */
short get_offset(const std::string& sym, int bits, const LC3AssembleContext& context, bool really_is_number)
{
    bool number = isdigit(sym[0]) || sym[0] == '#' || sym[0] == '-' || sym[0] == '+';
    int offset;
    // For strings such as b0001 xCAFE #1
    //if ((isalpha(sym[0]) && sym.size() > 1 && sym[0] == 'b' && sym[0] == 'x') || sym[0] == '#')
    if (sym.size() > 1 && (sym[0] == 'b' || sym[0] == 'x'))
    {
        number = true;
        if (sym[0] == 'x')
        {
            for (unsigned int i = 1; i < sym.size(); i++)
            {
                if (!isxdigit(sym[i])) number = false;
            }
        }
        else if (sym[0] == 'b')
        {
            for (unsigned int i = 1; i < sym.size(); i++)
            {
                if (sym[i] != '0' && sym[i] != '1') number = false;
            }
        }
    }

    if (number)
        offset = get_imm(sym, bits, really_is_number, true, context);
    else
        offset = get_symbol_offset(sym, bits, context);

    return offset;
}

/** get_sym_imm
  *
  * Gets a symbols value or an immediate value
  */
short get_sym_imm(const std::string& sym, int bits, const LC3AssembleContext& context, bool really_is_number)
{
    bool number = isdigit(sym[0]) || sym[0] == '#' || sym[0] == '-' || sym[0] == '+';
    int offset;
    // For strings such as b0001 xCAFE #1
    //if ((isalpha(sym[0]) && sym.size() > 1 && sym[0] == 'b' && sym[0] == 'x') || sym[0] == '#')
    if (sym.size() > 1 && (sym[0] == 'b' || sym[0] == 'x'))
    {
        number = true;
        if (sym[0] == 'x')
        {
            for (unsigned int i = 1; i < sym.size(); i++)
            {
                if (!isxdigit(sym[i])) number = false;
            }
        }
        else if (sym[0] == 'b')
        {
            for (unsigned int i = 1; i < sym.size(); i++)
            {
                if (sym[i] != '0' && sym[i] != '1') number = false;
            }
        }
    }

    if (number)
        offset = get_imm(sym, bits, really_is_number, true, context);
    else
    {
        int location = lc3_sym_lookup(*context.state, sym);
        if (location == -1)
        {
            THROW(LC3AssembleException(context.line, sym, UNDEFINED_SYMBOL, context.lineno));
        }
        offset = check_value(location, bits, false, true, context);
    }

    return offset;
}

/** get_fill_value
  *
  * Gets the value for the .fill statement
  */
short get_fill_value(const std::string& value, const LC3AssembleContext& context)
{
    if (value[0] == '\'')
    {
        std::string str = process_str(value, context);
        if (str.size() != 1)
        {
            THROWANDDO(LC3AssembleException(context.line, value, INVALID_CHARACTER, context.lineno), return str[0]);
        }
        else
            return str[0];
    }
    else if (value[0] == '\"')
    {
        THROWANDDO(LC3AssembleException(context.line, value, INVALID_CHARACTER, context.lineno), return 0);
    }
    else
    {
        return get_sym_imm(value, 16, context, true);
    }
}

bool is_register_or_imm(const std::string& token)
{
    bool is_reg = (token[0] == 'R' || token[0] == 'r') && token.size() == 2 && isdigit(token[1]);

    if (is_reg)
        return true;
    else
    {
        bool number = isdigit(token[0]) || token[0] == '#' || token[0] == '-' || token[0] == '+';
        if (!number) return false;
        // For strings such as b0001 xCAFE #1
        //if ((isalpha(token[0]) && token.size() > 1 && token[0] == 'b' && token[0] == 'x') || token[0] == '#')
        if (token.size() > 1 && (token[0] == 'b' || token[0] == 'x'))
        {
            number = true;
            if (token[0] == 'x')
            {
                for (unsigned int i = 1; i < token.size(); i++)
                {
                    if (!isxdigit(token[i])) return false;
                }
            }
            else if (token[0] == 'b')
            {
                for (unsigned int i = 1; i < token.size(); i++)
                {
                    if (token[i] != '0' && token[i] != '1') return false;
                }
            }
        }
    }

    return true;
}

/** tokenize
  *
  * Tokenizes a string credit to
  * http://oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
  */
void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        tokens.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

bool is_hex(const std::string& str)
{
    if (str.size() < 2 || str[0] != 'x')
        return false;

    for (unsigned int i = 1; i < str.size(); i++)
    {
        if (!isxdigit(str[i]))
            return false;
    }
    return true;
}

bool is_binary(const std::string& str)
{
    if (str.size() < 2 || str[0] != 'b')
        return false;

    for (unsigned int i = 1; i < str.size(); i++)
    {
        if (!(str[i] == '0' || str[i] == '1'))
            return false;
    }
    return true;
}
