#define BOOST_TEST_MODULE LC3_Plugin_Test
#include <lc3.hpp>
#include <boost/test/unit_test.hpp>
#include <iostream>
#include <istream>
#include <fstream>
#include <vector>

#define IS_EXCEPTION(type) [](const LC3AssembleException& e) {return e.get_id() == type;}

struct LC3PluginTest
{
    LC3PluginTest()
    {
        lc3_init(state, false);
        options.multiple_errors = false;
    }

    ~LC3PluginTest()
    {
        lc3_remove_plugins(state);
    }

    lc3_state state;
    LC3AssembleOptions options;
};

BOOST_FIXTURE_TEST_CASE(TestInstructionPlugin, LC3PluginTest)
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
    BOOST_REQUIRE_NO_THROW(lc3_assemble(state, file, options));
    BOOST_REQUIRE(state.instructionPlugin != nullptr);
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

BOOST_FIXTURE_TEST_CASE(TestInstructionPluginDisassemble, LC3PluginTest)
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
    BOOST_REQUIRE_NO_THROW(lc3_assemble(state, file, options));

    BOOST_REQUIRE(state.instructionPlugin != nullptr);

    for (unsigned int i = 0; i < answers_basic.size(); i++)
        BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3000 + i], -1, LC3_BASIC_DISASSEMBLE), answers_basic[i]);

    for (unsigned int i = 0; i < answers_basic.size(); i++)
        BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3000 + i], -1, LC3_NORMAL_DISASSEMBLE), answers_basic[i]);

    for (unsigned int i = 0; i < answers_advanced.size(); i++)
        BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3000 + i], -1, LC3_ADVANCED_DISASSEMBLE), answers_advanced[i]);
}

BOOST_FIXTURE_TEST_CASE(TestInstructionPluginColoring, LC3PluginTest)
{
    std::map<std::string, std::string> params;
    BOOST_REQUIRE_NO_THROW(lc3_install_plugin(state, "lc3_multiply", params));

    BOOST_REQUIRE(state.instructionPlugin != nullptr);

    std::list<RLEColorEntry> colors;

    // MUL R0, R1, 3
    colors = state.instructionPlugin->GetInstructionColoring(0xD163);
    std::vector<RLEColorEntry> coloring(colors.begin(), colors.end());
    // Colors can change so just check the lengths
    BOOST_REQUIRE_EQUAL(coloring.size(), 4);
    BOOST_CHECK_EQUAL(coloring[0].length, 3);
    BOOST_CHECK_EQUAL(coloring[1].length, 3);
    BOOST_CHECK_EQUAL(coloring[2].length, 1);
    BOOST_CHECK_EQUAL(coloring[3].length, 5);

    colors = state.instructionPlugin->GetInstructionColoring(0xD042);
    coloring.assign(colors.begin(), colors.end());
    // Colors can change so just check the lengths
    BOOST_REQUIRE_EQUAL(coloring.size(), 4);
    BOOST_CHECK_EQUAL(coloring[0].length, 3);
    BOOST_CHECK_EQUAL(coloring[1].length, 3);
    BOOST_CHECK_EQUAL(coloring[2].length, 3);
    BOOST_CHECK_EQUAL(coloring[3].length, 3);
}

bool is_syntax_error(const LC3AssembleException& ex)
{
    return ex.get_id() == SYNTAX_ERROR;
}

BOOST_FIXTURE_TEST_CASE(TestInstructionPluginAssembleFail, LC3PluginTest)
{
    const std::string asm_file =
    ";@plugin filename=lc3_multiply\n"
    ".orig x3000\n"
    "    MUL R0, R0\n"
    ".end";

    std::stringstream file(asm_file);
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, is_syntax_error);
}

bool is_extra_input(const LC3AssembleException& ex)
{
    return ex.get_id() == EXTRA_INPUT;
}

BOOST_FIXTURE_TEST_CASE(TestInstructionPluginAssembleFail2, LC3PluginTest)
{
    const std::string asm_file =
    ";@plugin filename=lc3_multiply\n"
    ".orig x3000\n"
    "    MUL R0, R0, R3, R4\n"
    ".end";

    std::stringstream file(asm_file);;
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, is_extra_input);
}

BOOST_FIXTURE_TEST_CASE(TestTrapPlugin, LC3PluginTest)
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
    BOOST_REQUIRE_NO_THROW(lc3_assemble(state, file, options));
    BOOST_REQUIRE(state.trapPlugins[0x80] != nullptr);
    BOOST_REQUIRE(lc3_sym_lookup(state, "UDIV") == -1);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xF080));
    lc3_run(state, 4);
    BOOST_REQUIRE(state.halted);
    BOOST_CHECK_EQUAL(state.regs[0], 250);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
}

BOOST_FIXTURE_TEST_CASE(TestTrapPluginViaVector, LC3PluginTest)
{
    const std::string asm_file =
    ";@plugin filename=lc3_udiv vector=x80\n"
    ".orig x3000\n"
    "    LD R0, A\n"
    "    LD R1, B\n"
    "    TRAP x80\n"
    "    HALT\n"
    "A .fill 2000\n"
    "B .fill 8\n"
    ".end";

    std::stringstream file(asm_file);
    BOOST_REQUIRE_NO_THROW(lc3_assemble(state, file, options));
    BOOST_REQUIRE(state.trapPlugins[0x80] != nullptr);
    BOOST_REQUIRE(lc3_sym_lookup(state, "UDIV") == -1);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xF080));
    lc3_run(state, 4);
    BOOST_REQUIRE(state.halted);
    BOOST_CHECK_EQUAL(state.regs[0], 250);
    BOOST_CHECK_EQUAL(state.regs[1], 0);
}

BOOST_FIXTURE_TEST_CASE(TestTrapPluginDivByZero, LC3PluginTest)
{
    const std::string asm_file =
    ";@plugin filename=lc3_udiv vector=x80\n"
    ".orig x3000\n"
    "    AND R0, R0, 0\n"
    "    AND R1, R1, 0\n"
    "    UDIV\n"
    "    HALT\n"
    ".end";

    std::stringstream file(asm_file);
    BOOST_REQUIRE_NO_THROW(lc3_assemble(state, file, options));
    BOOST_REQUIRE(state.trapPlugins[0x80] != nullptr);
    BOOST_REQUIRE(lc3_sym_lookup(state, "UDIV") == -1);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xF080));
    lc3_run(state, 4);
    BOOST_REQUIRE(state.halted);
    BOOST_CHECK_EQUAL(state.regs[0], -1);
    BOOST_CHECK_EQUAL(state.regs[1], -1);
}

BOOST_FIXTURE_TEST_CASE(TestTrapPluginDisassemble, LC3PluginTest)
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
    BOOST_REQUIRE_NO_THROW(lc3_assemble(state, file, options));
    BOOST_REQUIRE(state.trapPlugins[0x80] != nullptr);
    BOOST_REQUIRE(lc3_sym_lookup(state, "UDIV") == -1);
    BOOST_REQUIRE_EQUAL(state.mem[0x3002], short(0xF080));
    BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3002], -1, LC3_BASIC_DISASSEMBLE), "TRAP x80");
    BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3002], -1, LC3_NORMAL_DISASSEMBLE), "UDIV");
    BOOST_CHECK_EQUAL(lc3_disassemble(state, state.mem[0x3002], -1, LC3_ADVANCED_DISASSEMBLE), "UDIV");
}

BOOST_FIXTURE_TEST_CASE(TestTrapPluginAssembleFailure, LC3PluginTest)
{
    const std::string asm_file =
    ";@plugin filename=lc3_udiv\n"
    ".orig x3000\n"
    "   ADD R0, R2, R3\n"
    ".end";

    std::stringstream file(asm_file);
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(PLUGIN_FAILED_TO_LOAD));
}

BOOST_FIXTURE_TEST_CASE(TestTrapPluginAssembleFailure2, LC3PluginTest)
{
    const std::string asm_file =
    ";@plugin filename=lc3_udiv vector=800\n"
    ".orig x3000\n"
    "   ADD R0, R2, R3\n"
    ".end";

    std::stringstream file(asm_file);
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(PLUGIN_FAILED_TO_LOAD));
}