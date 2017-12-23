//#define BOOST_TEST_MODULE AssembleTest
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <lc3_all.hpp>
#include "ExpressionEvaluator.hpp"

#define IS_EXCEPTION(type) [](const LC3AssembleException& e) {return e.get_id() == type;}

struct LC3AssembleTest
{
    lc3_state state;

    LC3AssembleTest()
    {
        lc3_init(state, false);
    }
};

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

    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, 16", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, x1F", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, x10", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "ADD R0, R0, xFFFF", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
}

BOOST_FIXTURE_TEST_CASE(AndImmediateTest, LC3AssembleTest)
{
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 0"), 0x5020);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 1"), 0x5021);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, -1"), 0x503F);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 8"), 0x5028);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, -16"), 0x5030);
    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, 15"), 0x502F);

    BOOST_CHECK_EQUAL(lc3_assemble_one(state, 0x3000, "AND R0, R0, xF"), 0x502F);

    LC3AssembleOptions options;
    options.multiple_errors = false;

    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, 16", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, x1F", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, x10", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
    BOOST_CHECK_EXCEPTION(lc3_assemble_one(state, 0x3000, "AND R0, R0, xFFFF", -1, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));
}

BOOST_FIXTURE_TEST_CASE(FillTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        ".fill 0\n"
        ".fill 1\n"
        ".fill 10\n"
        ".fill xFFFF\n"
        ".fill -1\n"
        ".fill 'A'\n"
        ".fill -32768\n"
        ".fill x8000\n"
        ".end"
    );
    std::vector<code_range> ranges;
    LC3AssembleOptions options;
    options.multiple_errors = false;
    lc3_assemble(state, file, ranges, options);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 1);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 10);
    BOOST_CHECK_EQUAL(state.mem[0x3003], -1);
    BOOST_CHECK_EQUAL(state.mem[0x3004], -1);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 'A');
    BOOST_CHECK_EQUAL(state.mem[0x3006], -32768);
    BOOST_CHECK_EQUAL(state.mem[0x3007], -32768);
    ranges.clear();

    std::istringstream bad_file(
        ".orig x3000\n"
        ".fill 32768\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, bad_file, ranges, options), LC3AssembleException, IS_EXCEPTION(NUMBER_OVERFLOW));

}

BOOST_FIXTURE_TEST_CASE(StringzTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        "A .stringz \"Hello!\"\n"
        "B .stringz \"World\"\n"
        ".end"
    );
    std::vector<code_range> ranges;
    LC3AssembleOptions options;
    options.multiple_errors = false;
    lc3_assemble(state, file, ranges, options);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 'H');
    BOOST_CHECK_EQUAL(state.mem[0x3001], 'e');
    BOOST_CHECK_EQUAL(state.mem[0x3002], 'l');
    BOOST_CHECK_EQUAL(state.mem[0x3003], 'l');
    BOOST_CHECK_EQUAL(state.mem[0x3004], 'o');
    BOOST_CHECK_EQUAL(state.mem[0x3005], '!');
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0);
    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "A"), 0x3000);

    BOOST_CHECK_EQUAL(state.mem[0x3007], 'W');
    BOOST_CHECK_EQUAL(state.mem[0x3008], 'o');
    BOOST_CHECK_EQUAL(state.mem[0x3009], 'r');
    BOOST_CHECK_EQUAL(state.mem[0x300A], 'l');
    BOOST_CHECK_EQUAL(state.mem[0x300B], 'd');
    BOOST_CHECK_EQUAL(state.mem[0x300C], 0);
    BOOST_CHECK_EQUAL(lc3_sym_lookup(state, "B"), 0x3007);

    ranges.clear();

    std::istringstream bad_file(
        ".orig x3000\n"
        ".stringz lol\n"
        ".end"
    );

    BOOST_CHECK_EXCEPTION(lc3_assemble(state, bad_file, ranges, options), LC3AssembleException, IS_EXCEPTION(MALFORMED_STRING));
}

BOOST_FIXTURE_TEST_CASE(InvalidSymbolTest, LC3AssembleTest)
{
    std::istringstream long_sym(
        ".orig x3000\n"
        "IAMAVERYLONGSYMBOLGUIZE .fill 0\n"
        ".end"
    );
    std::vector<code_range> ranges;
    LC3AssembleOptions options;
    options.multiple_errors = false;

    BOOST_CHECK_EXCEPTION(lc3_assemble(state, long_sym, ranges, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream register_sym(
        ".orig x3000\n"
        "R0 .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, register_sym, ranges, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream hexa_sym(
        ".orig x3000\n"
        "x8 .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, hexa_sym, ranges, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream bin_sym(
        ".orig x3000\n"
        "b101 .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, bin_sym, ranges, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream num_sym(
        ".orig x3000\n"
        "8HIYA .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, num_sym, ranges, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));

    std::istringstream invalid_sym(
        ".orig x3000\n"
        "DOLLADOLLA$$$ .fill 0\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, invalid_sym, ranges, options), LC3AssembleException, IS_EXCEPTION(INVALID_SYMBOL));
}

BOOST_FIXTURE_TEST_CASE(BinaryLiteralTest, LC3AssembleTest)
{
    std::istringstream file(
        ".orig x3000\n"
        ".fill b0\n"
        ".fill b1\n"
        ".fill b0111000011101111\n"
        "ADD R0, R1, b01111\n"
        "LEA R0, b011110000\n"
        "JSR b01111000001\n"
        "LDR R0, R1, b011111\n"
        ".end"
    );
    std::vector<code_range> ranges;
    LC3AssembleOptions options;
    options.multiple_errors = false;
    lc3_assemble(state, file, ranges, options);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 0);
    BOOST_CHECK_EQUAL(state.mem[0x3001], 1);
    BOOST_CHECK_EQUAL(state.mem[0x3002], 0x70EF);
    BOOST_CHECK_EQUAL(state.mem[0x3003], 0x106F);
    BOOST_CHECK_EQUAL(state.mem[0x3004], (short)0xE0F0);
    BOOST_CHECK_EQUAL(state.mem[0x3005], 0x4BC1);
    BOOST_CHECK_EQUAL(state.mem[0x3006], 0x605F);
}
