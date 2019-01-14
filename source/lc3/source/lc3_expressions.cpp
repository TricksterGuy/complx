#include "lc3/lc3_expressions.hpp"

#include <cassert>
#include <sstream>

#include "lc3/ExpressionEvaluator.hpp"
#include "lc3/lc3_symbol.hpp"

using namespace ExpressionEvaluator;

static int lc3_get_symbol(const std::string& symbol, bool has_ref, int ref, int& error);

static lc3_state* calc_state;

int get_r0(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[0] + ref)];
    else
        return calc_state->regs[0];
}

int get_r1(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[1] + ref)];
    else
        return calc_state->regs[1];
}

int get_r2(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[2] + ref)];
    else
        return calc_state->regs[2];
}

int get_r3(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[3] + ref)];
    else
        return calc_state->regs[3];
}

int get_r4(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[4] + ref)];
    else
        return calc_state->regs[4];
}

int get_r5(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[5] + ref)];
    else
        return calc_state->regs[5];
}

int get_r6(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[6] + ref)];
    else
        return calc_state->regs[6];
}

int get_r7(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->regs[7] + ref)];
    else
        return calc_state->regs[7];
}

int get_pc(bool hasref, int ref)
{
    if (hasref)
        return calc_state->mem[(unsigned short)(calc_state->pc + ref)];
    else
        return calc_state->pc;
}

int get_mem(bool /*hasref*/, int ref)
{
    return calc_state->mem[(unsigned short) ref];
}

int lc3_get_symbol(const std::string& symbol, bool hasref, int ref, int& error)
{
    assert(calc_state != NULL);

    int addr = lc3_sym_lookup(*calc_state, symbol);

    // No Symbol in LC3 symbol table error
    if (addr == -1)
        return -1;

    // There isn't an error.
    error = 0;

    if (hasref)
        return calc_state->mem[(unsigned short)(addr + ref)];
    else
        return (unsigned short)addr;
}

std::string LC3CalculateException::what() const throw()
{
    std::stringstream stream("Error evaluating expression ");

    switch(id)
    {
        case eval_unbalanced:
            stream << "\"" << expression << "\". Unbalanced parenthesis";
            break;
        case eval_invalidoperator:
            stream << "\"" << expression << "\". Invalid Operator";
            break;
        case eval_invalidoperand:
            stream << "\"" << expression << "\". Invalid operand";
            break;
        case eval_malformedreference:
            stream << "\"" << expression << "\". Malformed reference";
            break;
        case eval_undefinedsymbol:
            stream << "\"" << expression << "\". Undefined symbol";
            break;
        default:
            stream << "\"" << expression << "\". Unknown error";
            break;
    }

    return stream.str();
}


int lc3_calculate(lc3_state& state, const std::string& expr)
{
    static int lazy_load = 1;

    if (lazy_load)
    {
        lc3_init_eval();
        OnUndefinedSymbol = lc3_get_symbol;
        lazy_load = 0;
    }

    calc_state = &state;

    int result = 0;
    int error = Calculate(expr, result);

    calc_state = NULL;

    if (error)
        throw LC3CalculateException(expr, error);

    return result;
}

void lc3_init_eval(void)
{
    symbol_table["R0"] =  {EVAL_FUNCTION, {get_r0}};
    symbol_table["r0"] =  {EVAL_FUNCTION, {get_r0}};
    symbol_table["R1"] =  {EVAL_FUNCTION, {get_r1}};
    symbol_table["r1"] =  {EVAL_FUNCTION, {get_r1}};
    symbol_table["R2"] =  {EVAL_FUNCTION, {get_r2}};
    symbol_table["r2"] =  {EVAL_FUNCTION, {get_r2}};
    symbol_table["R3"] =  {EVAL_FUNCTION, {get_r3}};
    symbol_table["r3"] =  {EVAL_FUNCTION, {get_r3}};
    symbol_table["R4"] =  {EVAL_FUNCTION, {get_r4}};
    symbol_table["r4"] =  {EVAL_FUNCTION, {get_r4}};
    symbol_table["R5"] =  {EVAL_FUNCTION, {get_r5}};
    symbol_table["r5"] =  {EVAL_FUNCTION, {get_r5}};
    symbol_table["R6"] =  {EVAL_FUNCTION, {get_r6}};
    symbol_table["r6"] =  {EVAL_FUNCTION, {get_r6}};
    symbol_table["R7"] =  {EVAL_FUNCTION, {get_r7}};
    symbol_table["r7"] =  {EVAL_FUNCTION, {get_r7}};
    symbol_table["PC"] =  {EVAL_FUNCTION, {get_pc}};
    symbol_table["pc"] =  {EVAL_FUNCTION, {get_pc}};
    symbol_table["MEM"] = {EVAL_FUNCTION, {get_mem}};
    symbol_table["mem"] = {EVAL_FUNCTION, {get_mem}};
}
