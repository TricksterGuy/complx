
#define BOOST_TEST_MAIN
#if !defined(WINDOWS)
    #define BOOST_TEST_DYN_LINK
#endif
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <lc3_all.hpp>
#include "ExpressionEvaluator.hpp"

struct LC3AssembleTest
{
    lc3_state state;

    LC3AssembleTest()
    {
        lc3_init(state, false);
    }
};

bool is_number_overflow(const LC3AssembleException& ex)
{
    return ex.get_id() == NUMBER_OVERFLOW;
}

BOOST_FIXTURE_TEST_CASE(AddImmediateTest, LC3AssembleTest)
{
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 0"), 0x1020);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 1"), 0x1021);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, -1"), 0x103F);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 8"), 0x1028);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, -16"), 0x1030);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 15"), 0x102F);

    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "ADD R0, R0, xF"), 0x102F);

    LC3AssembleOptions options;
    options.multiple_errors = false;

    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 16", -1, options), LC3AssembleException, is_number_overflow);
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, x1F", -1, options), LC3AssembleException, is_number_overflow);
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, x10", -1, options), LC3AssembleException, is_number_overflow);
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, xFFFF", -1, options), LC3AssembleException, is_number_overflow);
}


