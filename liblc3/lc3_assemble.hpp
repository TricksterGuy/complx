#ifndef LC3_ASSEMBLE_HPP
#define LC3_ASSEMBLE_HPP

#include "lc3.hpp"
#include "lc3_parser.hpp"
#include <string>
#include <vector>
#include <stdexcept>

// For .orig statements
struct code_range
{
    code_range(unsigned int loc, unsigned int sz) : location(loc), size(sz) {}

    bool operator<(const code_range& o) const
    {
        return location < o.location;
    }

    bool operator>(const code_range& o) const
    {
        return location > o.location;
    }

    unsigned int location;
    unsigned int size;
};

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
    INVALID_BYTES, // Non ASCII bytes found in input.
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

class LC3AssembleException
{
public:
    LC3AssembleException(const std::string& line_str, const std::vector<std::string>& args, int errorid = UNKNOWN_ERROR, int linenum = -1) throw() :
        line(line_str), params(args), id(errorid), lineno(linenum) {}
    LC3AssembleException(const std::string& line_str, const std::string& param, int errorid = UNKNOWN_ERROR, int linenum = -1) throw() :
        line(line_str), params(1, param), id(errorid), lineno(linenum) {}
    ~LC3AssembleException() throw() {}
    std::string what() const throw();
    int get_id() const
    {
        return id;
    }
private:
    std::string line;
    std::vector<std::string> params;
    int id;
    int lineno;
    mutable char what_str[1024];
};

struct LC3AssembleOptions
{
    bool multiple_errors = true;
    bool warnings_as_errors = false;
    bool enable_warnings = false;
    bool disable_plugins = false;
    bool process_debug_comments = true;
};

struct LC3AssembleContext
{
    std::vector<std::string> tokens;
    mutable std::vector<LC3AssembleException> exceptions;
    std::string line;
    lc3_state* state;
    int lineno;
    unsigned short address;
    LC3AssembleOptions options;
};

unsigned short lc3_assemble_one(lc3_state& state, unsigned short address, const std::string& line, int lineno = -1, const LC3AssembleOptions& options = LC3AssembleOptions());
void lc3_assemble(lc3_state& state, const std::string& filename, std::vector<code_range>& ranges, const LC3AssembleOptions& options = LC3AssembleOptions());
void lc3_assemble(lc3_state& state, const std::string& filename, const LC3AssembleOptions& options = LC3AssembleOptions());
bool lc3_assemble(const std::string& filename, const std::string& output_prefix = "", const LC3AssembleOptions& options = LC3AssembleOptions());
// test only
void lc3_assemble(lc3_state& state, std::istream& file, std::vector<code_range>& ranges, const LC3AssembleOptions& options);

#endif
