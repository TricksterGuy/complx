#include "lc3/lc3_symbol.hpp"

#include <sstream>

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

int lc3_sym_lookup(lc3_state& state, const std::string& symbol)
{
    std::map<std::string, unsigned short>::iterator i = state.symbols.find(symbol);

    if (i == state.symbols.end()) return -1;

    return i->second;
}

const std::string lc3_sym_rev_lookup(lc3_state& state, unsigned short addr)
{
    std::map<unsigned short, std::string>::iterator i = state.rev_symbols.find(addr);

    if (i == state.rev_symbols.end()) return "";

    return i->second;
}

bool lc3_sym_add(lc3_state& state, const std::string& symbol, unsigned short addr)
{
    bool ret = (state.symbols.find(symbol) == state.symbols.end());
    state.symbols[symbol] = addr;
    state.rev_symbols[addr] = symbol;
    return ret;
}

void lc3_sym_delete(lc3_state& state, const std::string& symbol)
{
    int addr = lc3_sym_lookup(state, symbol);
    // Symbol not found.
    if (addr == -1) return;

    lc3_sym_delete(state, addr);
}

void lc3_sym_delete(lc3_state& state, unsigned short addr)
{
    std::string symbol = lc3_sym_rev_lookup(state, addr);

    // Symbol not found.
    if (symbol.empty()) return;

    state.symbols.erase(symbol);
    state.rev_symbols.erase(addr);
}

void lc3_sym_clear(lc3_state& state)
{
    state.symbols.clear();
    state.rev_symbols.clear();
}
