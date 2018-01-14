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
