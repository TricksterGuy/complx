#ifndef LC3_EXPRESSIONS_HPP
#define LC3_EXPRESSIONS_HPP

#include "lc3.hpp"

int get_r0(bool, int);
int get_r1(bool, int);
int get_r2(bool, int);
int get_r3(bool, int);
int get_r4(bool, int);
int get_r5(bool, int);
int get_r6(bool, int);
int get_r7(bool, int);
int get_pc(bool, int);
int get_mem(bool, int);

/** lc3_calculate
  *
  * Computes an expression against the state passed in.
  * @warning Not thread safe.
  * @param state
  * @param expr A string containing an expression.
  * @param r Output param for return value.
  * @return 0 on success, nonzero on error
  */
int lc3_calculate(lc3_state& state, std::string expr, int &r);
/** lc3_int_eval
  *
  * Initializes expression evaluator for symbols.
  * Users should not need to call this directly.
  */
void lc3_init_eval(void);

#endif
