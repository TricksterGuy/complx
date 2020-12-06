#include <lc3.hpp>
#include <boost/test/unit_test.hpp>
#include <fstream>
#include <iostream>
#include <istream>
#include <sstream>
#include <vector>

#define IS_EXCEPTION(type) [](const LC3AssembleException& e) {return e.get_id() == type;}

struct LC3ParserTest
{
    LC3ParserTest()
    {
        lc3_init(state, false);
        context.state = &state;
        context.options.multiple_errors = false;
    }

    lc3_state state;
    LC3AssembleContext context;
};

BOOST_FIXTURE_TEST_CASE(TestProcessStr, LC3ParserTest)
{
    BOOST_CHECK_EQUAL(process_str("\"\"", context), "");
    BOOST_CHECK_EQUAL(process_str("\"HELLO\"", context), "HELLO");
    BOOST_CHECK_EQUAL(process_str("\"\\a\"", context), "\a");
    BOOST_CHECK_EQUAL(process_str("\"\\b\"", context), "\b");
    BOOST_CHECK_EQUAL(process_str("\"\\f\"", context), "\f");
    BOOST_CHECK_EQUAL(process_str("\"\\n\"", context), "\n");
    BOOST_CHECK_EQUAL(process_str("\"\\r\"", context), "\r");
    BOOST_CHECK_EQUAL(process_str("\"\\t\"", context), "\t");
    BOOST_CHECK_EQUAL(process_str("\"\\v\"", context), "\v");
    BOOST_CHECK_EQUAL(process_str("\"'\"", context), "'");
    BOOST_CHECK_EQUAL(process_str("\"\\\"\"", context), "\"");
    BOOST_CHECK_EQUAL(process_str("\"\\\'\"", context), "\'");
    BOOST_CHECK_EQUAL(process_str("\"\\\\\"", context), "\\");
    BOOST_CHECK_EQUAL(process_str("\"\\x30\"", context), "0");
    BOOST_CHECK_EQUAL(process_str("\"\\x30000\"", context), "0000");
    BOOST_CHECK_EQUAL(process_str("\"\"", context), "");
    BOOST_CHECK_EQUAL(process_str("\"\\377\"", context), "\377");
    BOOST_CHECK_EQUAL(process_str("\"\\777\"", context), "\377");

    BOOST_CHECK_EXCEPTION(process_str("\"\\xG\"", context), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));
    BOOST_CHECK_EXCEPTION(process_str("\"\\8\"", context), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));

    context.options.warnings_as_errors = true;
    BOOST_CHECK_EXCEPTION(process_str("\"\\777\"", context), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));
}
