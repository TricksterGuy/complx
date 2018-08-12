#include "XmlTestParser.hpp"
#include "tinyformat.h"
#include <algorithm>
#include <string>
#include <lc3_all.hpp>

bool isSameAs(std::string a, std::string b, bool caseSensitive=false) {
    if (!caseSensitive) {
        std::transform(a.begin(), a.end(), a.begin(), ::tolower);
        std::transform(b.begin(), b.end(), b.begin(), ::tolower);
    }
    
    return a == b;
}

/** process_str
  *
  * Unescapes and removes quotes from string (see lc3_parser.cpp if you change this)
  */
std::string process_str(const std::string& inStr, int& error)
{
    // Preappend ""'s
    std::string str = "\"" + inStr + "\"";

    std::stringstream out;
    for (size_t i = 1; i < str.size() - 1; i++)
    {
        if (str[i] == '\\')
        {
            i++;
            // hmm
            if (i >= str.size() - 1)
            {
                error = 1;
                goto endstringprocessing;
            }
            char num = 0;
            char buf[4];
            switch(str[i])
            {
            case '\'':
                out << '\'';
                break;
            case '"':
                out << '"';
                break;
            case '\\':
                out << '\\';
                break;
            case 'a':
                out << '\a';
                break;
            case 'b':
                out << '\b';
                break;
            case 'f':
                out << '\f';
                break;
            case 'n':
                out << '\n';
                break;
            case 'r':
                out << '\r';
                break;
            case 't':
                out << '\t';
                break;
            case 'v':
                out << '\v';
                break;
            case 'x':
                i++;
                if (i < str.size() && isxdigit(str[i]))
                {
                    memset(buf, 0, 4);
                    buf[0] = str[i];

                    if (i + 1 < str.size() && isxdigit(str[i + 1]))
                    {
                        i++;
                        buf[1] = str[i];
                    }
                    num = strtol(buf, NULL, 16);
                }
                else
                {
                    error = 1;
                    goto endstringprocessing;
                }
                out << num;
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
                memset(buf, 0, 4);
                buf[0] = str[i];

                if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '7')
                {
                    i++;
                    buf[1] = str[i];
                    if (i + 1 < str.size() && str[i + 1] >= '0' && str[i + 1] <= '7')
                    {
                        i++;
                        buf[2] = str[i];
                    }
                }
                num = strtol(buf, NULL, 8);
                out << num;
                break;
            default:
                error = 1;
                goto endstringprocessing;
                break;
            }
        }
        else
        {
            out << str[i];
        }
    }

endstringprocessing:

    return error ? "" : out.str();
}

int GetCompareType(int type, const std::string& mode, const tinyxml2::XMLElement* node)
{
    // -1, all 0 integral, 1 string, 2 array, 3 subr
    int detected_type = 0;
    int real_type = 0;
    // see lc3_cmp enum
    int comp_type = -1;

    // Get Real type
    switch(type)
    {
    case TEST_VALUE:
        break;
    case TEST_REGISTER:
        break;
    case TEST_PC:
        break;
    case TEST_POINTER:
        break;
    case TEST_STRING:
        real_type = 1;
        break;
    case TEST_ARRAY:
        real_type = 2;
        break;
    case TEST_IO:
        real_type = 1;
        break;
    case TEST_SUBROUTINE:
        real_type = 3;
        break;
    }

    // If test-subr then just set to equal.
    if (real_type == 3)
        return CMP_EQUAL;

    if (isSameAs(mode, "equals", false) || isSameAs(mode, "==") || isSameAs(mode, "="))
    {
        detected_type = -1;
        comp_type = CMP_EQUAL;
    }
    else if (isSameAs(mode, "less", false) || isSameAs(mode, "<"))
    {
        comp_type = CMP_LESS;
    }
    else if (isSameAs(mode, "greater", false) || isSameAs(mode, ">"))
    {
        comp_type = CMP_GREATER;
    }
    else if (isSameAs(mode, "lessorequals", false) || isSameAs(mode, "<="))
    {
        comp_type = CMP_LESSEQUAL;
    }
    else if (isSameAs(mode, "greaterorequals", false) || isSameAs(mode, ">="))
    {
        comp_type = CMP_GREATEREQUAL;
    }
    else if (isSameAs(mode, "notequals", false) || isSameAs(mode, "!="))
    {
        detected_type = -1;
        comp_type = CMP_NOTEQUAL;
    }
    else if (isSameAs(mode, "equalsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_EQUALIC;
    }
    else if (isSameAs(mode, "notequalsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_NOTEQUALIC;
    }
    else if (isSameAs(mode, "contains", false) || isSameAs(mode, "c", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_CONTAINS;
    }
    else if (isSameAs(mode, "notcontains", false) || isSameAs(mode, "!c", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_NOTCONTAINS;
    }
    else if (isSameAs(mode, "containsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_CONTAINSIC;
    }
    else if (isSameAs(mode, "notcontainsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_NOTCONTAINSIC;
    }
    else
        throw XmlTestParserException(tfm::format("Unknown comparison condition %s", mode), node);

    // Special case handling
    if (detected_type == -1 && comp_type == CMP_EQUAL)
    {
        detected_type = real_type;
        if (detected_type == 1)
            comp_type = CMP_STRING_EQUAL;
        else if (detected_type == 2)
            comp_type = CMP_ARRAY_EQUAL;
    }
    else if (detected_type == -1 && comp_type == CMP_NOTEQUAL)
    {
        detected_type = real_type;
        if (detected_type == 1)
            comp_type = CMP_STRING_NOTEQUAL;
        else if (detected_type == 2)
            comp_type = CMP_ARRAY_NOTEQUAL;
    }

    if (detected_type != real_type)
        throw XmlTestParserException(tfm::format("Condition %s is not applicable for %s", mode, node->Name()), node);

    return comp_type;
}

/** LoadTest
  *
  *
  */
bool XmlTestParser::LoadTestSuite(lc3_test_suite& suite, const std::string& filename)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(filename.c_str());

    if (doc.Error()) return false;

    return LoadTestSuite(suite, &doc);
}

bool XmlTestParser::LoadTestSuiteData(lc3_test_suite& suite, const std::string& data)
{
    tinyxml2::XMLDocument doc;
    doc.Parse(data.c_str());

    if (doc.Error()) return false;

    return LoadTestSuite(suite, &doc);
}

bool XmlTestParser::LoadTestSuite(lc3_test_suite& suite, tinyxml2::XMLDocument* doc)
{
    tinyxml2::XMLElement* child = doc->FirstChildElement();

    while (child)
    {
        if (child->Name() != "comment")
        {
            if (child->Name() != "test-case")
                throw XmlTestParserException("Child elements must be named test-case", child);
            if (!LoadTest(suite, child))
                return false;
        }
        child = child->NextSiblingElement();
    }

    for (const auto& test : suite.tests)
    {
        if (test.max_points != 0 && test.random_seed == -1 && test.randomize)
        {
            fprintf(stderr, "Warning for test-case: %s! <random-seed> not set!\nA random seed should be set per test that uses randomization if this xml file is intended to be used as a autograder.\n", test.name.c_str());
        }
    }

    return true;

}

bool XmlTestParser::LoadTest(lc3_test_suite& suite, tinyxml2::XMLElement* root)
{
    tinyxml2::XMLElement* child = root->FirstChildElement();

    lc3_test test;
    while(child)
    {
        if (isSameAs(child->Name(), "name"))
            test.name = child->Value();
        else if (isSameAs(child->Name(), "true-traps"))
            test.true_traps = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "has-max-executions"))
            test.has_max_executions = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "disable-plugins"))
            test.disable_plugins = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "max-executions"))
            test.max_executions = atoi(child->Value());
        else if (isSameAs(child->Name(), "randomize"))
            test.randomize = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "fully-randomize"))
            test.fully_randomize = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "strict-execution"))
            test.strict_execution = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "random-seed"))
        {
            std::string seed = child->Value();
            test.random_seed = strtol(seed.c_str(), NULL, 10);
        }
        else if (isSameAs(child->Name(), "fill-value"))
        {
            std::string fill_value = child->Value();
            if (fill_value.size() > 1 && fill_value[0] == 'x')
                fill_value = "0" + fill_value;
            test.fill_value = strtol(fill_value.c_str(), NULL, 0);
        }
        else if (isSameAs(child->Name(), "interrupt-enabled"))
            test.interrupt_enabled = atoi(child->Value()) != 0;
        else if (isSameAs(child->Name(), "input"))
        {
            if (!LoadTestInput(test, child))
                throw XmlTestParserException("test-input improperly formatted", child);
        }
        else if (isSameAs(child->Name(), "output"))
        {
            if (!LoadTestOutput(test, child))
                throw XmlTestParserException("test-output improperly formatted", child);
        }

        child = child->NextSiblingElement();
    }

    suite.tests.push_back(test);
    return true;
}

bool XmlTestParser::LoadTestInput(lc3_test& test, tinyxml2::XMLElement* root)
{
    tinyxml2::XMLElement* child = root->FirstChildElement();
    int error = 0;

    while(child)
    {
        lc3_test_input input;
        tinyxml2::XMLElement* grandchild = child->FirstChildElement();
        if (isSameAs(child->Name(), "test-value"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    input.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    input.value = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_VALUE;
        }
        else if (isSameAs(child->Name(), "test-register"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "register"))
                    input.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    input.registerval = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_REGISTER;
        }
        else if (isSameAs(child->Name(), "test-pc"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "value"))
                    input.pcval = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_PC;
        }
        else if (isSameAs(child->Name(), "test-pointer"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    input.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    input.pointer = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_POINTER;
        }
        else if (isSameAs(child->Name(), "test-string"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    input.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    input.text = process_str(grandchild->Value(), error);
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);

                if (error) throw XmlTestParserException(tfm::format("Malformed string %s in %s", grandchild->Value(), child->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_STRING;
        }
        else if (isSameAs(child->Name(), "test-array"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    input.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                {
                    tokenize(grandchild->Value(), input.array, ",");
                    if (input.array.empty())
                        throw XmlTestParserException(tfm::format("%s must be given at least one element", child->Name()), grandchild);
                }
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_ARRAY;
        }
        else if (isSameAs(child->Name(), "test-stdin"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "value"))
                    input.io = process_str(grandchild->Value(), error);
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                if (error) throw XmlTestParserException(tfm::format("Malformed string %s in %s", grandchild->Value(), child->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_IO;
        }
        else if (isSameAs(child->Name(), "test-subr"))
        {
            lc3_subr_input& subr = input.subroutine;
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "name"))
                    subr.name = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "stack"))
                    subr.stack = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "r7"))
                    subr.r7 = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "r5"))
                    subr.r5 = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "params"))
                    tokenize(grandchild->Value(), subr.params, ",");
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            input.type = TEST_SUBROUTINE;
        }
        else if (child->Name() != "comment")
        {
            return false;
        }
        else
        {
            child = child->NextSiblingElement();
            continue;
        }
        test.input.push_back(input);
        child = child->NextSiblingElement();
    }

    return true;
}

bool XmlTestParser::LoadTestOutput(lc3_test& test, tinyxml2::XMLElement* root)
{
    tinyxml2::XMLElement* child = root->FirstChildElement();
    int error = 0;

    while(child)
    {
        tinyxml2::XMLElement* grandchild = child->FirstChildElement();
        lc3_test_output output;
        output.passed = false;
        output.points = 0;

        std::string points = child->Attribute("points", "0");
        std::string mode = child->Attribute("condition", "equals");
        output.points = atoi(points.c_str());

        if (isSameAs(child->Name(), "test-value"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    output.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    output.value = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_VALUE;
        }
        else if (isSameAs(child->Name(), "test-register"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "register"))
                    output.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    output.registerval = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_REGISTER;
        }
        else if (isSameAs(child->Name(), "test-pc"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "value"))
                    output.pcval = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_PC;
        }
        else if (isSameAs(child->Name(), "test-pointer"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    output.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    output.pointer = grandchild->Value();
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_POINTER;
        }
        else if (isSameAs(child->Name(), "test-string"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    output.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                    output.text = process_str(grandchild->Value(), error);
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                if (error) throw XmlTestParserException(tfm::format("Malformed string %s in %s", grandchild->Value(), child->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_STRING;
        }
        else if (isSameAs(child->Name(), "test-array"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "address"))
                    output.address = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "value"))
                {
                    tokenize(grandchild->Value(), output.array, ",");
                    if (output.array.empty())
                        throw XmlTestParserException(tfm::format("%s must be given at least one element", child->Name()), grandchild);
                }
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_ARRAY;
        }
        else if (isSameAs(child->Name(), "test-stdout"))
        {
            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "value"))
                    output.io = process_str(grandchild->Value(), error);
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                if (error) throw XmlTestParserException(tfm::format("Malformed string %s in %s", grandchild->Value(), child->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_IO;
        }
        else if (isSameAs(child->Name(), "test-subr"))
        {
            lc3_subr_output& subr = output.subroutine;

            while (grandchild)
            {
                if (isSameAs(grandchild->Name(), "answer"))
                    subr.answer = grandchild->Value();
                else if (isSameAs(grandchild->Name(), "locals"))
                    tokenize(grandchild->Value(), subr.locals, ",");
                else if (isSameAs(grandchild->Name(), "calls"))
                {
                    tinyxml2::XMLElement* ggchild = grandchild->FirstChildElement();
                    while(ggchild)
                    {
                        if (isSameAs(ggchild->Name(), "call"))
                        {
                            tinyxml2::XMLElement* gggchild = ggchild->FirstChildElement();
                            const char* required = ggchild->Attribute("required");
                            int requiredInt = required != nullptr ? atoi(required) : 1;
                            lc3_subr_output_subr_call call;
                            call.required = requiredInt != 0;
                            while (gggchild)
                            {
                                if (isSameAs(gggchild->Name(), "name"))
                                    call.name = gggchild->Value();
                                else if (isSameAs(gggchild->Name(), "params"))
                                    tokenize(gggchild->Value(), call.params, ",");
                                else
                                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", ggchild->Name(), gggchild->Name()), gggchild);
                                gggchild = gggchild->NextSiblingElement();
                            }
                            subr.calls.push_back(call);
                        }
                        else
                        {
                            throw XmlTestParserException(tfm::format("2Unknown tag found in %s %s", child->Name(), ggchild->Name()), grandchild);
                        }
                        ggchild = ggchild->NextSiblingElement();
                    }
                }
                else if (isSameAs(grandchild->Name(), "points"))
                {
                    tinyxml2::XMLElement* ggchild = grandchild->FirstChildElement();
                    while (ggchild)
                    {
                        if (isSameAs(ggchild->Name(), "answer"))
                            subr.points_answer = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "params"))
                            subr.points_params = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "r7"))
                            subr.points_r7 = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "r6"))
                            subr.points_r6 = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "r5"))
                            subr.points_r5 = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "locals"))
                            subr.points_locals = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "calls"))
                            subr.points_calls = atoi(ggchild->Value());
                        else if (isSameAs(ggchild->Name(), "read-answer"))
                            subr.points_read_answer = atoi(ggchild->Value());
                        else
                            throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), ggchild->Name()), ggchild);
                        ggchild = ggchild->NextSiblingElement();
                    }
                }
                else if (isSameAs(grandchild->Name(), "deductions-per-mistake"))
                    subr.deductions_edist = atoi(grandchild->Value());
                else
                    throw XmlTestParserException(tfm::format("Unknown tag found in %s %s", child->Name(), grandchild->Name()), grandchild);
                grandchild = grandchild->NextSiblingElement();
            }
            output.type = TEST_SUBROUTINE;

            // Post processing find test-subr in input and copy attributes over
            lc3_test_input* subr_input = NULL;
            for (unsigned int i = 0; i < test.input.size(); i++)
            {
                lc3_test_input& tinput = test.input[i];
                if (tinput.type == TEST_SUBROUTINE)
                {
                    subr_input = &tinput;
                    break;
                }
            }

            if (!subr_input) throw XmlTestParserException("Used output type of test-subr without specifying subroutine input, Please specify test-input first", child);

            subr.params = subr_input->subroutine.params;
            subr.r7 = subr_input->subroutine.r7;
            subr.r5 = subr_input->subroutine.r5;
            subr.stack = subr_input->subroutine.stack;

            for (const auto& call : subr.calls)
            {
                lc3_subroutine_info info;
                info.name = call.name;
                info.num_params = call.params.size();
                subr_input->subroutine.subroutines.push_back(info);
            }

            // Post processing part II calculate total points
            output.points = subr.points_locals * subr.locals.size() +
                            subr.points_params * subr.params.size() +
                            subr.points_answer + subr.points_r5 + subr.points_r7 + subr.points_r6 +
                            subr.points_read_answer;
            for (const auto& call : subr.calls)
                if (call.required)
                    output.points += subr.points_calls;
        }
        else if (child->Name() != "comment")
            return false;
        else
        {
            child = child->NextSiblingElement();
            continue;
        }
        output.cmp_type = GetCompareType(output.type, mode, child);
        test.output.push_back(output);
        child = child->NextSiblingElement();
    }

    return true;
}
