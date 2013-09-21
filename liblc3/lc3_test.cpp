#include "lc3_test.hpp"
#include "lc3_cmp.hpp"
#include <sstream>

void lc3_run_test_suite(lc3_test_suite& suite, const std::string& filename, bool disable_plugins)
{
    bool passed = true;
    unsigned int total_points = 0;
    unsigned int points = 0;
    for (unsigned int i = 0; i < suite.tests.size(); i++)
    {
        lc3_run_test_case(suite.tests[i], filename);
        passed = passed && suite.tests[i].passed;
        points += suite.tests[i].points;
        total_points += suite.tests[i].max_points;
    }
    suite.passed = passed;
    suite.points = points;
    suite.max_points = total_points;
}

void lc3_run_test_case(lc3_test& test, const std::string& filename, bool disable_plugins)
{
    lc3_state state;
    lc3_init(state);

    // Preliminary stuff
    if (test.randomize) lc3_randomize(state);
    if (test.true_traps) lc3_set_true_traps(state, 1);
    if (test.interrupt_enabled) state.interrupt_enabled = 1;
    if (test.has_disable_plugins) disable_plugins = test.disable_plugins;

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

        if (input.type != TEST_IO && input.type != TEST_REGISTER && input.type != TEST_PC && lc3_calculate(state, input.address, address_calc) == -1)
            throw "An address expression was not formed correctly.";
        else if (input.type == TEST_REGISTER)
        {
            if (input.address.size() != 2)
                throw "Register format is RX where x is between 0-7";

            address_calc = input.address[1] - '0';
            if (address_calc > 7 || address_calc < 0)
                throw "Invalid register";
        }
        else
        {
            lc3_calculate(state, input.address, address_calc);

        }

        effective_address = (unsigned short) address_calc;

        switch (input.type)
        {
            case TEST_VALUE:
                if (lc3_calculate(state, input.value, value_calc))
                    throw "A value expression was malformed.";
                state.mem[effective_address] = (short) value_calc;
                break;
            case TEST_REGISTER:
                if (lc3_calculate(state, input.registerval, value_calc))
                    throw "A value expression was malformed.";
                state.regs[effective_address] = (short) value_calc;
                break;
            case TEST_PC:
                if (lc3_calculate(state, input.pcval, value_calc))
                    throw "A value expression was malformed.";
                state.pc = (unsigned short) value_calc;
                break;
            case TEST_POINTER:
                if (lc3_calculate(state, input.pointer, value_calc))
                    throw "An expression was malformed.";
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
                        throw "An expression was malformed.";
                    state.mem[(unsigned short) (state.mem[effective_address] + j)] = (short) value_calc;
                }
                break;
            case TEST_IO:
                newinput->str(input.io);
                break;
            default:
                throw "Unknown test type";
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
        unsigned int i = 0;
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
            throw "An address expression was not formed correctly.";
        else if (output.type == TEST_REGISTER)
        {
            address_calc = output.address[1] - '0';
            if (address_calc > 7)
                throw "Invalid register";
        }
        effective_address = (unsigned short) address_calc;

        output.passed = true;
        switch (output.type)
        {
            case TEST_VALUE:
                if (lc3_calculate(state, output.value, value_calc))
                    throw "An expression was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.mem[effective_address], &short_cmp);

                actual << state.mem[effective_address];
                expected << short_cmp;
                break;
            case TEST_REGISTER:
                if (lc3_calculate(state, output.registerval, value_calc))
                    throw "An expression was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.regs[effective_address], &short_cmp);

                actual << state.regs[effective_address];
                expected << short_cmp;
                break;
            case TEST_PC:
                if (lc3_calculate(state, output.pcval, value_calc))
                    throw "An expression was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.pc, &short_cmp);

                actual << state.pc;
                expected << short_cmp;

                break;
            case TEST_POINTER:
                if (lc3_calculate(state, output.pointer, value_calc))
                    throw "An expression was malformed.";

                short_cmp = (short) value_calc;
                output.passed = lc3_test_check(output, &state.mem[state.mem[effective_address]], &short_cmp);

                actual << state.mem[state.mem[effective_address]];
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
                        throw "An expression was malformed.";

                    arrayexpected.push_back(state.mem[state.mem[effective_address] + j]);
                    arrayactual.push_back((short)value_calc);

                    actual << state.mem[state.mem[effective_address] + j] << " ";
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
            default:
                throw "Unknown test type";
        }
        test_passed = test_passed && output.passed;

        if (output.passed)
            test_points += output.points;
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
            oss << (j + 1) << " (" << (output.passed ? 'P' : 'F') << " " << (output.passed ? output.points : 0) << " / " << output.points <<
                   ") " << type << "\n  expected (right): " << output.expected << "\n    actual  (left): " << output.actual << "\n";
        }
        else
        {
            oss << (j + 1) << " (" << (output.passed ? 'P' : 'F') << ") " << type << "\n  expected (right): " << output.expected <<
                   "\n    actual  (left): " << output.actual << "\n";
        }

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
    }

    return oss.str();
}
