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
    TEST_SIZE,
};

typedef struct lc3_subr_input
{
    std::string name;
    std::string stack;
    std::string r7;
    std::string r5;
    std::vector<std::string> params;
    std::vector<lc3_subroutine_info> subroutines; // Will be filled out by <calls> in test-subr output.
} lc3_subr_input;

typedef struct lc3_subr_output_subr_call
{
    std::string name;
    std::vector<std::string> params;
} lc3_subr_output_subr_call;

typedef struct lc3_subr_output
{
    lc3_subr_output() : points_answer(0), points_params(0), points_r6(0),  points_r7(0), points_r5(0), points_locals(0),
    points_calls(0), points_read_answer(0), deductions_edist(0) {}
    std::string answer;
    std::vector<std::string> params; // Same as input not present in its xml tag.
    std::string stack; // Same as input not present in its xml tag.
    std::string r7; // Same as input not present in its xml tag.
    std::string r5; // Same as input not present in its xml tag.
    std::vector<std::string> locals;
    std::vector<lc3_subr_output_subr_call> calls;
    unsigned int points_answer;
    unsigned int points_params;
    unsigned int points_r6;
    unsigned int points_r7;
    unsigned int points_r5;
    unsigned int points_locals;
    unsigned int points_calls;
    unsigned int points_read_answer;
    unsigned int deductions_edist;
} lc3_subr_output;

typedef struct lc3_test_input
{
    lc3_test_input() : type(0) {}
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
    lc3_test_output() : type(0), cmp_type(0), passed(false), earned(0), points(0) {}
    unsigned int type;
    unsigned int cmp_type;
    std::string address;
    bool passed;
    unsigned int earned;
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
    std::string extra_output;
} lc3_test_output;

typedef struct lc3_test
{
    lc3_test() : points(0), max_points(0), passed(false), true_traps(false), interrupt_enabled(false),
                 disable_plugins(false), randomize(false), has_max_executions(false), has_halted(false),
                 executions(0), max_executions(0), warnings(0) {}
    std::string name;
    std::string warning;
    std::vector<lc3_test_input> input;
    std::vector<lc3_test_output> output;
    unsigned int points;
    unsigned int max_points;
    bool passed;
    bool true_traps;
    bool interrupt_enabled;
    bool disable_plugins;
    bool randomize;
    bool has_max_executions;
    bool has_halted;
    unsigned long executions;
    unsigned long max_executions;
    unsigned long warnings;
} lc3_test;

typedef struct lc3_test_suite
{
    lc3_test_suite() : passed(false), points(0), max_points(0) {}
    bool passed;
    unsigned int points;
    unsigned int max_points;
    std::vector<lc3_test> tests;
} lc3_test_suite;

void lc3_run_test_suite(lc3_test_suite& suite, const std::string& filename, int seed = -1);
void lc3_run_test_case(lc3_test& test, const std::string& filename, int seed = -1);
void lc3_write_test_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename);
void lc3_write_test_report(std::stringstream& oss, lc3_test& test, int& minipass_count, int& total_minitests);
std::string lc3_test_input_string(lc3_test_input& test);
std::string lc3_test_output_string(lc3_test_output& test);

#endif
