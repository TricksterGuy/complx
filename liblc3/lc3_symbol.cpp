#include "lc3_symbol.hpp"
#include <sstream>

/** lc3_load_sym
  *
  * Loads the given symbol file (liblc3 format) into the program.
  */
int lc3_load_sym(lc3_state& state, std::istream& file)
{
    // Once upon a time there was the simplc3 tools
    // It came with an assembler called lc3as
    // The symbol file format was HORRIBLY annoying
    // Symbol Table
    // Scope level 0:
    //	Symbol Name       Page Address
    //	----------------  ------------
    //	name               address
    // comment above is dedicated to how annoying it is...
    std::string line;

    if (!file.good()) return -1;

    getline(file, line);
    // Can assume there is at least one symbol.
    // If there wasn't then there wouldn't be a point to a symbol file.
    while (!line.empty())
    {
        std::string sym_name;
        unsigned short location;

        std::stringstream ss(line);
        ss >> sym_name;
        ss >> std::hex >> location;

        state.symbols[sym_name] = location;
        state.rev_symbols[location] = sym_name;
        if (!file.good()) return -1;
        getline(file, line);
    }

    return 0;
}

/** lc3_sym_lookup
  *
  * Looks up the given symbol in the symbol table
  * @return -1 if the symbol was not found else the value.
  */
int lc3_sym_lookup(lc3_state& state, const std::string& symbol)
{
    std::map<std::string, unsigned short>::iterator i = state.symbols.find(symbol);

    if (i == state.symbols.end()) return -1;

    return i->second;
}

/** lc3_sym_rev_lookup
  *
  * Looks at the given memory address for a symbol.
  * @return the empty string if no symbol else the symbol name.
  */
const std::string lc3_sym_rev_lookup(lc3_state& state, unsigned short addr)
{
    std::map<unsigned short, std::string>::iterator i = state.rev_symbols.find(addr);

    if (i == state.rev_symbols.end()) return "";

    return i->second;
}

/** lc3_sym_add
  *
  * Adds this symbol to the symbol table.
  * @return true if added successfully (no replacements) false if symbol already exists
  * @warning if the symbol already exists it will be overwritten.
  */
bool lc3_sym_add(lc3_state& state, const std::string& symbol, unsigned short addr)
{
    bool ret = (state.symbols.find(symbol) == state.symbols.end());
    state.symbols[symbol] = addr;
    state.rev_symbols[addr] = symbol;
    return ret;
}

/** lc3_sym_delete
  *
  * Deletes the symbol from the symbol table
  */
void lc3_sym_delete(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    // Symbol not found.
    if (addr == -1) return;

    lc3_sym_delete(state, addr);
}

/** lc3_sym_delete
  *
  * Deletes the symbol that refers to the address given.
  */
void lc3_sym_delete(lc3_state& state, unsigned short addr)
{
    std::string symbol = lc3_sym_rev_lookup(state, addr);

    // Symbol not found.
    if (symbol.empty()) return;

    state.symbols.erase(symbol);
    state.rev_symbols.erase(addr);
}

/** lc3_sym_clear
  *
  * Clears the symbol table
  */
void lc3_sym_clear(lc3_state& state)
{
    state.symbols.clear();
    state.rev_symbols.clear();
}
