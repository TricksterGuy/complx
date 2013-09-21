#ifndef LC3_PARSER_HPP
#define LC3_PARSER_HPP

#include "lc3.hpp"
#include <string>
#include <vector>

enum LC3AssembleExceptionTypes
{
    UNKNOWN_ERROR = 0,
    SYNTAX_ERROR,
    ORIG_MATCHUP,
    ORIG_OVERLAP,
    STRAY_END,
    STRAY_DATA,
    UNDEFINED_SYMBOL,
    DUPLICATE_SYMBOL, // Two of the same symbol was used
    MULTIPLE_SYMBOL, // Multiple symbols was defined for an address.
    INVALID_SYMBOL,
    INVALID_REGISTER,
    INVALID_INSTRUCTION,
    INVALID_DIRECTIVE,
    INVALID_FLAGS,
    INVALID_CHARACTER,
    INVALID_NUMBER, // Signed number given to unsigned value.
    NUMBER_OVERFLOW,
    OFFSET_OVERFLOW,
    MEMORY_OVERFLOW,
    SCAN_OVERFLOW, // Like MEMORY OVERFLOW except you were at xFFFF and the next address would be x0000.
    FILE_ERROR,
    UNTERMINATED_STRING,
    MALFORMED_STRING,
    EXTRA_INPUT,
    PLUGIN_FAILED_TO_LOAD,
};


#define THROW(exception) do \
{ \
    if (context.multiple) \
    { \
        context.exceptions.push_back(exception);\
    } \
    else \
    { \
        throw exception;\
    } \
} while(0) \

#define WARN(exception) do \
{ \
    if (context.werror) \
    { \
        THROW(exception); \
    } \
    else if (context.warn) \
    { \
        printf("Warning: %s\n", exception.what().c_str());\
    } \
} while(0) \

#define THROWANDDO(exception, do_this) do \
{ \
    if (context.multiple) \
    { \
        context.exceptions.push_back(exception);\
        do_this; \
    } \
    else \
    { \
        throw exception;\
    } \
} while(0) \

class LC3AssembleException;

struct LC3AssembleContext
{
    std::vector<std::string> tokens;
    mutable std::vector<LC3AssembleException> exceptions;
    std::string line;
    lc3_state* state;
    int lineno;
    unsigned short address;
    bool multiple;
    bool werror;
    bool warn;
    bool debug;
    bool disable_plugins;
};

/* Removes leading and trailing whitespace*/
void trim(std::string& line);
/* Removes comments from a string*/
void remove_comments(std::string& line, std::string& comment);
/* Processes and unescapes a string*/
std::string process_str(std::string str, const LC3AssembleContext&);
/* Gets the value from a .fill directive*/
short get_fill_value(const std::string& value, const LC3AssembleContext& context);
/* Gets the opcode from a line*/
int get_opcode(const std::string&, int&, const LC3AssembleContext&, bool opcode = true);
/* Gets a register R0-7*/
short get_register(const std::string&, const LC3AssembleContext&);
/* Gets a register or a 5 bit immediate value*/
short get_register_imm5(const std::string&, bool&, const LC3AssembleContext&);
/* Gets the condition code flags*/
void get_cc_flags(const std::string&, bool&, bool&, bool&, const LC3AssembleContext&);
/* Gets a PCOffset*/
short get_offset(const std::string&, int, const LC3AssembleContext&, bool really_is_number = false);
/* Gets a symbol value or immediate value*/
short get_sym_imm(const std::string& sym, int bits, const LC3AssembleContext& context, bool really_is_number);
/* Gets an immediate value*/
int get_imm(const std::string&, int, bool, bool, const LC3AssembleContext&);
/* Reads a number with a specified number of bits in any base*/
void get_number(std::string&, int&, bool&);
/* Checks if a number fits in a specified number of bits*/
short check_value(long, int, bool, bool, const LC3AssembleContext&);
/* Tokenizes a string */
void tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");
/* Checks if token is a register or immediate value */
bool is_register_or_imm(const std::string& token);

#endif
