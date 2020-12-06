#include <cstdlib>
#include <cstring>
#include <cstdio>
#include "lc3/ExpressionEvaluator.hpp"

/* -----------------------------------------------------------------------------
 * Copyright (c) 2003 Lallous <lallousx86@yahoo.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * -----------------------------------------------------------------------------
 * Small modifications by Brandon Whitehead <bwhitehead0308@gmail.com>
 * 1) Removed support for doubles (and thus the / operator)
 * 2) Added support for symbols (or a function if you will).
 * 3) Added support for []'s (though you can only use one i.e. A[5][4] is not allowed) you may nest them though A[A[5]] works.
 */

/*
 History
 ----------

 Sunday, November 2, 2003
            * Initial version

 Monday, November 3, 2003
            * Fixed precedence rule of multiplication

 Tuesday, November 4, 2003
            * Fixed a bug in isOperator()
            * Added support for negative and positive numbers as: -1 or +1
              (initially they were supported as: 0-1 or 0+1)
            * Added exception handling and foolproofs against malformed expression
            * Added >=, <=, != operators
*/
namespace ExpressionEvaluator
{
using std::string;
const char *operator_mul   = "*";
const char *operator_div   = "/";
const char *operator_mod   = "%";
const char *operator_add   = "+";
const char *operator_sub   = "-";
const char *operator_xor   = "^";
const char *operator_band  = "&";
const char *operator_bor   = "|";
const char *operator_land  = "&&";
const char *operator_lor   = "||";
const char *operator_nor   = "!|";
const char *operator_nand  = "!&";
const char *operator_iseq  = "==";
const char *operator_ne    = "!=";
const char *operator_shl   = "<<";
const char *operator_shr   = ">>";
const char *operator_lt    = "<";
const char *operator_gt    = ">";
const char *operator_gte   = ">=";
const char *operator_lte   = "<=";

struct operator_t
{
    const char *op;
    int  precedence;
};

const operator_t operators[] =
{
    {"<>=*/%+-^&|\\", -1}, // Operators[0] is reserved for storing all symbols that can be used in operators
    {operator_mul, 19},
    {operator_div, 19},
    {operator_mod, 18},
    {operator_shl, 17}, {operator_shr, 17},
    {operator_sub, 16}, {operator_add, 16},
    {operator_xor, 15}, {operator_band, 15}, {operator_bor, 15}, {operator_nor, 15},
    {operator_land, 14}, {operator_lor, 14}, {operator_nand, 15},
    {operator_iseq, 13}, {operator_lt, 13}, {operator_gt, 13},
    {operator_gte, 13}, {operator_ne, 13}, {operator_lte, 13}
};


std::map<std::string, eval_type> symbol_table;
int (*OnUndefinedSymbol)(const std::string& symbol, bool hasref, int ref, int& error) = 0;

int EvaluateOperand(string, int&);

// Returns < 0 if 'op' is not an operator
// Otherwise it returns the index of the operator in the 'operators' array
// The index can be used to get the operator's precedence value.
int isOperator(string op)
{
    unsigned int i = 0;
    int oplen = 0;
    string s = operators[0].op;
    // scan as long as it is a valid operator
    // an operator might  have not just one symbol to represent it

    while (i<op.length())
    {
        if (s.find(op[i]) == s.npos)
            break;
        oplen++;
        i++;
    }

    // no valid symbol in operator!s
    if (oplen == 0)
        return -1;

    // identify what operator that is and return its index
    // 1 = highest , N = lowest
    // -1 = invalid operator
    s = op.substr(0, oplen);
    for (i=1; i<sizeof(operators)/sizeof(operator_t); i++)
    {
        if (s == operators[i].op)
            return i;
    }
    return -1;
}

// returns the operands length.
// scans as long as the current value is an alphanumeric or []
int getToken(string str)
{
    unsigned int i=0;
    int tokenLen = 0;
    int depth = 0;
    while (i < str.length() && (isalnum(str[i]) || str[i] == '[' || str[i] == ']' || str[i] == '_' || depth != 0) )
    {
        if (str[i] == '[') depth++;
        if (str[i] == ']') depth--;
        if (depth < 0) return 0;
        tokenLen++;
        i++;
    }
    return depth == 0 ? tokenLen : 0;
}

int toRPN(string exp, string &rpn)
{
    std::stack<string> st;
    string token, topToken;
    int  tokenLen, topPrecedence, idx, precedence;
    string SEP(" "), EMPTY("");

    rpn = "";

    for (unsigned int i=0; i<exp.length(); i++)
    {
        char token1 = exp[i];

        // skip white space
        if (isspace(token1))
            continue;

        // push left parenthesis
        else if (token1 == '(')
        {
            st.push(EMPTY+token1);
            continue;
        }

        // flush all stack till matching the left-parenthesis
        else if (token1 == ')')
        {
            for (;;)
            {
                // could not match left-parenthesis
                if (st.empty())
                    return eval_unbalanced;

                topToken = st.top();
                st.pop();
                if (topToken == "(")
                    break;
                rpn.append(SEP + topToken);
            }
            continue;
        }

        // is this an operator?
        idx = isOperator(exp.substr(i));

        // an operand
        if (idx < 0)
        {
            tokenLen = getToken(exp.substr(i));
            if (tokenLen == 0)
            {
                return eval_invalidoperand;
            }

            token = exp.substr(i, tokenLen);
            rpn.append(SEP + token);
            i += tokenLen - 1;
            continue;
        }

        // is an operator
        else
        {
            // expression is empty or last operand an operator
            if (rpn.empty() || (isOperator(token) > 0))
            {
                rpn.append(SEP + "0");
            }
            // get precedence
            precedence = operators[idx].precedence;
            topPrecedence = 0;

            // get current operator
            tokenLen = strlen(operators[idx].op);
            token    = exp.substr(i, tokenLen);
            i += tokenLen - 1;
            for (;;)
            {
                // get top's precedence
                if (!st.empty())
                {
                    topToken = st.top();
                    idx = isOperator(topToken.c_str());
                    if (idx < 0)
                        topPrecedence = 1; // give a low priority if operator not ok!
                    else
                        topPrecedence = operators[idx].precedence;
                }

                if (st.empty() || st.top() == "(" ||
                        precedence > topPrecedence
                   )
                {
                    st.push(token);
                    break;
                }
                // operator has lower precedence then pop it
                else
                {
                    st.pop();
                    rpn.append(SEP + topToken);
                }
            }
            continue;
        }
    }

    for (;;)
    {
        if (st.empty())
            break;
        topToken = st.top();
        st.pop();
        if (topToken != "(")
            rpn.append(SEP + topToken);
        else
        {
            return eval_unbalanced;
        }
    }
    return eval_ok;
}

//template <typename T> int evaluateRPN(string rpn, T &result)
int evaluateRPN(string rpn, int& result)
{
    std::stack<int> st;
    string token;
    int r = 0, op1, op2;
    int idx, tokenLen;

    for (unsigned int i=0; i<rpn.length(); i++)
    {
        char token1 = rpn[i];
        if (isspace(token1))
            continue;

        idx = isOperator(rpn.substr(i));

        // not an operator
        if (idx < 0)
        {
            tokenLen = getToken(rpn.substr(i));
            token    = rpn.substr(i, tokenLen);

            int error;
            int val = EvaluateOperand(token, error);
            r = val;

            if (error) return error;

            st.push(r);
            i += tokenLen - 1;
            continue;
        }
        // an operator
        else
        {
            // get operand1 and op2
            op2 = st.top();
            st.pop();
            if (!st.empty())
            {
                op1 = st.top();
                st.pop();
            }
            else
                return eval_unbalanced;

            tokenLen = strlen(operators[idx].op);
            token    = rpn.substr(i, tokenLen);
            i += tokenLen - 1;

            if (token == operator_mul)
                r = op1 * op2;
            else if (token == operator_div)
                r = op1 / op2;
            else if (token == operator_mod)
                r = op1 % op2;
            else if (token == operator_add)
                r = op1 + op2;
            else if (token == operator_sub)
                r = op1 - op2;
            else if (token == operator_land)
                r = op1 && op2;
            else if (token == operator_band)
                r = op1 & op2;
            else if (token == operator_lor)
                r = op1 || op2;
            else if (token == operator_bor)
                r = op1 | op2;
            else if (token == operator_xor)
                r = op1 ^ op2;
            else if (token == operator_nor)
                r = ~(op1 | op2);
            else if (token == operator_nand)
                r = ~(op1 & op2);
            else if (token == operator_iseq)
                r = op1 == op2;
            else if (token == operator_ne)
                r = op1 != op2;
            else if (token == operator_shl)
                r = op1 << op2;
            else if (token == operator_shr)
                r = op1 >> op2;
            else if (token == operator_lt)
                r = op1 < op2;
            else if (token == operator_lte)
                r = op1 <= op2;
            else if (token == operator_gt)
                r = op1 > op2;
            else if (token == operator_gte)
                r = op1 >= op2;
            // push result
            st.push(r);
        }
    }
    result = st.top();
    st.pop();
    if (!st.empty())
        return eval_evalerr;
    return eval_ok;
}

int Calculate(string expr, int &r)
{
    string rpn;
    int err = eval_evalerr; // unexpected error

    if (expr.empty())
    {
        r = 0;
        return 0;
    }

    try
    {
        if ( (err = toRPN(expr, rpn)) != eval_ok)
            return err;
        err = evaluateRPN(rpn, r);
    }
    catch(...)
    {
    }
    return err;
}

int SymbolTableLookup(string symbol, bool hasref, int ref, int& error)
{
    if (symbol_table.find(symbol) == symbol_table.end())
    {
        error = eval_undefinedsymbol;
        // Last hope~ if they defined the undefined symbol function
        if (OnUndefinedSymbol)
        {
            int value = OnUndefinedSymbol(symbol, hasref, ref, error);
            return value;
        }
        else
        {
            return -1;
        }
    }

    eval_type value = symbol_table[symbol];

    switch (value.type)
    {
    case EVAL_VALUE:
        return value.eval.value;
        break;
    case EVAL_FUNCTION:
        return value.eval.func(hasref, ref);
        break;
    default:
        error = eval_evalerr;
    }

    return -1;
}

int EvaluateOperand(string token, int& error)
{
    error = 0;

    // Handle numerals
    char *errstr;
    int d = (int)strtol(token.c_str(), &errstr, 0);
    if (!(*errstr)) return d;
    if (token[0] == 'x' || token[0] == 'X')
    {
        std::string hex = "0" + token;
        d = (int)strtol(hex.c_str(), &errstr, 0);
        if (!(*errstr)) return d;
    }

    // If we have a [] then attempt to evaluate the contents of this first
    size_t begin = token.find('[');
    size_t end = token.rfind(']');

    // If we get something like [3293] reject
    if (begin == 0)
    {
        error = eval_malformedreference;
        return -1;
    }
    // If we do have an end but the end is not the end of the string reject i.e. A[5670]A
    if (end != std::string::npos && end != token.length() - 1)
    {
        error = eval_malformedreference;
        return -1;
    }
    // If we get something like 123[123] reject
    if (!isalpha(token[0]))
    {
        error = eval_malformedreference;
        return -1;
    }
    // if we get something like A[] reject
    if (begin + 1 == end)
    {
        error = eval_malformedreference;
        return -1;
    }

    int ref = -1;
    bool has_ref = begin != end && begin != std::string::npos;
    if (has_ref)
    {
        error = Calculate(token.substr(begin + 1, end - begin - 1), ref);
        if (error) return -1;
    }

    return SymbolTableLookup(token.substr(0, begin), has_ref, ref, error);
}
}


