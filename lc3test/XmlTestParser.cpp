#include "XmlTestParser.hpp"
#include <lc3_all.hpp>
#include <sstream>

/** process_str
  *
  * Unescapes and removes quotes from string (see lc3_parser.cpp if you change this)
  */
wxString process_str(const wxString& wxStr, int& error)
{
    // Preappend ""'s
    std::string str = "\"" + wxStr.ToStdString() + "\"";

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

wxXmlNode* getNextNode(wxXmlNode* node)
{
    if (node == NULL) return NULL;
    node = node->GetNext();

    while (node != NULL && node->GetName().IsSameAs("comment"))
    {
        node = node->GetNext();
    }

    return node;
}

int GetCompareType(int type, const wxString& mode, const wxXmlNode* node)
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

    if (mode.IsSameAs("equals", false) || mode.IsSameAs("==") || mode.IsSameAs("="))
    {
        detected_type = -1;
        comp_type = CMP_EQUAL;
    }
    else if (mode.IsSameAs("less", false) || mode.IsSameAs("<"))
    {
        comp_type = CMP_LESS;
    }
    else if (mode.IsSameAs("greater", false) || mode.IsSameAs(">"))
    {
        comp_type = CMP_GREATER;
    }
    else if (mode.IsSameAs("lessorequals", false) || mode.IsSameAs("<="))
    {
        comp_type = CMP_LESSEQUAL;
    }
    else if (mode.IsSameAs("greaterorequals", false) || mode.IsSameAs(">="))
    {
        comp_type = CMP_GREATEREQUAL;
    }
    else if (mode.IsSameAs("notequals", false) || mode.IsSameAs("!="))
    {
        detected_type = -1;
        comp_type = CMP_NOTEQUAL;
    }
    else if (mode.IsSameAs("equalsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_EQUALIC;
    }
    else if (mode.IsSameAs("notequalsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_NOTEQUALIC;
    }
    else if (mode.IsSameAs("contains", false) || mode.IsSameAs("c", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_CONTAINS;
    }
    else if (mode.IsSameAs("notcontains", false) || mode.IsSameAs("!c", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_NOTCONTAINS;
    }
    else if (mode.IsSameAs("containsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_CONTAINSIC;
    }
    else if (mode.IsSameAs("notcontainsignorecase", false))
    {
        detected_type = 1;
        comp_type = CMP_STRING_NOTCONTAINSIC;
    }
    else
        throw XmlTestParserException(wxString::Format("Unknown comparison condition %s", mode), node);

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
        throw XmlTestParserException(wxString::Format("Condition %s is not applicable for %s", mode, node->GetName()), node);

    return comp_type;
}

/** LoadTest
  *
  *
  */
bool XmlTestParser::LoadTestSuite(lc3_test_suite& suite, const std::string filename)
{
    wxXmlDocument doc;
    if (!doc.Load(filename.c_str())) return false;

    wxXmlNode* child = doc.GetRoot()->GetChildren();

    while (child)
    {
        if (child->GetName() != "comment")
        {
            if (child->GetName() != "test-case")
                throw XmlTestParserException("Child elements must be named test-case", child);
            if (!LoadTest(suite, child))
                return false;
        }
        child = getNextNode(child);
    }

    for (const auto& test : suite.tests)
    {
        if (test.max_points != 0 && test.random_seed == -1 && test.randomize)
        {
            printf("Warning for test-case: %s! <random-seed> not set!\nA random seed should be set per test that uses randomization if this xml file is intended to be used as a autograder.\n", test.name.c_str());
        }
    }

    return true;
}

bool XmlTestParser::LoadTest(lc3_test_suite& suite, wxXmlNode* root)
{
    wxXmlNode* child = root->GetChildren();

    lc3_test test;
    while(child)
    {
        if (child->GetName() == "name")
            test.name = child->GetNodeContent();
        else if (child->GetName() == "true-traps")
            test.true_traps = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "has-max-executions")
            test.has_max_executions = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "disable-plugins")
            test.disable_plugins = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "max-executions")
            test.max_executions = wxAtol(child->GetNodeContent());
        else if (child->GetName() == "randomize")
            test.randomize = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "fully-randomize")
            test.fully_randomize = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "random-seed")
        {
            std::string seed = child->GetNodeContent().ToStdString();
            test.random_seed = strtol(seed.c_str(), NULL, 10);
        }
        else if (child->GetName() == "fill-value")
        {
            std::string fill_value = child->GetNodeContent().ToStdString();
            if (fill_value.size() > 1 && fill_value[0] == 'x')
                fill_value = "0" + fill_value;
            test.fill_value = strtol(fill_value.c_str(), NULL, 0);
        }
        else if (child->GetName() == "interrupt-enabled")
            test.interrupt_enabled = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "input")
        {
            if (!LoadTestInput(test, child))
                throw XmlTestParserException("test-input improperly formatted", child);
        }
        else if (child->GetName() == "output")
        {
            if (!LoadTestOutput(test, child))
                throw XmlTestParserException("test-output improperly formatted", child);
        }

        child = getNextNode(child);
    }

    suite.tests.push_back(test);
    return true;
}

bool XmlTestParser::LoadTestInput(lc3_test& test, wxXmlNode* root)
{
    wxXmlNode* child = root->GetChildren();
    int error = 0;

    while(child)
    {
        lc3_test_input input;
        wxXmlNode* grandchild = child->GetChildren();
        if (child->GetName() == "test-value")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    input.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    input.value = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_VALUE;
        }
        else if (child->GetName() == "test-register")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "register")
                    input.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    input.registerval = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_REGISTER;
        }
        else if (child->GetName() == "test-pc")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "value")
                    input.pcval = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_PC;
        }
        else if (child->GetName() == "test-pointer")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    input.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    input.pointer = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_POINTER;
        }
        else if (child->GetName() == "test-string")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    input.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    input.text = process_str(grandchild->GetNodeContent(), error);
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);

                if (error) throw XmlTestParserException(wxString::Format("Malformed string %s in %s", grandchild->GetNodeContent(), child->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_STRING;
        }
        else if (child->GetName() == "test-array")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    input.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                {
                    tokenize(grandchild->GetNodeContent().ToStdString(), input.array, ",");
                    if (input.array.empty())
                        throw XmlTestParserException(wxString::Format("%s must be given at least one element", child->GetName()), grandchild);
                }
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_ARRAY;
        }
        else if (child->GetName() == "test-stdin")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "value")
                    input.io = process_str(grandchild->GetNodeContent(), error);
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                if (error) throw XmlTestParserException(wxString::Format("Malformed string %s in %s", grandchild->GetNodeContent(), child->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_IO;
        }
        else if (child->GetName() == "test-subr")
        {
            lc3_subr_input& subr = input.subroutine;
            while (grandchild)
            {
                if (grandchild->GetName() == "name")
                    subr.name = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "stack")
                    subr.stack = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "r7")
                    subr.r7 = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "r5")
                    subr.r5 = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "params")
                    tokenize(grandchild->GetNodeContent().ToStdString(), subr.params, ",");
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            input.type = TEST_SUBROUTINE;
        }
        else if (child->GetName() != "comment")
        {
            return false;
        }
        else
        {
            child = getNextNode(child);
            continue;
        }
        test.input.push_back(input);
        child = getNextNode(child);
    }

    return true;
}

bool XmlTestParser::LoadTestOutput(lc3_test& test, wxXmlNode* root)
{
    wxXmlNode* child = root->GetChildren();
    int error = 0;

    while(child)
    {
        wxXmlNode* grandchild = child->GetChildren();
        lc3_test_output output;
        output.passed = false;
        output.points = 0;

        wxString points = child->GetAttribute("points", "0");
        wxString mode = child->GetAttribute("condition", "equals");
        output.points = wxAtoi(points);

        if (child->GetName() == "test-value")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    output.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    output.value = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_VALUE;
        }
        else if (child->GetName() == "test-register")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "register")
                    output.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    output.registerval = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_REGISTER;
        }
        else if (child->GetName() == "test-pc")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "value")
                    output.pcval = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_PC;
        }
        else if (child->GetName() == "test-pointer")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    output.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    output.pointer = grandchild->GetNodeContent();
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_POINTER;
        }
        else if (child->GetName() == "test-string")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    output.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                    output.text = process_str(grandchild->GetNodeContent(), error);
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                if (error) throw XmlTestParserException(wxString::Format("Malformed string %s in %s", grandchild->GetNodeContent(), child->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_STRING;
        }
        else if (child->GetName() == "test-array")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "address")
                    output.address = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "value")
                {
                    tokenize(grandchild->GetNodeContent().ToStdString(), output.array, ",");
                    if (output.array.empty())
                        throw XmlTestParserException(wxString::Format("%s must be given at least one element", child->GetName()), grandchild);
                }
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_ARRAY;
        }
        else if (child->GetName() == "test-stdout")
        {
            while (grandchild)
            {
                if (grandchild->GetName() == "value")
                    output.io = process_str(grandchild->GetNodeContent(), error);
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                if (error) throw XmlTestParserException(wxString::Format("Malformed string %s in %s", grandchild->GetNodeContent(), child->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
            }
            output.type = TEST_IO;
        }
        else if (child->GetName() == "test-subr")
        {
            lc3_subr_output& subr = output.subroutine;

            while (grandchild)
            {
                if (grandchild->GetName() == "answer")
                    subr.answer = grandchild->GetNodeContent();
                else if (grandchild->GetName() == "locals")
                    tokenize(grandchild->GetNodeContent().ToStdString(), subr.locals, ",");
                else if (grandchild->GetName() == "calls")
                {
                    wxXmlNode* ggchild = grandchild->GetChildren();
                    while(ggchild)
                    {
                        if (ggchild->GetName() == "call")
                        {
                            wxString required = ggchild->GetAttribute("required", "1");
                            wxXmlNode* gggchild = ggchild->GetChildren();
                            lc3_subr_output_subr_call call;
                            call.required = wxAtoi(required) != 0;
                            while (gggchild)
                            {
                                if (gggchild->GetName() == "name")
                                    call.name = gggchild->GetNodeContent();
                                else if (gggchild->GetName() == "params")
                                    tokenize(gggchild->GetNodeContent().ToStdString(), call.params, ",");
                                else
                                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", ggchild->GetName(), gggchild->GetName()), gggchild);
                                gggchild = getNextNode(gggchild);
                            }
                            subr.calls.push_back(call);
                        }
                        else
                        {
                            throw XmlTestParserException(wxString::Format("2Unknown tag found in %s %s", child->GetName(), ggchild->GetName()), grandchild);
                        }
                        ggchild = getNextNode(ggchild);
                    }
                }
                else if (grandchild->GetName() == "points")
                {
                    wxXmlNode* ggchild = grandchild->GetChildren();
                    while (ggchild)
                    {
                        if (ggchild->GetName() == "answer")
                            subr.points_answer = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "params")
                            subr.points_params = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "r7")
                            subr.points_r7 = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "r6")
                            subr.points_r6 = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "r5")
                            subr.points_r5 = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "locals")
                            subr.points_locals = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "calls")
                            subr.points_calls = wxAtoi(ggchild->GetNodeContent());
                        else if (ggchild->GetName() == "read-answer")
                            subr.points_read_answer = wxAtoi(ggchild->GetNodeContent());
                        else
                            throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), ggchild->GetName()), ggchild);
                        ggchild = getNextNode(ggchild);
                    }
                }
                else if (grandchild->GetName() == "deductions-per-mistake")
                    subr.deductions_edist = wxAtoi(grandchild->GetNodeContent());
                else
                    throw XmlTestParserException(wxString::Format("Unknown tag found in %s %s", child->GetName(), grandchild->GetName()), grandchild);
                grandchild = getNextNode(grandchild);
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
        else if (child->GetName() != "comment")
            return false;
        else
        {
            child = getNextNode(child);
            continue;
        }
        output.cmp_type = GetCompareType(output.type, mode, child);
        test.output.push_back(output);
        child = getNextNode(child);
    }

    return true;
}
