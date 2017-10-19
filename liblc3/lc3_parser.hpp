#ifndef LC3_PARSER_HPP
#define LC3_PARSER_HPP

#include "lc3.hpp"
#include "lc3_assemble.hpp"
#include <string>
#include <vector>

class LC3AssembleContext;

#define THROW(exception) do \
{ \
    if (context.options.multiple_errors) \
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
    if (context.options.warnings_as_errors) \
    { \
        THROW(exception); \
    } \
    else if (context.options.enable_warnings) \
    { \
        printf("Warning: %s\n", exception.what().c_str());\
    } \
} while(0) \

#define THROWANDDO(exception, do_this) do \
{ \
    if (context.options.multiple_errors) \
    { \
        context.exceptions.push_back(exception);\
        do_this; \
    } \
    else \
    { \
        throw exception;\
    } \
} while(0) \

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
/* Checks if token is a hexadecimal literal */
bool is_hex(const std::string& token);
/* Checks if token is a binary literal */
bool is_binary(const std::string& token);

#endif
