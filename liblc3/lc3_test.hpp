#ifndef LC3_TEST_HPP
#define LC3_TEST_HPP

#include <vector>
#include <string>
#include <sstream>
#include "lc3.hpp"
#include "lc3_cmp.hpp"

/** Type of test item */
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

/** Record of lc3 subroutine precondition */
typedef struct lc3_subr_input
{
    std::string name;
    std::string stack;
    std::string r7;
    std::string r5;
    std::vector<std::string> params;
    std::vector<lc3_subroutine_info> subroutines; // Will be filled out by <calls> in test-subr output.
} lc3_subr_input;

/** Record of expected subroutine call info */
typedef struct lc3_subr_output_subr_call
{
    std::string name;
    std::vector<std::string> params;
    bool required;
} lc3_subr_output_subr_call;

/** Record of lc3 subroutine postconditions to check */
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

/** Main record for a single precondition for a given test. */
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

/** Main record for a single postcondition to check for a given test. */
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

/** Main record for a given test case. */
typedef struct lc3_test
{
    lc3_test() : points(0), max_points(0), passed(false), true_traps(false), interrupt_enabled(false),
        disable_plugins(false), randomize(false), fully_randomize(false), has_max_executions(false), has_halted(false), has_halted_normally(false),
        random_seed(-1), fill_value(-1), executions(0), max_executions(0), warnings(0) {}
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
    bool fully_randomize;
    bool has_max_executions;
    bool has_halted;
    bool has_halted_normally; // Halt via the instruction.
    long random_seed;
    int fill_value;
    unsigned long executions;
    unsigned long max_executions;
    unsigned long warnings;
} lc3_test;

/** Main record for a suite of tests */
typedef struct lc3_test_suite
{
    lc3_test_suite() : passed(false), points(0), max_points(0) {}
    bool passed;
    unsigned int points;
    unsigned int max_points;
    std::vector<lc3_test> tests;
} lc3_test_suite;

/** lc3_run_test_suite
  *
  * Runs an entire test suite.
  * @param suite An lc3_test_suite, the results will be written to the object.
  * @param filename Path to an assembly file.
  * @param seed Random seed to use.
  * @param run Run number.
  */
void lc3_run_test_suite(lc3_test_suite& suite, const std::string& filename, int seed = -1, int run = 0);
/** lc3_init_test_case
  *
  * Initializes and loads assembly file and sets up lc3_state to match the test's environment.
  * @param state LC3State object.
  * @param filename Path to an assembly file.
  * @param test An lc3_test.
  * @param seed Random seed to use.
  * @param in_lc3_test true if this is being called via lc3_test binary. (The effect if true is that backstepping is disabled).
  * @param run Run number. Random seed used in test will be incremented by this number
  */
void lc3_init_test_case(lc3_state& state, const std::string& filename, lc3_test& test, int seed = -1, bool in_lc3_test = false, int run = 0);
/** lc3_setup_test_case
  *
  * Sets up a single test case by applying all of the input conditions to the current lc3_state.
  * @param state LC3State object.
  * @param test An lc3_test.
  * @param input Input stream to set up.
  */
void lc3_setup_test_case(lc3_state& state, lc3_test& test, std::stringstream& input);
/** lc3_run_test_case
  *
  * Runs a single test case.
  * @param test An lc3_test, the results will be written to the object.
  * @param filename Path to an assembly file.
  * @param seed Random seed to use.
  */
void lc3_run_test_case(lc3_test& test, const std::string& filename, int seed = -1, int run = 0);

/** lc3_write_test_report
  *
  * Writes a test suite report to the stringstream given.
  * @param oss Output stringstream to write to.
  * @param suite An lc3_test_suite lc3_run_test_suite must be called previously on the suite.
  * @param filename Path to an assembly file.
  */
void lc3_write_test_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename);
/** lc3_write_test_report
  *
  * Writes a test case report to the stringstream given.
  * @param oss Output stringstream to write to.
  * @param test An lc3_test lc3_run_test_case must be called previously on the test.
  * @param minipass_count Output param for how many postconditions were passed.
  * @param total_minitests Output param for total number of postconditions in test case.
  */
void lc3_write_test_report(std::stringstream& oss, lc3_test& test, int& minipass_count, int& total_minitests);

/** lc3_test_input_string
  *
  * Gives a human readable version of the test input precondition.
  * @param test lc3 test input precondition.
  * @return Human readable version of the test input precondition.
  */
std::string lc3_test_input_string(const lc3_test_input& test);
/** lc3_test_output_string
  *
  * Gives a human readable version of the test output postcondition.
  * @param test lc3 test output postcondition.
  * @return Human readable version of the test output postcondition.
  */
std::string lc3_test_output_string(const lc3_test_output& test);

#endif
