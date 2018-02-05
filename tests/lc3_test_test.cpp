#include <boost/test/auto_unit_test.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <lc3_all.hpp>
#include "ExpressionEvaluator.hpp"
#include "XmlTestParser.hpp"

struct LC3TestTest
{
    lc3_state state;
    lc3_test_suite suite;

    LC3TestTest()
    {
        lc3_init(state, false, false);
    }
};

BOOST_FIXTURE_TEST_CASE(TestParseXML, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0"?>
    <test-suite>
        <test-case>
            <name>Default</name>
            <true-traps>0</true-traps>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>0</randomize>
            <fully-randomize>1</fully-randomize>
            <random-seed>2110</random-seed>
            <interrupt-enabled>0</interrupt-enabled>
            <strict-execution>0</strict-execution>
            <input>
                <test-value><address>A</address><value>157</value></test-value>
                <test-value><address>A2</address><value>A + 1 - R0</value></test-value>
                <test-pointer><address>B</address><value>7</value></test-pointer>
                <test-register><register>R6</register><value>80</value></test-register>
                <test-pc><value>x5000</value></test-pc>
                <test-array><address>C</address><value>7,5,3,2,3,4,2,3,1,90</value></test-array>
                <test-string><address>D</address><value>HELLO WORLD!</value></test-string>
                <test-stdin><value>HERE IS SOME INPUT</value></test-stdin>
                <test-subr><name>SUBR</name><stack>xF000</stack><r5>xCAFE</r5><r7>x8000</r7><params>1,7,20</params></test-subr>
            </input>
            <output>
                <test-value condition="equals"><address>E</address><value>158</value></test-value>
                <test-pointer><address>F</address><value>8</value></test-pointer>
                <test-register><register>R7</register><value>52</value></test-register>
                <test-pc><value>x3333</value></test-pc>
                <test-array><address>G</address><value>8,6,4,3,4,5,3,4,2,91</value></test-array>
                <test-string points="3"><address>H</address><value>!DLROW OLLEH</value></test-string>
                <test-stdout><value>HERE IS SOME OUTPUT</value></test-stdout>
                <test-subr><answer>0</answer><points><answer>1</answer><params>1</params><r7>1</r7><r6>1</r6><r5>1</r5></points><deductions-per-mistake>1</deductions-per-mistake></test-subr>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    BOOST_REQUIRE_EQUAL(suite.tests.size(), 1);

    lc3_test& test = suite.tests[0];
    BOOST_CHECK_EQUAL(test.name, "Default");
    BOOST_CHECK_EQUAL(test.true_traps, false);
    BOOST_CHECK_EQUAL(test.has_max_executions, true);
    BOOST_CHECK_EQUAL(test.max_executions, 1000000);
    BOOST_CHECK_EQUAL(test.randomize, false);
    BOOST_CHECK_EQUAL(test.fully_randomize, true);
    BOOST_CHECK_EQUAL(test.random_seed, 2110);
    BOOST_CHECK_EQUAL(test.interrupt_enabled, false);
    BOOST_CHECK_EQUAL(test.strict_execution, false);


    BOOST_REQUIRE_EQUAL(test.input.size(), 9);

    BOOST_CHECK_EQUAL(test.input[0].address, "A");
    BOOST_CHECK_EQUAL(test.input[0].type, TEST_VALUE);
    BOOST_CHECK_EQUAL(test.input[0].value, "157");

    BOOST_CHECK_EQUAL(test.input[1].address, "A2");
    BOOST_CHECK_EQUAL(test.input[1].type, TEST_VALUE);
    BOOST_CHECK_EQUAL(test.input[1].value, "A + 1 - R0");

    BOOST_CHECK_EQUAL(test.input[2].address, "B");
    BOOST_CHECK_EQUAL(test.input[2].type, TEST_POINTER);
    BOOST_CHECK_EQUAL(test.input[2].pointer, "7");

    BOOST_CHECK_EQUAL(test.input[3].address, "R6");
    BOOST_CHECK_EQUAL(test.input[3].type, TEST_REGISTER);
    BOOST_CHECK_EQUAL(test.input[3].registerval, "80");

    BOOST_CHECK_EQUAL(test.input[4].type, TEST_PC);
    BOOST_CHECK_EQUAL(test.input[4].pcval, "x5000");

    const std::vector<std::string> answer_array = {"7","5","3","2","3","4","2","3","1","90"};
    BOOST_CHECK_EQUAL(test.input[5].address, "C");
    BOOST_CHECK_EQUAL(test.input[5].type, TEST_ARRAY);
    BOOST_CHECK_EQUAL_COLLECTIONS(test.input[5].array.begin(), test.input[5].array.end(), answer_array.begin(), answer_array.end());

    BOOST_CHECK_EQUAL(test.input[6].address, "D");
    BOOST_CHECK_EQUAL(test.input[6].type, TEST_STRING);
    BOOST_CHECK_EQUAL(test.input[6].text, "HELLO WORLD!");

    BOOST_CHECK_EQUAL(test.input[7].type, TEST_IO);
    BOOST_CHECK_EQUAL(test.input[7].io, "HERE IS SOME INPUT");


    BOOST_REQUIRE_EQUAL(test.input[8].type, TEST_SUBROUTINE);

    lc3_subr_input& subr_input = test.input[8].subroutine;
    BOOST_CHECK_EQUAL(subr_input.name, "SUBR");
    BOOST_CHECK_EQUAL(subr_input.stack, "xF000");
    BOOST_CHECK_EQUAL(subr_input.r5, "xCAFE");
    BOOST_CHECK_EQUAL(subr_input.r7, "x8000");
    const std::vector<std::string> answer_params = {"1", "7", "20"};
    BOOST_CHECK_EQUAL_COLLECTIONS(subr_input.params.begin(), subr_input.params.end(), answer_params.begin(), answer_params.end());

    BOOST_REQUIRE_EQUAL(test.output.size(), 8);

    BOOST_CHECK_EQUAL(test.output[0].address, "E");
    BOOST_CHECK_EQUAL(test.output[0].type, TEST_VALUE);
    BOOST_CHECK_EQUAL(test.output[0].cmp_type, CMP_EQUAL);
    BOOST_CHECK_EQUAL(test.output[0].value, "158");

    BOOST_CHECK_EQUAL(test.output[1].address, "F");
    BOOST_CHECK_EQUAL(test.output[1].type, TEST_POINTER);
    BOOST_CHECK_EQUAL(test.output[1].cmp_type, CMP_EQUAL);
    BOOST_CHECK_EQUAL(test.output[1].pointer, "8");

    BOOST_CHECK_EQUAL(test.output[2].address, "R7");
    BOOST_CHECK_EQUAL(test.output[2].type, TEST_REGISTER);
    BOOST_CHECK_EQUAL(test.output[2].cmp_type, CMP_EQUAL);
    BOOST_CHECK_EQUAL(test.output[2].registerval, "52");

    BOOST_CHECK_EQUAL(test.output[3].type, TEST_PC);
    BOOST_CHECK_EQUAL(test.output[3].cmp_type, CMP_EQUAL);
    BOOST_CHECK_EQUAL(test.output[3].pcval, "x3333");

    const std::vector<std::string> array_answer = {"8","6","4","3","4","5","3","4","2","91"};
    BOOST_CHECK_EQUAL(test.output[4].address, "G");
    BOOST_CHECK_EQUAL(test.output[4].type, TEST_ARRAY);
    BOOST_CHECK_EQUAL(test.output[4].cmp_type, CMP_ARRAY_EQUAL);
    BOOST_CHECK_EQUAL_COLLECTIONS(test.output[4].array.begin(), test.output[4].array.end(), array_answer.begin(), array_answer.end());

    BOOST_CHECK_EQUAL(test.output[5].address, "H");
    BOOST_CHECK_EQUAL(test.output[5].type, TEST_STRING);
    BOOST_CHECK_EQUAL(test.output[5].cmp_type, CMP_STRING_EQUAL);
    BOOST_CHECK_EQUAL(test.output[5].text, "!DLROW OLLEH");
    BOOST_CHECK_EQUAL(test.output[5].points, 3);

    BOOST_CHECK_EQUAL(test.output[6].type, TEST_IO);
    BOOST_CHECK_EQUAL(test.output[6].cmp_type, CMP_STRING_EQUAL);
    BOOST_CHECK_EQUAL(test.output[6].io, "HERE IS SOME OUTPUT");

    BOOST_REQUIRE_EQUAL(test.output[7].type, TEST_SUBROUTINE);

    lc3_subr_output& subr_output = test.output[7].subroutine;
    BOOST_CHECK_EQUAL(subr_output.answer, "0");
    BOOST_CHECK_EQUAL(subr_output.stack, "xF000");
    BOOST_CHECK_EQUAL(subr_output.r5, "xCAFE");
    BOOST_CHECK_EQUAL(subr_output.r7, "x8000");
    BOOST_CHECK(subr_output.locals.empty());
    BOOST_CHECK(subr_output.calls.empty());
    const std::vector<std::string> out_params = {"1", "7", "20"};
    BOOST_CHECK_EQUAL_COLLECTIONS(subr_output.params.begin(), subr_output.params.end(), out_params.begin(), out_params.end());
    BOOST_CHECK_EQUAL(subr_output.points_answer, 1);
    BOOST_CHECK_EQUAL(subr_output.points_params, 1);
    BOOST_CHECK_EQUAL(subr_output.points_r6, 1);
    BOOST_CHECK_EQUAL(subr_output.points_r7, 1);
    BOOST_CHECK_EQUAL(subr_output.points_r5, 1);
    BOOST_CHECK_EQUAL(subr_output.points_locals, 0);
    BOOST_CHECK_EQUAL(subr_output.points_calls, 0);
    BOOST_CHECK_EQUAL(subr_output.points_read_answer, 0);
    BOOST_CHECK_EQUAL(subr_output.deductions_edist, 1);
}

BOOST_FIXTURE_TEST_CASE(SimpleTest, LC3TestTest)
{

    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>ADD_TEST</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <input>
                <test-value><address>A</address><value>1</value></test-value>
                <test-value><address>B</address><value>2</value></test-value>
            </input>
            <output>
                <test-value><address>ANSWER</address><value>3</value></test-value>
            </output>
        </test-case>
        <test-case>
            <name>ADD_TEST 2</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <input>
                <test-value><address>A</address><value>-1</value></test-value>
                <test-value><address>B</address><value>1</value></test-value>
            </input>
            <output>
                <test-value><address>ANSWER</address><value>0</value></test-value>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        LD R0, A
        LD R1, B
        ADD R0, R1, R0
        ST R0, ANSWER
        HALT
        A .blkw 1
        B .blkw 1
        ANSWER .blkw 1
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    BOOST_REQUIRE(suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 2);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 5);

    const lc3_test& test2 = suite.tests[1];
    BOOST_CHECK(test2.passed);
    BOOST_CHECK(test2.has_halted);
    BOOST_CHECK(test2.has_halted_normally);
    BOOST_CHECK_EQUAL(test2.executions, 5);
}

BOOST_FIXTURE_TEST_CASE(PointerTest, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>ADD_PTR_TEST</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <input>
                <test-pointer><address>A</address><value>1</value></test-pointer>
                <test-pointer><address>B</address><value>2</value></test-pointer>
            </input>
            <output>
                <test-pointer><address>ANSWER</address><value>3</value></test-pointer>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        LDI R0, A
        LDI R1, B
        ADD R0, R1, R0
        STI R0, ANSWER
        HALT
        A .fill x4000
        B .fill x4001
        ANSWER .fill x4002
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    BOOST_REQUIRE(suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 1);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 5);
}

BOOST_FIXTURE_TEST_CASE(StringTest, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>ADD_PTR_TEST</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <input>
                <test-string><address>A</address><value>ABC</value></test-string>
            </input>
            <output>
                <test-string><address>A</address><value>ABC</value></test-string>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        HALT
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    BOOST_REQUIRE(suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 1);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 1);
}

BOOST_FIXTURE_TEST_CASE(ArrayTest, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>ADD_PTR_TEST</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <input>
                <test-array><address>A</address><value>0, 1, 2, 3, 4</value></test-array>
            </input>
            <output>
                <test-array><address>A</address><value>0, 1, 2, 3, 4</value></test-array>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        HALT
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    BOOST_REQUIRE(suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 1);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 1);
}



BOOST_FIXTURE_TEST_CASE(OutputTest, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>Hello world test</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <output>
                <test-stdout><value>HELLO WORLD!</value></test-stdout>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        LEA R0, HELLO_WORLD
        PUTS
        HALT
        HELLO_WORLD .stringz "HELLO WORLD!"
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    BOOST_REQUIRE(suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 1);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 3);
}

BOOST_FIXTURE_TEST_CASE(SubroutineTest, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>Hello world test</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <input>
                <test-subr><name>RETURN7</name><stack>xF000</stack><r5>xCAFE</r5><r7>x8000</r7></test-subr>
            </input>
            <output>
                <test-subr><answer>7</answer></test-subr>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        RETURN7
            ADD R6, R6, -3
            STR R5, R6, 0
            STR R7, R6, 1
            AND R5, R5, 0
            ADD R5, R5, 7
            STR R5, R6, 2
            LDR R5, R6, 0
            LDR R7, R6, 1
            ADD R6, R6, 2
            RET
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    std::stringstream oss;
    lc3_write_test_report(oss, suite, "");

    BOOST_REQUIRE(suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 1);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 11); // Plus implicit halt at x8000.
}

BOOST_FIXTURE_TEST_CASE(StrictExecutionTest, LC3TestTest)
{
    const std::string xml = R"(<?xml version="1.0" encoding="UTF-8"?>
    <test-suite>
        <test-case>
            <name>ADD_TEST</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <strict-execution>1</strict-execution>
            <input>
                <test-value><address>A</address><value>1</value></test-value>
                <test-value><address>B</address><value>2</value></test-value>
            </input>
            <output>
                <test-value><address>ANSWER</address><value>3</value></test-value>
            </output>
        </test-case>
        <test-case>
            <name>ADD_TEST 2</name>
            <has-max-executions>1</has-max-executions>
            <max-executions>1000000</max-executions>
            <randomize>1</randomize>
            <fully-randomize>0</fully-randomize>
            <strict-execution>0</strict-execution>
            <input>
                <test-value><address>A</address><value>-1</value></test-value>
                <test-value><address>B</address><value>1</value></test-value>
            </input>
            <output>
                <test-value><address>ANSWER</address><value>0</value></test-value>
            </output>
        </test-case>
    </test-suite>)";

    BOOST_REQUIRE(XmlTestParser().LoadTestSuiteData(suite, xml));

    std::stringstream assembly(R"(
    .orig x3000
        LD R0, A
        LD R1, B
        ADD R0, R1, R0
        .fill 0
        ST R0, ANSWER
        HALT
        A .blkw 1
        B .blkw 1
        ANSWER .blkw 1
    .end
    )");

    lc3_run_test_suite(suite, assembly, 0, 0);

    BOOST_REQUIRE(!suite.passed);
    BOOST_REQUIRE_EQUAL(suite.tests.size(), 2);

    const lc3_test& test = suite.tests[0];
    BOOST_CHECK(test.strict_execution);
    BOOST_CHECK(!test.passed);
    BOOST_CHECK(test.has_halted);
    BOOST_CHECK(!test.has_halted_normally);
    BOOST_CHECK_EQUAL(test.executions, 4);

    const lc3_test& test2 = suite.tests[1];
    BOOST_CHECK(test.strict_execution);
    BOOST_CHECK(test2.passed);
    BOOST_CHECK(test2.has_halted);
    BOOST_CHECK(test2.has_halted_normally);
    BOOST_CHECK_EQUAL(test2.executions, 6);
}
