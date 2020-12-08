#ifndef LC3_EXPRESSIONS_HPP
#define LC3_EXPRESSIONS_HPP

#include "lc3/lc3.hpp"

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

class LC3_API LC3CalculateException : public std::exception
{
public:
    LC3CalculateException(const std::string& expr, unsigned int error_id);
    const char* what() const noexcept override { return message.c_str(); }
    unsigned int get_id() const { return id; }
private:
    std::string form_error_message() const noexcept;
    std::string message;
    std::string expression;
    unsigned int id;
};

/** lc3_calculate
  *
  * Computes an expression against the state passed in.
  * @warning Not thread safe.
  * @param state LC3State object.
  * @param expr A string containing an expression.
  * @return Return value
  * @throws LC3CalculateException if an error occurs.
  */
int LC3_API lc3_calculate(lc3_state& state, const std::string& expr);

/** lc3_int_eval
  *
  * Initializes expression evaluator for symbols.
  */
void lc3_init_eval();

#endif
