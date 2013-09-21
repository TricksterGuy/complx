#ifndef LC3_ASSEMBLE_HPP
#define LC3_ASSEMBLE_HPP

#include "lc3.hpp"
#include <string>
#include <vector>
#include <stdexcept>

struct code_range;

/// @todo reduce the number of parameters!
unsigned short lc3_assemble_one(lc3_state& state, unsigned short address, const std::string& line, int lineno = -1, bool multiple = false, bool werror = false, bool warn = false, bool disable_plugins = false);
void lc3_assemble(lc3_state& state, const std::string& filename, std::vector<code_range>& ranges, bool multiple = true, bool werror = false, bool debug = true, bool warn = false, bool disable_plugins = false);
void lc3_assemble(lc3_state& state, const std::string& filename, bool multiple = true, bool werror = false, bool debug = true, bool warn = false, bool disable_plugins = false);
bool lc3_assemble(const std::string& filename, const std::string& output_prefix = "", bool multiple = true, bool werror = false, bool debug = true, bool warn = false, bool disable_plugins = false);

class LC3AssembleException
{
    public:

        LC3AssembleException(const std::string& line, const std::vector<std::string>& params, int id = 0 /* Unknown error */, int lineno = -1) throw();
        LC3AssembleException(const std::string& line, const std::string& param, int id = 0, int lineno = -1) throw();
        ~LC3AssembleException() throw();
        std::string what() const throw();
    private:
        std::string line;
        std::vector<std::string> params;
        int lineno;
        int id;
        mutable char what_str[1024];
};

#endif
