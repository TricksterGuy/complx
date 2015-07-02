#include "lc3_test.hpp"
#include "lc3_cmp.hpp"
#include <sstream>

#define ANSWER_FOUND "answer found on stack"
#define R6_FOUND "r6 points to r6-1 (answer location)"
#define R7_FOUND "r7 found on stack and r7 not clobbered"
#define R5_FOUND "r5 found on stack and r5 not clobbered"
#define PARAM_FOUND "param found on stack"
#define LOCAL_FOUND "local found on stack"
#define CHECK "      [✓] "
#define MISS "      [x] "

std::string join(const std::vector<std::string>& vec, const std::string& join)
{
    if (vec.empty()) return "";

    std::stringstream oss;
    for (unsigned int i = 0; i < vec.size() - 1; i++)
        oss << vec[i] << join;
    oss << vec[vec.size() - 1];
    return oss.str();
}

/* Modified edit distance from Wikipedia */
int edit_distance(const std::vector<short>& s, const std::vector<short>& t)
{
    // degenerate cases
    if (s == t) return 0;
    if (s.size() == 0) return t.size();
    if (t.size() == 0) return s.size();

    // create two work vectors of integer distances
    std::vector<unsigned int> v0(t.size() + 1);
    std::vector<unsigned int> v1(t.size() + 1);

    // initialize v0 (the previous row of distances)
    // this row is A[0][i]: edit distance for an empty s
    // the distance is just the number of characters to delete from t
    for (unsigned int i = 0; i < v0.size(); i++)
        v0[i] = i;

    for (unsigned int i = 0; i < s.size(); i++)
    {
        // calculate v1 (current row distances) from the previous row v0

        // first element of v1 is A[i+1][0]
        //   edit distance is delete (i+1) chars from s to match empty t
        v1[0] = i + 1;

        // use formula to fill in the rest of the row
        for (unsigned int j = 0; j < t.size(); j++)
        {
            int cost = (s[i] == t[j]) ? 0 : 1;
            v1[j + 1] = std::min(std::min(v1[j] + 1, v0[j + 1] + 1), v0[j] + cost);
        }

        // copy v1 (current row) to v0 (previous row) for next iteration
        for (unsigned int j = 0; j < v0.size(); j++)
            v0[j] = v1[j];
    }

    return v1[t.size()];
}

void lc3_run_test_suite(lc3_test_suite& suite, const std::string& filename, int seed)
{
    bool passed = true;
    unsigned int total_points = 0;
    unsigned int points = 0;
    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_run_test_case(suite.tests[i], filename, seed);
        passed = passed && suite.tests[i].passed;
        points += suite.tests[i].points;
        total_points += suite.tests[i].max_points;
    }
    suite.passed = passed;
    suite.points = points;
    suite.max_points = total_points;
}

void lc3_run_test_case(lc3_test& test, const std::string& filename, int seed)
{
    lc3_state state;
    lc3_init(state);

    // Preliminary stuff
    if (seed != -1) srand(seed);
    if (test.randomize) lc3_randomize(state);
    if (test.true_traps) lc3_set_true_traps(state, 1);
    if (test.interrupt_enabled) state.interrupt_enabled = 1;
    bool disable_plugins = test.disable_plugins;
    state.max_stack_size = 0;
    state.max_call_stack_size = -1;
    state.in_lc3test = true;

    try
    {
        lc3_assemble(state, filename, false, false, false, false, disable_plugins);
    }
    catch (LC3AssembleException e)
    {
        throw e.what();
    }

    std::stringstream* newinput = new std::stringstream();

    // Set up test environment
    for (unsigned int i = 0; i < test.input.size(); i++)
    {
        lc3_test_input& input = test.input[i];
        int value_calc;
        int address_calc = 0;
        unsigned short effective_address;

        ///TODO flip the condition here so that if new types are added you don't have to check for it here...
        if (input.type != TEST_IO && input.type != TEST_REGISTER && input.type != TEST_PC && input.type != TEST_SUBROUTINE &&
            lc3_calculate(state, input.address, address_calc) == -1)
            throw "An address expression " + input.address + " was not formed correctly.";
        else if (input.type == TEST_REGISTER)
        {
            if (input.address.size() != 2)
                throw "Register format is RX where x is between 0-7 found: " + input.address;

            address_calc = input.address[1] - '0';
            if (address_calc > 7 || address_calc < 0)
                throw "Invalid register " + input.address;
        }
        else
            lc3_calculate(state, input.address, address_calc);

        effective_address = (unsigned short) address_calc;

        switch (input.type)
        {
            case TEST_VALUE:
                if (lc3_calculate(state, input.value, value_calc))
                    throw "<in test-value> A value expression " + input.value + " was malformed.";
                state.mem[effective_address] = (short) value_calc;
                break;
            case TEST_REGISTER:
                if (lc3_calculate(state, input.registerval, value_calc))
                    throw "<in test-register> A value expression " + input.registerval + " was malformed.";
                state.regs[effective_address] = (short) value_calc;
                break;
            case TEST_PC:
                if (lc3_calculate(state, input.pcval, value_calc))
                    throw "<in test-pc> A value expression " + input.pcval + " was malformed.";
                state.pc = (unsigned short) value_calc;
                break;
            case TEST_POINTER:
                if (lc3_calculate(state, input.pointer, value_calc))
                    throw "<in test-pointer> An expression was " + input.pointer + " malformed.";
                state.mem[(unsigned short) state.mem[effective_address]] = (short) value_calc;
                break;
            case TEST_STRING:
                for (unsigned int j = 0; j < input.text.size(); j++)
                    state.mem[state.mem[effective_address] + j] = input.text[j];
                state.mem[(unsigned short) (state.mem[effective_address] + input.text.size())] = 0;
                break;
            case TEST_ARRAY:
                for (unsigned int j = 0; j < input.array.size(); j++)
                {
                    if (lc3_calculate(state, input.array[j], value_calc))
                        throw "<in test-array> An expression was " + input.array[j] + " malformed.";
                    state.mem[(unsigned short) (state.mem[effective_address] + j)] = (short) value_calc;
                }
                break;
            case TEST_IO:
                newinput->str(input.io);
                break;
            case TEST_SUBROUTINE:
                break;
            default:
                throw "Unknown test type";
        }

        if (input.type == TEST_SUBROUTINE)
        {
            lc3_subr_input& subr = input.subroutine;
            int pc;
            int r7;
            int r6;
            int r5;
            if (lc3_calculate(state, subr.name, pc) == -1)
                throw "<in test-subr> invalid subroutine name given " + subr.name;
            if (lc3_calculate(state, subr.stack, r6) == -1)
                throw "<in test-subr> stack expression was malformed " + subr.stack;
            if (lc3_calculate(state, subr.r7, r7) == -1)
                throw "<in test-subr> r7 expression was malformed " + subr.r7;
            if (lc3_calculate(state, subr.r5, r5) == -1)
                throw "<in test-subr> r5 expression was malformed " + subr.r5;

            state.pc = (unsigned short) pc;
            state.regs[6] = (unsigned short)(r6 - subr.params.size());
            state.regs[7] = (unsigned short) r7;
            state.regs[5] = (unsigned short) r5;
            state.mem[state.regs[7]] = 0xF025;

            for (unsigned int j = 0; j < subr.params.size(); j++)
            {
                if (lc3_calculate(state, subr.params[j], value_calc))
                    throw "<in test-subr> param expression " + subr.params[j] + " was malformed.";
                state.mem[(unsigned short)state.regs[6] + j] = (short) value_calc;
            }
        }
    }

    state.input = newinput;
    // Setup output capture device
    std::stringstream* newoutput = new std::stringstream();
    state.output = newoutput;
    std::stringstream* newwarning = new std::stringstream();
    state.warning = newwarning;

    if (test.has_max_executions)
    {
        unsigned long i = 0;
        // Do this num times or until halted.
        while (i < test.max_executions && !state.halted)
        {
            //printf("%04x: %s (%x)\n", state.pc, lc3_disassemble(state, state.mem[state.pc]).c_str(), (unsigned short)state.mem[state.pc]);
                /*printf("R0 %6d|x%04x\tR1 %6d|x%04x\tR2 %6d|x%04x\tR3 %6d|x%04x\nR4 %6d|x%04x\tR5 %6d|x%04x\tR6 %6d|x%04x\tR7 %6d|x%04x\nCC: %s\tPC: %04x\n\n",
                       state.regs[0], (unsigned short)state.regs[0], state.regs[1], (unsigned short)state.regs[1], state.regs[2], (unsigned short)state.regs[2],
                       state.regs[3], (unsigned short)state.regs[3], state.regs[4], (unsigned short)state.regs[4], state.regs[5], (unsigned short)state.regs[5],
                       state.regs[6], (unsigned short)state.regs[6], state.regs[7], (unsigned short)state.regs[7], (state.n ? "N" : (state.z ? "Z" : "P")),
                       (unsigned short) state.pc);*/
            // Step one instruction
            lc3_step(state);
            // Increment instruction count
            i++;
        }
        //lc3_run(state, test.max_executions);
    }
    else
        lc3_run(state);

    // Fill in the output values
    test.has_halted = state.halted;
    test.executions = state.executions;
    test.warnings = state.warnings;
    test.warning = newwarning->str();

    bool test_passed = true;
    unsigned int test_points = 0;
    unsigned int test_max_points = 0;
    // Check test environment
    for (unsigned int i = 0; i < test.output.size(); i++)
    {
        std::stringstream expected;
        std::stringstream actual;

        lc3_test_output& output = test.output[i];
        int value_calc;
        short short_cmp;
        int address_calc = 0;
        unsigned short effective_address;
        std::string str;
        std::vector<short> arrayexpected;
        std::vector<short> arrayactual;

        if (output.type != TEST_IO && output.type != TEST_REGISTER && output.type != TEST_PC && lc3_calculate(state, output.address, address_calc) == -1)
            throw "An address expression " + output.address + " was not formed correctly.";
        else if (output.type == TEST_REGISTER)
        {
            address_calc = output.address[1] - '0';
            if (address_calc > 7)
                throw "Invalid register " + output.address;
        }
        effective_address = (unsigned short) address_calc;

        output.passed = true;
        output.earned = 0;
        switch (output.type)
        {
            case TEST_VALUE:
                if (lc3_calculate(state, output.value, value_calc))
                    throw "<in test-value> An expression " + output.value + " was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.mem[effective_address], &short_cmp);

                actual << state.mem[effective_address];
                expected << short_cmp;
                break;
            case TEST_REGISTER:
                if (lc3_calculate(state, output.registerval, value_calc))
                    throw "<in test-register> An expression " + output.registerval + " was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.regs[effective_address], &short_cmp);

                actual << state.regs[effective_address];
                expected << short_cmp;
                break;
            case TEST_PC:
                if (lc3_calculate(state, output.pcval, value_calc))
                    throw "<in test-pc> An expression " + output.pcval + " was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.pc, &short_cmp);

                actual << state.pc;
                expected << short_cmp;

                break;
            case TEST_POINTER:
                if (lc3_calculate(state, output.pointer, value_calc))
                    throw "<in test-pointer> An expression " + output.pointer + " was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.mem[(unsigned short)state.mem[effective_address]], &short_cmp);

                actual << state.mem[(unsigned short)state.mem[effective_address]];
                expected << short_cmp;

                break;
            case TEST_STRING:
                value_calc = (unsigned short)state.mem[effective_address];
                short_cmp = state.mem[(unsigned short)value_calc];

                while(short_cmp > 0 && short_cmp <= 255)
                {
                    actual.put((char) short_cmp);
                    value_calc++;
                    short_cmp = state.mem[(unsigned short)value_calc];
                }

                str = actual.str();
                output.passed = lc3_test_check(output, &str, &output.text);
                expected << output.text;
                break;
            case TEST_ARRAY:
                for (unsigned int j = 0; j < output.array.size(); j++)
                {
                    if (lc3_calculate(state, output.array[j], value_calc))
                        throw "<in test-array> An expression " + output.array[j] + " was malformed.";

                    arrayexpected.push_back(state.mem[(unsigned short)(state.mem[effective_address] + j)]);
                    arrayactual.push_back((short)value_calc);

                    actual << state.mem[(unsigned short)(state.mem[effective_address] + j)] << " ";
                    expected << (short)value_calc << " ";
                }

                output.passed = lc3_test_check(output, &arrayactual, &arrayexpected);
                break;
            case TEST_IO:
                str = newoutput->str();
                output.passed = lc3_test_check(output, &str, &output.io);
                actual << str;
                expected << output.io;
                break;
            case TEST_SUBROUTINE:
                break;
            default:
                throw "Unknown test type";
        }

        if (output.type != TEST_SUBROUTINE)
        {
            if (output.passed)
                output.earned = output.points;
        }

        if (output.type == TEST_SUBROUTINE)
        {
            std::stringstream extra;
            lc3_subr_output& subr = output.subroutine;

            std::vector<short> expected_stack;
            std::vector<short> actual_stack;

            std::vector<short> locals;
            std::vector<short> params;
            for (int j = (int)subr.locals.size() - 1; j >= 0; j--)
            {
                if (lc3_calculate(state, subr.locals[j], value_calc))
                    throw "<in test-subr> A local variable expression " + subr.locals[i] + "was malformed.";

                expected_stack.push_back((short)value_calc);
                locals.push_back((short) value_calc);
            }

            int r7;
            int r6;
            int r5;
            int answer;
            if (lc3_calculate(state, subr.stack, r6) == -1)
                throw "<in test-subr> stack expression " + subr.stack + " was malformed";
            if (lc3_calculate(state, subr.r7, r7) == -1)
                throw "<in test-subr> r7 expression " + subr.r7 + " was malformed";
            if (lc3_calculate(state, subr.r5, r5) == -1)
                throw "<in test-subr> r5 expression " + subr.r5 + " was malformed";
            if (lc3_calculate(state, subr.answer, answer) == -1)
                throw "<in test-subr> answer expression " + subr.answer + " was malformed";

            expected_stack.push_back((short)r5);
            expected_stack.push_back((short)r7);
            expected_stack.push_back((short)answer);

            for (unsigned int j = 0; j < subr.params.size(); j++)
            {
                if (lc3_calculate(state, subr.params[j], value_calc))
                    throw "<in test-subr> A param expression " + subr.params[j] + " was malformed.";
                expected_stack.push_back((short)value_calc);
                params.push_back((short) value_calc);
            }

            // Code to get the students stack frame
            unsigned short actual_r6 = (unsigned short) r6;
            if (state.call_stack.size() >= 1)
            {
                unsigned short start = state.call_stack[0].r6 + subr.params.size();
                // Screams...
                if (start >= actual_r6)
                    extra << "      [WARNING] Could not get students stack frame.\n"
                             "      Is the student managing the stack correctly?\n";
                else
                    actual_stack.assign(state.mem + start, state.mem + actual_r6);
            }
            else if (state.call_stack.size() == 0)
            {
                int num_params = subr.params.size();
                // Get at least the parameters student could probably not save anything...
                actual_stack.assign(state.mem + (actual_r6 - num_params), state.mem + actual_r6);
                // Get additional addresses modified
                unsigned short start = actual_r6 - num_params - 1;
                while (state.memory_ops.find(start) != state.memory_ops.end())
                {
                    if (!state.memory_ops[start].writes) break;
                    actual_stack.insert(actual_stack.begin(), state.mem[start]);
                    start--;
                }
            }

            for (unsigned int j = 0; j < expected_stack.size(); j++)
                expected << std::hex << "0x" << expected_stack[j] << " ";
            expected << " r5: " << std::hex << "0x" << (short)r5 <<
                        " r6: " << std::hex << "0x" << (actual_r6 - subr.params.size() - 1) <<
                        " r7: " << std::hex << "0x" << (short)(r7 + 1);


            for (unsigned int j = 0; j < actual_stack.size(); j++)
                actual << std::hex << "0x" << actual_stack[j] << " ";
            actual << " r5: " << std::hex << "0x" << state.regs[5] <<
                      " r6: " << std::hex << "0x" << state.regs[6] <<
                      " r7: " << std::hex << "0x" << state.regs[7];

            // now that I have all information available time for some checks.
            std::map<short, int> actual_stack_map;
            for (unsigned int j = 0; j < actual_stack.size(); j++)
                actual_stack_map[actual_stack[j]] += 1;

            int points = 0;
            // If they get something wrong then it will also be wrong in edit distance
            // so cut some slack if you get a value wrong.
            int ed_forgiveness = 0;

            if (actual_stack_map[(short)answer] > 0)
            {
                actual_stack_map[(short)answer] -= 1;
                points += subr.points_answer;
                extra << CHECK << ANSWER_FOUND << " +" << subr.points_answer << "\n";
            }
            else
            {
                ed_forgiveness++;
                extra << MISS << ANSWER_FOUND << " -" << subr.points_answer << "\n";
            }

            if (state.regs[6] == (short)(actual_r6 - subr.params.size() - 1))
            {
                points += subr.points_r6;
                extra << CHECK << R6_FOUND << " +" << subr.points_r6 << "\n";
            }
            else
            {
                extra << MISS << R6_FOUND << " -" << subr.points_r6 << "\n";
            }

            if (actual_stack_map[(short)r7] > 0 && state.regs[7] == (short)(r7+1))
            {
                actual_stack_map[(short)r7] -= 1;
                points += subr.points_r7;
                extra << CHECK << R7_FOUND << " +" << subr.points_r7 << "\n";
            }
            else
            {
                // Don't count if just r7 was clobbered
                if (actual_stack_map[(short)r7] <= 0)
                    ed_forgiveness++;
                extra << MISS << R7_FOUND << " -" << subr.points_r7 << "\n";
            }

            if (actual_stack_map[(short)r5] > 0 && state.regs[5] == (short)r5)
            {
                actual_stack_map[(short)r5] -= 1;
                points += subr.points_r5;
                extra << CHECK << R5_FOUND << " +" << subr.points_r5 << "\n";
            }
            else
            {
                if (actual_stack_map[(short)r5] <= 0)
                    ed_forgiveness++;
                extra << MISS << R5_FOUND << " -" << subr.points_r5 << "\n";
            }

            for (unsigned int j = 0; j < params.size(); j++)
            {
                if (actual_stack_map[params[j]] > 0)
                {
                    actual_stack_map[params[j]] -= 1;
                    points += subr.points_params;
                    extra << CHECK << params[j] << " " << PARAM_FOUND << " +" << subr.points_params << "\n";
                }
                else
                {
                    ed_forgiveness++;
                    extra << MISS << params[j] << " " << PARAM_FOUND << " -" << subr.points_params << "\n";
                }
            }

            bool all_locals_wrong = true;
            for (unsigned int j = 0; j < locals.size(); j++)
            {
                if (actual_stack_map[locals[j]] > 0)
                {
                    actual_stack_map[locals[j]] -= 1;
                    points += subr.points_locals;
                    all_locals_wrong = false;
                    extra << CHECK << locals[j] << " " << LOCAL_FOUND << " +" << subr.points_locals << "\n";
                }
                else
                {
                    ed_forgiveness++;
                    extra << MISS << locals[j] << " " << LOCAL_FOUND << " -" << subr.points_locals << "\n";
                }
            }

            // If all local variables are wrong then it can be argued that they was saving registers
            // And forgot to save locals...
            if (all_locals_wrong)
            {
                // Bro do you even calling convention
                if (actual_stack.size() > subr.params.size() + 3)
                    // Truncate stack to last num_params + 3 elements the stuff we care about (don't do expected since ed forgiveness handles it).
                    actual_stack.erase(actual_stack.begin(), actual_stack.begin() + (actual_stack.size() - subr.params.size() - 3));
                if (!subr.locals.empty())
                    extra << "      All locals were not found, so locals aren't included in structure check\n";
            }
            else
            {
                if (actual_stack.size() > expected_stack.size())
                    expected_stack.insert(expected_stack.begin(), actual_stack.begin(), actual_stack.begin() + (actual_stack.size() - expected_stack.size()));
            }

            int ed_grade = edit_distance(actual_stack, expected_stack);
            points -= (ed_grade - ed_forgiveness) * subr.deductions_edist;
            int mistakes = ed_grade - ed_forgiveness;

            if (mistakes == 0)
                extra << "      Found no structural mistakes in the stack.  No changes needed.\n";
            else
                extra << "      Found " << mistakes << " structural mistakes in stack -" << subr.deductions_edist * mistakes << "\n";

            output.passed = (ed_grade == 0) && (ed_forgiveness == 0);
            output.extra_output += extra.str();
            output.earned = points;
        }

        test_passed = test_passed && output.passed;
        test_points += output.earned;
        test_max_points += output.points;

        output.expected = expected.str();
        output.actual = actual.str();
    }

    // Tear down
    delete state.input;
    delete state.output;
    delete state.warning;

    test.passed = test_passed;
    test.points = test_points;
    test.max_points = test_max_points;
}

std::string get_comp_op(int type)
{
    switch(type)
    {
        case CMP_EQUAL:
            return "==";
        case CMP_LESS:
            return "<";
        case CMP_GREATER:
            return ">";
        case CMP_LESSEQUAL:
            return "<=";
        case CMP_GREATEREQUAL:
            return ">=";
        case CMP_NOTEQUAL:
            return "!=";

        case CMP_STRING_EQUAL:
            return "==";
        case CMP_STRING_NOTEQUAL:
            return "!=";
        case CMP_STRING_EQUALIC:
            return "== (ic)";
        case CMP_STRING_NOTEQUALIC:
            return "!= (ic)";
        case CMP_STRING_CONTAINS:
            return "c";
        case CMP_STRING_NOTCONTAINS:
            return "!c";
        case CMP_STRING_CONTAINSIC:
            return "c (ic)";
        case CMP_STRING_NOTCONTAINSIC:
            return "!c (ic)";

        case CMP_ARRAY_EQUAL:
            return "==";
        case CMP_ARRAY_NOTEQUAL:
            return "!=";
        default:
            return "==";
    }
}

void lc3_write_test_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename)
{
    int pass_count = 0;
    int minipass_count = 0;
    int total_minitests = 0;

    oss << "Test suite report for file " << filename << "\n";
    oss << "-------------------------------------\n\n";

    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_test& test = suite.tests[i];
        lc3_write_test_report(oss, test, minipass_count, total_minitests);
        if (test.passed) pass_count++;
    }

    oss << "Summary\n";
    oss << "-------\n";
    oss << "Checks passed: " << minipass_count << " / " << total_minitests << "\n";
    oss << "Tests passed: " << pass_count << " / " << suite.tests.size() << "\n";
    if (suite.max_points) oss << "Grade: " << suite.points << " / " << suite.max_points << "\n";
}

void lc3_write_test_report(std::stringstream& oss, lc3_test& test, int& minipass_count, int& total_minitests)
{
    int pass_count = 0;
    if (test.max_points)
    {
        oss << (test.passed ? "Passed" : "Failed") << " Test case - " << test.name << " " <<
                test.points << " / " << test.max_points << " " << (test.has_halted ? "" : "(Did not finish!)") << "\n";
    }
    else
    {
        oss << (test.passed ? "Passed" : "Failed") << " Test case - " << test.name << " " <<
               (test.has_halted ? "" : "(Did not finish!)") << "\n";
    }

    if (test.passed) pass_count++;
    oss << "----------------------\n";

    for (unsigned int j = 0; j < test.output.size(); j++)
    {
        lc3_test_output& output = test.output[j];
        std::string type = lc3_test_output_string(output);

        if (output.points)
        {
            oss << (j + 1) << " (" << (output.passed ? 'P' : 'F') << " " << output.earned << " / " << output.points <<
                   ") " << type << "\n  expected (right): " << output.expected << "\n    actual  (left): " << output.actual << "\n";
        }
        else
        {
            oss << (j + 1) << " (" << (output.passed ? 'P' : 'F') << ") " << type << "\n  expected (right): " << output.expected <<
                   "\n    actual  (left): " << output.actual << "\n";
        }

        if (!output.extra_output.empty())
            oss << output.extra_output;

        if (output.passed) minipass_count++;
        total_minitests++;
    }

    if (test.warnings)
    {
        oss << "\nTest completed with " << test.warnings <<" warnings\n";
        oss << "-----------------------------------\n";
        oss << test.warning;
        oss << "\n";
    }

    oss << "\n";
}

std::string lc3_test_input_string(lc3_test_input& test)
{
    std::stringstream oss;

    switch(test.type)
    {
        case TEST_VALUE:
            oss << "MEM[";
            oss << test.address;
            oss << "] = ";
            oss << test.value;
            break;
        case TEST_REGISTER:
            oss << test.address;
            oss << " = ";
            oss << test.registerval;
            break;
        case TEST_PC:
            oss << "PC = ";
            oss << test.pcval;
            break;
        case TEST_POINTER:
            oss << "MEM[MEM[";
            oss << test.address;
            oss << "]] = ";
            oss << test.pointer;
            break;
        case TEST_ARRAY:
            oss << "Array at MEM[";
            oss << test.address;
            oss << "] = ";
            for (unsigned int i = 0; i < test.array.size() - 1; i++)
            {
                oss << test.array[i];
                oss << ", ";
            }
            oss << test.array[test.array.size() - 1];
            break;
        case TEST_STRING:
            oss << "String at MEM[";
            oss << test.address;
            oss << "] = ";
            oss << test.text;
            break;
        case TEST_IO:
            oss << "Input: ";
            oss << test.io;
            break;
        case TEST_SUBROUTINE:
            oss << "Call ";
            oss << test.subroutine.name;
            oss << "(";
            oss << join(test.subroutine.params, ",");
            oss << ")";
            break;
    }

    return oss.str();
}

std::string lc3_test_output_string(lc3_test_output& test)
{
    std::stringstream oss;

    switch(test.type)
    {
        case TEST_VALUE:
            oss << "MEM[";
            oss << test.address;
            oss << "] " << get_comp_op(test.cmp_type) << " ";
            oss << test.value;
            break;
        case TEST_REGISTER:
            oss << test.address;
            oss << " " << get_comp_op(test.cmp_type) << " ";
            oss << test.registerval;
            break;
        case TEST_PC:
            oss << "PC " << get_comp_op(test.cmp_type) << " ";
            oss << test.pcval;
            break;
        case TEST_POINTER:
            oss << "MEM[MEM[";
            oss << test.address;
            oss << "]] " << get_comp_op(test.cmp_type) << " ";
            oss << test.pointer;
            break;
        case TEST_ARRAY:
            oss << "Array at MEM[";
            oss << test.address;
            oss << "] " << get_comp_op(test.cmp_type) << " [";
            for (unsigned int i = 0; i < test.array.size() - 1; i++)
            {
                oss << test.array[i];
                oss << ", ";
            }
            oss << test.array[test.array.size() - 1];
            oss << "]";
            break;
        case TEST_STRING:
            oss << "String at MEM[";
            oss << test.address;
            oss << "] " << get_comp_op(test.cmp_type) << " \"";
            oss << test.text << "\"";
            break;
        case TEST_IO:
            oss << "Output " << get_comp_op(test.cmp_type) << " ";
            oss << test.io;
            break;
        case TEST_SUBROUTINE:
            oss << "Answer " << test.subroutine.answer << " calling convention followed";
            break;
    }

    return oss.str();
}
