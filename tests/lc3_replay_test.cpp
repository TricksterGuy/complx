#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <lc3_all.hpp>

#include "lc3_replay.hpp"

void split(const std::string& s, char delimiter, std::vector<std::string>& tokens)
{
    std::stringstream ss(s);
    std::string item;
    while(std::getline(ss, item, delimiter))
        tokens.push_back(item);
}

void lc3_setup_replay(lc3_state& state, std::istream& file, const std::string& replay_string, std::stringstream& newinput);

const std::string REPLAY_STRING = "AQEAAAACAQAAAAMBAAAABAAAAAAFAAAAAAb/////BwCAAAAQEQEAAAA0AQAAAAFAEgAAAAABAAAAAAUTAwAAAEFISAEAAAAHABQEAAAAQkxBSAEAAACmAhUGAAAAQ0FXQ0FXBQAAAAUAAgAJAAAw//8WBAAAAFBBUEEEAAAATQBBAE0AQQAXAAAAAAYAAABSAEEASABSAEEASAAYBAAAAFRBVEEGAAAABQBAQACAAgAFAAcA/w==";

const std::vector<std::string> REPLAY_DESCRIPTIONS = {
"true_traps: on",
"interrupts: on",
"plugins: on",
"strict_execution: off",
"memory_strategy: fill_with_value",
"memory_strategy_value: 4294967295",
"breakpoint: x8000",
"",
"R4 = (16385 x4001)",
"PC = x0500",
"MEM[AHH] = (7 x0007)",
"MEM[MEM[BLAH]] = (678 x02a6)",
"Array at MEM[CAWCAW] = (5 x0005),(2 x0002),(9 x0009),(12288 x3000),(-1 xffff)",
"String at MEM[PAPA] = MAMA",
"Console Input RAHRAH",
"Call Subroutine TATA params: (2 x0002),(5 x0005),(7 x0007) R5 = x0005 R6 = x4040 R7 = x8000",
};

struct LC3ReplayTest
{
    LC3ReplayTest()
    {
        lc3_init(state, false, false);
        options.multiple_errors = false;
    }
    LC3AssembleOptions options;
    lc3_state state;
};

BOOST_FIXTURE_TEST_CASE(ReplayTest, LC3ReplayTest)
{
    const std::string asm_file =
    ".orig x3000\n"
    "   AHH .blkw 1\n"
    "   BLAH .fill x4000\n"
    "   CAWCAW .fill x5000\n"
    "   PAPA .fill x6000\n"
    "   TATA RET\n"
    ".end\n";

    std::stringstream file(asm_file);
    std::stringstream input;
    lc3_setup_replay(state, file, REPLAY_STRING, input);

    BOOST_CHECK_EQUAL(state.regs[4], 0x4001);

    BOOST_CHECK_EQUAL(state.pc, 0x3004);

    BOOST_CHECK_EQUAL(state.mem[0x3000], 7);

    BOOST_CHECK_EQUAL(state.mem[0x4000], 678);

    BOOST_CHECK_EQUAL(state.mem[0x5000], 5);
    BOOST_CHECK_EQUAL(state.mem[0x5001], 2);
    BOOST_CHECK_EQUAL(state.mem[0x5002], 9);
    BOOST_CHECK_EQUAL(state.mem[0x5003], 0x3000);
    BOOST_CHECK_EQUAL((unsigned short)state.mem[0x5004], 0xFFFF);

    BOOST_CHECK_EQUAL(state.mem[0x6000], 'M');
    BOOST_CHECK_EQUAL(state.mem[0x6001], 'A');
    BOOST_CHECK_EQUAL(state.mem[0x6002], 'M');
    BOOST_CHECK_EQUAL(state.mem[0x6003], 'A');
    BOOST_CHECK_EQUAL(state.mem[0x6004], 0);

    BOOST_CHECK_EQUAL(input.str(), "RAHRAH");

    BOOST_CHECK_EQUAL(state.pc, 0x3004);
    BOOST_CHECK_EQUAL(state.regs[5], 5);
    BOOST_CHECK_EQUAL(state.regs[6], 0x4040 - 3);
    BOOST_CHECK_EQUAL((unsigned short)state.regs[7], 0x8000);
    BOOST_CHECK_EQUAL(state.mem[0x4040 - 3], 2);
    BOOST_CHECK_EQUAL(state.mem[0x4040 - 2], 5);
    BOOST_CHECK_EQUAL(state.mem[0x4040 - 1], 7);

    BOOST_CHECK(state.breakpoints.find(0x8000) != state.breakpoints.end());

    BOOST_CHECK(state.true_traps);
    BOOST_CHECK(state.interrupt_enabled);
    BOOST_CHECK(!state.strict_execution);
    BOOST_CHECK_EQUAL(state.mem[0x3010], -1);

}

BOOST_FIXTURE_TEST_CASE(DescribeReplayTest, LC3ReplayTest)
{
    std::string output = lc3_describe_replay(REPLAY_STRING);
    std::vector<std::string> lines;
    split(output, '\n', lines);

    for (unsigned int i = 0; i < lines.size(); i++)
        BOOST_CHECK_EQUAL(lines[i], REPLAY_DESCRIPTIONS[i]);


}
