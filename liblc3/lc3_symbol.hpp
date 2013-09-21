#ifndef LC3_SYMBOL_HPP
#define LC3_SYMBOL_HPP

#include "lc3.hpp"

/*
======================
Symbol Table Functions
======================
*/
/** Loads the given symbol file into the machine */
int lc3_load_sym(lc3_state& state, std::istream& file);
/** Lookups the symbol in the symbol table */
int lc3_sym_lookup(lc3_state& state, const std::string& symbol);
/** Reverse lookups a memory address for its associating symbol if any */
const std::string lc3_sym_rev_lookup(lc3_state& state, unsigned short addr);
/** Adds a new symbol into the symbol table */
bool lc3_sym_add(lc3_state& state, const std::string& symbol, unsigned short addr);
/** Removes a symbol from the symbol table */
void lc3_sym_delete(lc3_state& state, const std::string& symbol);
/** Removes a memory address from the symbol table */
void lc3_sym_delete(lc3_state& state, unsigned short addr);
/** Clears the symbol table */
void lc3_sym_clear(lc3_state& state);

#endif

