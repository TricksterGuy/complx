#include "lc3_cmp.hpp"
#include <string>
#include <algorithm>

bool lc3_is_equal(const void* a, const void* b);
bool lc3_is_less(const void* a, const void* b);
bool lc3_is_greater(const void* a, const void* b);
bool lc3_is_lessequal(const void* a, const void* b);
bool lc3_is_greaterequal(const void* a, const void* b);
bool lc3_is_notequal(const void* a, const void* b);

bool lc3_is_string_equal(const void* a, const void* b);
bool lc3_is_string_notequal(const void* a, const void* b);
bool lc3_is_string_equal_ic(const void* a, const void* b);
bool lc3_is_string_notequal_ic(const void* a, const void* b);

bool lc3_is_string_contains(const void* a, const void* b);
bool lc3_is_string_notcontains(const void* a, const void* b);
bool lc3_is_string_contains_ic(const void* a, const void* b);
bool lc3_is_string_notcontains_ic(const void* a, const void* b);

bool lc3_is_array_equal(const void* a, const void* b);
bool lc3_is_array_notequal(const void* a, const void* b);

lc3_cmp lc3_comparators[CMP_SIZE] =
{
    lc3_is_equal,
    lc3_is_less,
    lc3_is_greater,
    lc3_is_lessequal,
    lc3_is_greaterequal,
    lc3_is_notequal,

    lc3_is_string_equal,
    lc3_is_string_notequal,
    lc3_is_string_equal_ic,
    lc3_is_string_notequal_ic,

    lc3_is_string_contains,
    lc3_is_string_notcontains,
    lc3_is_string_contains_ic,
    lc3_is_string_notcontains_ic,

    lc3_is_array_equal,
    lc3_is_array_notequal,
};

bool lc3_is_equal(const void* a, const void* b)
{
    return *(short*)a == *(short*)b;
}

bool lc3_is_less(const void* a, const void* b)
{
    return *(short*)a < *(short*)b;
}

bool lc3_is_greater(const void* a, const void* b)
{
    return *(short*)a > *(short*)b;
}

bool lc3_is_lessequal(const void* a, const void* b)
{
    return *(short*)a <= *(short*)b;
}

bool lc3_is_greaterequal(const void* a, const void* b)
{
    return *(short*)a >= *(short*)b;
}

bool lc3_is_notequal(const void* a, const void* b)
{
    return *(short*)a != *(short*)b;
}

bool lc3_is_string_equal(const void* a, const void* b)
{
    const std::string& sa = *(std::string*)a;
    const std::string& sb = *(std::string*)b;

    return sa == sb;
}

bool lc3_is_string_notequal(const void* a, const void* b)
{
    const std::string& sa = *(std::string*)a;
    const std::string& sb = *(std::string*)b;

    return sa != sb;
}

bool lc3_is_string_equal_ic(const void* a, const void* b)
{
    std::string sal = *(std::string*)a;
    std::string sbl = *(std::string*)b;

    std::transform(sal.begin(), sal.end(), sal.begin(), (int(*)(int))std::tolower);
    std::transform(sbl.begin(), sbl.end(), sbl.begin(), (int(*)(int))std::tolower);

    return sal == sbl;
}

bool lc3_is_string_notequal_ic(const void* a, const void* b)
{
    std::string sal = *(std::string*)a;
    std::string sbl = *(std::string*)b;

    std::transform(sal.begin(), sal.end(), sal.begin(), (int(*)(int))std::tolower);
    std::transform(sbl.begin(), sbl.end(), sbl.begin(), (int(*)(int))std::tolower);

    return sal != sbl;
}

bool lc3_is_string_contains(const void* a, const void* b)
{
    const std::string& sa = *(std::string*)a;
    const std::string& sb = *(std::string*)b;

    return sa.find(sb) != std::string::npos;
}

bool lc3_is_string_notcontains(const void* a, const void* b)
{
    const std::string& sa = *(std::string*)a;
    const std::string& sb = *(std::string*)b;

    return sa.find(sb) == std::string::npos;
}

bool lc3_is_string_contains_ic(const void* a, const void* b)
{
    std::string sal = *(std::string*)a;
    std::string sbl = *(std::string*)b;

    std::transform(sal.begin(), sal.end(), sal.begin(), (int(*)(int))std::tolower);
    std::transform(sbl.begin(), sbl.end(), sbl.begin(), (int(*)(int))std::tolower);

    return sal.find(sbl) != std::string::npos;
}

bool lc3_is_string_notcontains_ic(const void* a, const void* b)
{
    std::string sal = *(std::string*)a;
    std::string sbl = *(std::string*)b;

    std::transform(sal.begin(), sal.end(), sal.begin(), (int(*)(int))std::tolower);
    std::transform(sbl.begin(), sbl.end(), sbl.begin(), (int(*)(int))std::tolower);

    return sal.find(sbl) == std::string::npos;
}

bool lc3_is_array_equal(const void* a, const void* b)
{
    const std::vector<short>& actual = *(const std::vector<short>*)a;
    const std::vector<short>& expected = *(const std::vector<short>*)b;

    for (unsigned int i = 0; i < expected.size(); i++)
    {
        if (actual[i] != expected[i])
            return false;
    }
    return true;
}

bool lc3_is_array_notequal(const void* a, const void* b)
{
    const std::vector<short>& actual = *(const std::vector<short>*)a;
    const std::vector<short>& expected = *(const std::vector<short>*)b;

    for (unsigned int i = 0; i < expected.size(); i++)
    {
        if (actual[i] != expected[i])
            return true;
    }
    return false;
}

bool lc3_test_check(lc3_test_output& test_cond, const void* actual, const void* expected)
{
    if (test_cond.cmp_type < 0 || test_cond.cmp_type >= CMP_SIZE)
        throw "Invalid comparison type";

    lc3_cmp func = lc3_comparators[test_cond.cmp_type];

    return func(actual, expected);
}
