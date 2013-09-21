#ifndef LC3_CMP_HPP
#define LC3_CMP_HPP

#include "lc3_test.hpp"

struct lc3_test_output;

enum lc3_cmp_type
{
    CMP_EQUAL = 0,
    CMP_LESS = 1,
    CMP_GREATER = 2,
    CMP_LESSEQUAL = 3,
    CMP_GREATEREQUAL = 4,
    CMP_NOTEQUAL = 5,

    CMP_STRING_EQUAL = 6,
    CMP_STRING_NOTEQUAL = 7,
    CMP_STRING_EQUALIC = 8,
    CMP_STRING_NOTEQUALIC = 9,
    CMP_STRING_CONTAINS = 10,
    CMP_STRING_NOTCONTAINS = 11,
    CMP_STRING_CONTAINSIC = 12,
    CMP_STRING_NOTCONTAINSIC = 13,

    CMP_ARRAY_EQUAL = 14,
    CMP_ARRAY_NOTEQUAL = 15,
    CMP_SIZE,
};

typedef bool (*lc3_cmp)(const void*, const void*);

extern lc3_cmp lc3_comparators[CMP_SIZE];

bool lc3_test_check(lc3_test_output& test_cond, const void*, const void*);

#endif
