#ifndef LC3_ASSEMBLE_HPP
#define LC3_ASSEMBLE_HPP

#include "lc3.hpp"
#include "lc3_parser.hpp"
#include <string>
#include <vector>
#include <stdexcept>

/** Code ranges present in the .asm file when assembled.
  * Each .orig .end pair will translate into an object of this type.
  */
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

/** Enumeration of possible problems that may occur in assembling a file */
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

/** Exception class for assembler errors */
class LC3AssembleException
{
public:
    /** Constructor
      *
      * @param line_str line causing the error.
      * @param args Arguments for formatting.
      * @param errorid Error ID. @see LC3AssembleExceptionTypes
      * @param linenum Line number.
      */
    LC3AssembleException(const std::string& line_str, const std::vector<std::string>& args, int errorid = UNKNOWN_ERROR, int linenum = -1) throw() :
        line(line_str), params(args), id(errorid), lineno(linenum) {}
    /** Constructor
      *
      * @param line_str line causing the error.
      * @param param Parameter for formatting.
      * @param errorid Error ID. @see LC3AssembleExceptionTypes
      * @param linenum Line number.
      */
    LC3AssembleException(const std::string& line_str, const std::string& param, int errorid = UNKNOWN_ERROR, int linenum = -1) throw() :
        line(line_str), params(1, param), id(errorid), lineno(linenum) {}
    ~LC3AssembleException() throw() {}
    std::string what() const throw();
    int get_id() const {return id;}
private:
    std::string line;
    std::vector<std::string> params;
    int id;
    int lineno;
    mutable char what_str[1024];
};

/** Options for the lc3_assemble functions */
struct LC3AssembleOptions
{
    bool multiple_errors = true;
    bool warnings_as_errors = false;
    bool enable_warnings = false;
    bool disable_plugins = false;
    bool process_debug_comments = true;
};

/** Contextual information pass among assemble/parser functions */
struct LC3AssembleContext
{
    std::vector<std::string> tokens;
    mutable std::vector<LC3AssembleException> exceptions;
    std::string line = "";
    lc3_state* state = nullptr;
    int lineno = -1;
    unsigned short address = 0x0000;
    LC3AssembleOptions options;
};

/** lc3_assemble_one
  *
  * Assembles a single instruction.
  * @param state LC3State object.
  * @param address Address of the instruction.
  * @param line Text to assemble.
  * @param lineno Line number.
  * @param options Assembler options.
  * @return The assembled data.
  * @throw LC3AssembleException on error
  */
unsigned short lc3_assemble_one(lc3_state& state, unsigned short address, const std::string& line, int lineno = -1, const LC3AssembleOptions& options = LC3AssembleOptions());
/** lc3_assemble
  *
  * Assembles a file into the LC3State object given.
  * @param state LC3State object.
  * @param filename Path to file to assemble.
  * @param ranges Output parameter for list of .orig/.end pairs.
  * @param options Assembler options.
  * @throw LC3AssembleException on error
  */
void lc3_assemble(lc3_state& state, const std::string& filename, std::vector<code_range>& ranges, const LC3AssembleOptions& options = LC3AssembleOptions());
/** lc3_assemble
  *
  * Assembles a file into the LC3State object given.
  * @param state LC3State object.
  * @param filename Path to file to assemble.
  * @param ranges Output parameter for list of .orig/.end pairs.
  * @param options Assembler options.
  * @throw LC3AssembleException on error
  */
void lc3_assemble(lc3_state& state, const std::string& filename, const LC3AssembleOptions& options = LC3AssembleOptions());
/** lc3_assemble
  *
  * Assembles a file into an object and sym file.
  * @param filename Path to file to assemble.
  * @param output_prefix Filename without extension to save the object and sym file.
  * @param options Assembler options.
  * @return True on success.
  * @throw LC3AssembleException on error
  */
bool lc3_assemble(const std::string& filename, const std::string& output_prefix = "", const LC3AssembleOptions& options = LC3AssembleOptions());

/** lc3_assemble
  *
  * Assembles a file into the LC3State object given (Test only)
  * @param state LC3State object.
  * @param file Stream containing the assembly code to assemble.
  * @param ranges Output parameter for list of .orig/.end pairs.
  * @param options Assembler options.
  * @throw LC3AssembleException on error
  */
void lc3_assemble(lc3_state& state, std::istream& file, std::vector<code_range>& ranges, const LC3AssembleOptions& options);

#endif
