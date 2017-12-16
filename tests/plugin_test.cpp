//#define BOOST_TEST_MODULE PluginTest
#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <lc3_all.hpp>
#include "ExpressionEvaluator.hpp"

struct LC3Test
{
    LC3Test()
    {
        lc3_init(state, false);
        options.multiple_errors = false;
    }

    lc3_state state;
    LC3AssembleOptions options;
};

BOOST_FIXTURE_TEST_CASE(TestInstructionPlugin, LC3Test)
{
    const std::string asm_file =
    ";@plugin filename=lc3_multiply\n"
    ".orig x3000\n"
    "    LD R0, OTF\n"
    "    LD R1, EIGHT\n"
    "    MUL R2, R0, R1\n"
    "    MUL R3, R0, 4\n"
    "    HALT\n"
    "OTF .fill 125\n"
    "EIGHT .fill 8\n"
    ".end";

    std::stringstream file(asm_file);
    std::vector<code_range> ranges;
    lc3_assemble(state, file, ranges, options);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xD401));
    BOOST_REQUIRE_EQUAL(state.mem[0x3003], short(0xD624));
    lc3_run(state, 5);
    BOOST_REQUIRE(state.halted);
    BOOST_CHECK_EQUAL(state.regs[0], 125);
    BOOST_CHECK_EQUAL(state.regs[1], 8);
    BOOST_CHECK_EQUAL(state.regs[2], 1000);
    BOOST_CHECK_EQUAL(state.regs[3], 500);
    BOOST_CHECK(!state.n);
    BOOST_CHECK(!state.z);
    BOOST_CHECK(state.p);
    lc3_rewind(state, 2);
    BOOST_CHECK_EQUAL(state.regs[0], 125);
    BOOST_CHECK_EQUAL(state.regs[1], 8);
    BOOST_CHECK_EQUAL(state.regs[2], 1000);
    BOOST_CHECK_EQUAL(state.regs[3], 0);
    lc3_rewind(state, 1);
    BOOST_CHECK_EQUAL(state.regs[0], 125);
    BOOST_CHECK_EQUAL(state.regs[1], 8);
    BOOST_CHECK_EQUAL(state.regs[2], 0);
    BOOST_CHECK_EQUAL(state.regs[3], 0);
}

BOOST_FIXTURE_TEST_CASE(TestInstructionPluginDisassemble, LC3Test)
{
    const std::string asm_file =
    ";@plugin filename=lc3_multiply\n"
    ".orig x3000\n"
    "    MUL R0, R0, 1\n"
    "    MUL R0, R1, 1\n"
    "    MUL R0, R0, 0\n"
    "    MUL R0, R0, 2\n"
    "    MUL R0, R1, 2\n"
    "    MUL R0, R0, R1\n"
    "    MUL R0, R1, R2\n"
    "    MUL R0, R0, R0\n"
    ".end";

    const std::vector<std::string> answers_basic = {
        "MUL R0, R0, #1",
        "MUL R0, R1, #1",
        "MUL R0, R0, #0",
        "MUL R0, R0, #2",
        "MUL R0, R1, #2",
        "MUL R0, R0, R1",
        "MUL R0, R1, R2",
        "MUL R0, R0, R0",
    };

    const std::vector<std::string> answers_advanced = {
        "TEST R0",
        "R0 = R1",
        "R0 = 0",
        "R0 *= 2",
        "R0 = R1 * 2",
        "R0 *= R1",
        "R0 = R1 * R2",
        "R0 *= R0",
    };

    std::stringstream file(asm_file);
    std::vector<code_range> ranges;
    lc3_assemble(state, file, ranges, options);

    for (unsigned int i = 0; i < answers_basic.size(); i++)
        BOOST_CHECK_EQUAL(lc3_basic_disassemble(state, state.mem[0x3000 + i]), answers_basic[i]);

    for (unsigned int i = 0; i < answers_basic.size(); i++)
        BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3000 + i]), answers_basic[i]);

    for (unsigned int i = 0; i < answers_advanced.size(); i++)
        BOOST_CHECK_EQUAL(lc3_smart_disassemble(state, state.mem[0x3000 + i]), answers_advanced[i]);
}

BOOST_FIXTURE_TEST_CASE(TestTrapPlugin, LC3Test)
{
    const std::string asm_file =
    ";@plugin filename=lc3_udiv vector=x80\n"
    ".orig x3000\n"
    "    LD R0, A\n"
    "    LD R1, B\n"
    "    UDIV\n"
    "    HALT\n"
    "A .fill 2000\n"
    "B .fill 8\n"
    ".end";

    std::stringstream file(asm_file);
    std::vector<code_range> ranges;
    lc3_assemble(state, file, ranges, options);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xF080));
    lc3_run(state, 4);
    BOOST_REQUIRE(state.halted);
    BOOST_CHECK_EQUAL(state.regs[0], 250);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
}

BOOST_FIXTURE_TEST_CASE(TestTrapPluginDisassemble, LC3Test)
{
    const std::string asm_file =
    ";@plugin filename=lc3_udiv vector=x80\n"
    ".orig x3000\n"
    "    LD R0, A\n"
    "    LD R1, B\n"
    "    UDIV\n"
    "    HALT\n"
    "A .fill 2000\n"
    "B .fill 8\n"
    ".end";

    std::stringstream file(asm_file);
    std::vector<code_range> ranges;
    lc3_assemble(state, file, ranges, options);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xF080));
    BOOST_CHECK_EQUAL(lc3_basic_disassemble(state, state.mem[0x3002]), "TRAP x80");
    BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3002]), "UDIV");
    BOOST_CHECK_EQUAL(lc3_smart_disassemble(state, state.mem[0x3002]), "UDIV");
}
