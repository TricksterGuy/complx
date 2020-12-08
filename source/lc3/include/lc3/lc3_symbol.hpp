#ifndef LC3_SYMBOL_HPP
#define LC3_SYMBOL_HPP

#include "lc3/lc3.hpp"

/** lc3_load_sym
  *
  * Loads the given symbol table file into the lc3 state.
  * @param state LC3State object.
  * @param file Stream for reading symbol table from.
  * @return 0 on success, nonzero for failure.
  */
int LC3_API lc3_load_sym(lc3_state& state, std::istream& file);
/** lc3_sym_lookup
  *
  * Looks up the given symbol in the symbol table.
  * @param state LC3State object.
  * @param symbol Symbol to look for.
  * @return -1 if the symbol was not found otherwise the address where the symbol is located.
  */
int LC3_API lc3_sym_lookup(lc3_state& state, const std::string& symbol);
/** lc3_sym_rev_lookup
  *
  * Looks at the given memory address for a symbol.
  * @param state LC3State object.
  * @param addr Address to search.
  * @return the symbol at the address, otherwise an empty string.
  */
LC3_API std::string lc3_sym_rev_lookup(lc3_state& state, uint16_t addr);
/** lc3_sym_add
  *
  * Adds this symbol to the symbol table, overwriting the associated address if it already exists.
  * @param state LC3State object.
  * @param symbol Symbol to look for.
  * @param addr Address to link symbol to.
  * @return true if added successfully (not replaced) false if symbol already exists.
  */
bool LC3_API lc3_sym_add(lc3_state& state, const std::string& symbol, uint16_t addr);
/** lc3_sym_delete
  *
  * Deletes the symbol from the symbol table.
  * @param state LC3State object.
  * @param symbol Symbol to delete.
  */
void LC3_API lc3_sym_delete(lc3_state& state, const std::string& symbol);
/** lc3_sym_delete
  *
  * Deletes the symbol that refers to the address given.
  * @param state LC3State object.
  * @param addr Address to delete associated symbols.
  */
void LC3_API lc3_sym_delete(lc3_state& state, uint16_t addr);
/** lc3_sym_clear
  *
  * Clears the symbol table.
  * @param state LC3State object.
  */
void LC3_API lc3_sym_clear(lc3_state& state);

#endif

