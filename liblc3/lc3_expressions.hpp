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

// Do not call from more than one thread.  NOT THREAD SAFE.
int lc3_calculate(lc3_state& state, std::string expr, int &r);
void lc3_init_eval(void);

#endif
