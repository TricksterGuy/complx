#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <lc3_all.hpp>

#define IS_EXCEPTION(type) [](const LC3AssembleException& e) {/*fprintf(stderr, "Exception: %s\n", e.what().c_str()); */return e.get_id() == type;}

struct LC3Revision2019Test
{
    lc3_state state;
    LC3AssembleOptions options;

    LC3Revision2019Test()
    {
        lc3_init(state, false, false);
        state.lc3_version = 1;
        options.multiple_errors = false;
    }
};

BOOST_FIXTURE_TEST_CASE(TestLea, LC3Revision2019Test)
{
    lc3_instr instr;
    // LEA R0, #-1      : op, reg, pc_offset
    memoryoffset_instr lea = (memoryoffset_instr){LEA_INSTR, 0, -1};
    instr.mem.offset = lea;
    state.n = 1;
    state.z = 0;
    state.p = 0;

    lc3_execute(state, instr);

    // LEA no longer changes the CC register.
    BOOST_CHECK_EQUAL(state.regs[0], 0x2FFF);
    BOOST_CHECK_EQUAL(state.n, 1);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 0);
}

BOOST_FIXTURE_TEST_CASE(TestCustomTrap, LC3Revision2019Test)
{
    std::istringstream file(
        ".orig x30\n"
        ".fill x1000\n"
        ".end\n\n"

        ".orig x1000\n"
        "AND R0, R0, 0\n"
        "RTI\n"
        ".end\n\n"

        ".orig x3000\n"
        "TRAP x30\n"
        ".end\n\n"
    );
    lc3_assemble(state, file, options);
    lc3_set_true_traps(state, true);
    state.n = 0;
    state.z = 0;
    state.p = 1;

    BOOST_REQUIRE_EQUAL(state.privilege, 1);
    // ========
    // TRAP x30
    // ========
    short r6 = state.regs[6];
    short r7 = state.regs[7];

    lc3_step(state);
    // PSR check.
    BOOST_CHECK_EQUAL(state.privilege, 0);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    BOOST_CHECK_EQUAL(state.pc, 0x1000);
    // TRAP no longer sets R7. Return address is automatically pushed on the stack now.
    BOOST_CHECK_EQUAL(state.regs[7], r7);
    BOOST_CHECK_EQUAL(state.regs[6], state.savedssp - 2);
    BOOST_CHECK_EQUAL(state.mem[static_cast<unsigned short>(state.regs[6])], 0x3001);
    BOOST_CHECK_EQUAL(state.mem[static_cast<unsigned short>(state.regs[6] + 1)], static_cast<short>(0x8001));
    BOOST_CHECK_EQUAL(state.savedusp, r6);

    // Internal state checks
    BOOST_REQUIRE_EQUAL(state.undo_stack.size(), 1);
    lc3_state_change change = state.undo_stack.back();
    BOOST_CHECK_EQUAL(change.changes, LC3_SUBROUTINE_BEGIN);
    BOOST_CHECK_EQUAL(change.privilege, 1);
    BOOST_CHECK_EQUAL(change.pc, 0x3001);
    BOOST_CHECK_EQUAL(change.subroutine.is_trap, true);
    BOOST_CHECK_EQUAL(change.subroutine.address, 0x1000);

    BOOST_REQUIRE_EQUAL(state.rti_stack.size(), 1);
    lc3_rti_stack_item item = state.rti_stack.back();
    BOOST_CHECK_EQUAL(item.is_interrupt, false);

    BOOST_REQUIRE_EQUAL(state.call_stack.size(), 1);
    lc3_subroutine_call call = state.call_stack.back();
    BOOST_CHECK_EQUAL(call.address, 0x1000);
    BOOST_CHECK_EQUAL(call.is_trap, true);

    // =============
    // AND R0, R0, 0
    // =============
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.regs[0], 0);
    BOOST_CHECK_EQUAL(state.pc, 0x1001);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 1);
    BOOST_CHECK_EQUAL(state.p, 0);

    // ===
    // RTI
    // ===
    lc3_step(state);
    BOOST_CHECK_EQUAL(state.privilege, 1);
    BOOST_CHECK_EQUAL(state.n, 0);
    BOOST_CHECK_EQUAL(state.z, 0);
    BOOST_CHECK_EQUAL(state.p, 1);

    BOOST_CHECK_EQUAL(state.pc, 0x3001);
    BOOST_CHECK_EQUAL(state.regs[6], r6);
    BOOST_CHECK_EQUAL(state.savedssp, 0x3000);
    BOOST_CHECK_EQUAL(state.savedusp, r6);

    // Internal state checks
    BOOST_REQUIRE_EQUAL(state.undo_stack.size(), 3);
    change = state.undo_stack.back();
    BOOST_CHECK_EQUAL(change.changes, LC3_SUBROUTINE_END);
    BOOST_CHECK_EQUAL(change.privilege, 0);
    BOOST_CHECK_EQUAL(change.pc, 0x1002);

    BOOST_CHECK(state.rti_stack.empty());
    BOOST_CHECK(state.call_stack.empty());
}

BOOST_FIXTURE_TEST_CASE(TestCustomMultiTrap, LC3Revision2019Test)
{
    std::istringstream file(
        ".orig x30\n"
        ".fill x1000\n"
        ".fill x1010\n"
        ".fill x1020\n"
        ".end\n\n"

        ".orig x1000\n"
        "AND R0, R0, 0\n"
        "TRAP x31\n"
        "RTI\n"
        ".end\n\n"

        ".orig x1010\n"
        "ADD R0, R0, -1\n"
        "TRAP x32\n"
        "RTI\n"
        ".end\n\n"

        ".orig x1020\n"
        "RTI\n"
        ".end\n\n"

        ".orig x3000\n"
        "TRAP x30\n"
        ".end\n\n"
    );
    lc3_assemble(state, file, options);
    lc3_set_true_traps(state, true);
    state.n = 0;
    state.z = 0;
    state.p = 1;

    BOOST_REQUIRE_EQUAL(state.privilege, 1);
    lc3_run(state, 5); // x1020.

    BOOST_CHECK_EQUAL(state.pc, 0x1020);
    // System Stack Check.
    BOOST_CHECK_EQUAL(state.mem[0x2FFF], static_cast<short>(0x8001));
    BOOST_CHECK_EQUAL(state.mem[0x2FFE], 0x3001);

    BOOST_CHECK_EQUAL(state.mem[0x2FFD], static_cast<short>(0x0002));
    BOOST_CHECK_EQUAL(state.mem[0x2FFC], 0x1002);

    BOOST_CHECK_EQUAL(state.mem[0x2FFB], static_cast<short>(0x0004));
    BOOST_CHECK_EQUAL(state.mem[0x2FFA], 0x1012);

    // Internal state checks
    BOOST_CHECK_EQUAL(state.rti_stack.size(), 3);
    BOOST_CHECK_EQUAL(state.call_stack.size(), 3);

    BOOST_CHECK_EQUAL(state.rti_stack[0].is_interrupt, false);
    BOOST_CHECK_EQUAL(state.rti_stack[1].is_interrupt, false);
    BOOST_CHECK_EQUAL(state.rti_stack[2].is_interrupt, false);

    BOOST_CHECK_EQUAL(state.call_stack[0].address, 0x1000);
    BOOST_CHECK_EQUAL(state.call_stack[0].is_trap, true);
    BOOST_CHECK_EQUAL(state.call_stack[1].address, 0x1010);
    BOOST_CHECK_EQUAL(state.call_stack[1].is_trap, true);
    BOOST_CHECK_EQUAL(state.call_stack[2].address, 0x1020);
    BOOST_CHECK_EQUAL(state.call_stack[2].is_trap, true);
}

BOOST_FIXTURE_TEST_CASE(LC3DefaultVersionTest, LC3Revision2019Test)
{
    std::istringstream file(
        ".orig x3000\n"
        "HALT\n"
        ".end"
    );

    lc3_init(state, false, false);
    lc3_assemble(state, file, options);

    BOOST_CHECK_EQUAL(state.lc3_version, 0);
}

BOOST_FIXTURE_TEST_CASE(LC3VersionTest, LC3Revision2019Test)
{
    std::istringstream file(
        ";@version 1\n"
        ".orig x3000\n"
        "HALT\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_CHECK_EQUAL(state.lc3_version, 1);
}

BOOST_FIXTURE_TEST_CASE(LC3VersionTest2, LC3Revision2019Test)
{
    std::istringstream file(
        ";@version 0\n"
        ".orig x3000\n"
        "HALT\n"
        ".end"
    );
    lc3_assemble(state, file, options);

    BOOST_CHECK_EQUAL(state.lc3_version, 0);
}

BOOST_FIXTURE_TEST_CASE(InvalidLC3VersionTest, LC3Revision2019Test)
{
    std::istringstream file(
        ";@version 15\n"
        ".orig x3000\n"
        "HALT\n"
        ".end"
    );
    BOOST_CHECK_EXCEPTION(lc3_assemble(state, file, options), LC3AssembleException, IS_EXCEPTION(INVALID_LC3_VERSION));
}

