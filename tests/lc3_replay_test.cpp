#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <lc3_all.hpp>

#include "lc3_replay.hpp"

void lc3_setup_replay(lc3_state& state, std::istream& file, const std::string& replay_string, std::stringstream& newinput);

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
    const std::string replay_str = "AQEAAAACAQAAAAMBAAAABAAAAAAFAAAAAAb/////BwCAAAAQEQEAAAA0AQAAAAFAEgAAAAABAAAAAAUTAwAAAEFISAEAAAAHABQEAAAAQkxBSAEAAACmAhUGAAAAQ0FXQ0FXBQAAAAUAAgAJAAAw//8WBAAAAFBBUEEEAAAATQBBAE0AQQAXAAAAAAYAAABSAEEASABSAEEASAAYBAAAAFRBVEEGAAAABQBAQACAAgAFAAcA/w==";

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
    lc3_setup_replay(state, file, replay_str, input);

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
