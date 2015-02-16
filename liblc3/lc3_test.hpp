#ifndef LC3_TEST_HPP
#define LC3_TEST_HPP

#include <vector>
#include <string>
#include <sstream>
#include "lc3.hpp"
#include "lc3_cmp.hpp"

enum lc3_test_type
{
    TEST_INVALID = 0,
    TEST_REGISTER,
    TEST_PC,
    TEST_VALUE,
    TEST_POINTER,
    TEST_ARRAY,
    TEST_STRING,
    TEST_IO,
    TEST_SUBROUTINE,
    SIZE,
};

typedef struct lc3_subr_input
{
    std::string name;
    std::string stack;
    std::string r7;
    std::string r5;
    std::vector<std::string> params;
} lc3_subr_input;

typedef struct lc3_subr_output
{
    std::string answer;
    std::vector<std::string> params; // Same as input not present in its xml tag.
    std::string stack; // Same as input not present in its xml tag.
    std::string r7; // Same as input not present in its xml tag.
    std::string r5; // Same as input not present in its xml tag.
    std::vector<std::string> locals;
    unsigned int points_answer;
    unsigned int points_params;
    unsigned int points_r7;
    unsigned int points_r5;
    unsigned int points_locals;
    unsigned int deductions_edist;
} lc3_subr_output;

typedef struct lc3_test_input
{
    unsigned int type;
    std::string address;
    /// TODO only one of these will be used.  Can't do this with a union
    std::string value;
    std::string registerval;
    std::string pcval;
    std::string pointer;
    std::vector<std::string> array;
    std::string text;
    std::string io;
    lc3_subr_input subroutine;
} lc3_test_input;

typedef struct lc3_test_output
{
    unsigned int type;
    unsigned int cmp_type;
    std::string address;
    bool passed;
    unsigned int points;
    /// TODO only one of these will be used.  Can't do this with a union
    std::string value;
    std::string registerval;
    std::string pcval;
    std::string pointer;
    std::vector<std::string> array;
    std::string text;
    std::string io;
    lc3_subr_output subroutine;

    std::string expected;
    std::string actual;
} lc3_test_output;

typedef struct lc3_test
{
    std::string name;
    std::string warning;
    std::vector<lc3_test_input> input;
    std::vector<lc3_test_output> output;
    unsigned int points;
    unsigned int max_points;
    bool passed;
    bool true_traps;
    bool interrupt_enabled;
    bool has_disable_plugins; // Override lc3test parameter
    bool disable_plugins; // With this value
    bool randomize;
    bool has_max_executions;
    bool has_halted;
    unsigned long executions;
    unsigned long max_executions;
    unsigned long warnings;
} lc3_test;

typedef struct lc3_test_suite
{
    bool passed;
    unsigned int points;
    unsigned int max_points;
    std::vector<lc3_test> tests;
} lc3_test_suite;

void lc3_run_test_suite(lc3_test_suite& suite, const std::string& filename, bool disable_plugins = false);
void lc3_run_test_case(lc3_test& test, const std::string& filename, bool disable_plugins = false);
void lc3_write_test_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename);
void lc3_write_test_report(std::stringstream& oss, lc3_test& test, int& minipass_count, int& total_minitests);
std::string lc3_test_input_string(lc3_test_input& test);
std::string lc3_test_output_string(lc3_test_output& test);

#endif
