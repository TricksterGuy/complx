#include "XmlTestParser.hpp"
#include <sstream>

/** process_str
  *
  * Unescapes and removes quotes from string (see lc3_parser.cpp if you change this)
  */
std::string process_str(wxString str, int& error)
{
    return process_str(str.ToStdString(), error);
}
std::string process_str(std::string str, int& error)
{
    // Preappend ""'s
    str = "\"" + str + "\"";

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
                case '\'': out << '\''; break;
                case '"': out << '"'; break;
                case '\\': out << '\\'; break;
                case 'a': out << '\a'; break;
                case 'b': out << '\b'; break;
                case 'f': out << '\f'; break;
                case 'n': out << '\n'; break;
                case 'r': out << '\r'; break;
                case 't': out << '\t'; break;
                case 'v': out << '\v'; break;
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

/** ~XmlTestParser
  *
  * Destructor
  */
XmlTestParser::~XmlTestParser()
{

}

/** LoadTest
  *
  *
  */
bool XmlTestParser::LoadTestSuite(lc3_test_suite& suite, const std::string filename)
{
    wxXmlDocument doc;
    if (!doc.Load(filename.c_str())) return false;

    suite.passed = false;
    suite.points = 0;
    suite.max_points = 0;

    wxXmlNode* child = doc.GetRoot()->GetChildren();

    while (child)
    {
        if (child->GetName() != "comment")
        {
            if (child->GetName() != "test-case") throw "Child elements must be named test-case";
            if (!LoadTest(suite, child)) return false;
        }
        child = getNextNode(child);
    }

    return true;
}

bool XmlTestParser::LoadTest(lc3_test_suite& suite, wxXmlNode* root)
{
    wxXmlNode* child = root->GetChildren();

    lc3_test test;

    test.has_halted = false;
    test.executions = 0;
    test.has_max_executions = false;
    test.has_disable_plugins = false;
    test.disable_plugins = false;
    test.max_executions = 0;
    test.passed = false;
    test.randomize = false;
    test.true_traps = false;
    test.interrupt_enabled = false;
    test.warnings = false;
    test.points = 0;
    test.max_points = 0;

    while(child)
    {
        if (child->GetName() == "name")
            test.name = child->GetNodeContent();
        else if (child->GetName() == "true-traps")
            test.true_traps = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "has-max-executions")
            test.has_max_executions = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "has-disable-plugins")
            test.has_disable_plugins = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "disable-plugins")
            test.disable_plugins = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "max-executions")
            test.max_executions = wxAtol(child->GetNodeContent());
        else if (child->GetName() == "randomize")
            test.randomize = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "interrupt-enabled")
            test.interrupt_enabled = wxAtoi(child->GetNodeContent()) != 0;
        else if (child->GetName() == "input")
        {
            if (!LoadTestInput(test, child)) throw "test-input improperly formatted";
        }
        else if (child->GetName() == "output")
        {
            if (test.input.empty()) throw "test-input must come before test-output";
            if (!LoadTestOutput(test, child)) throw "test-output improperly formatted";
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
            if (grandchild->GetName() != "address") throw "address must come before value";
            input.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            input.value = grandchild->GetNodeContent();

            input.type = TEST_VALUE;
        }
        else if (child->GetName() == "test-register")
        {
            if (grandchild->GetName() != "register") throw "register must come before value";
            input.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            input.registerval = grandchild->GetNodeContent();

            input.type = TEST_REGISTER;
        }
        else if (child->GetName() == "test-pc")
        {
            if (grandchild->GetName() != "value") throw "value tag not found";
            input.pcval = grandchild->GetNodeContent();

            input.type = TEST_PC;
        }
        else if (child->GetName() == "test-pointer")
        {
            if (grandchild->GetName() != "address") throw "address must come before value";
            input.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            input.pointer = grandchild->GetNodeContent();

            input.type = TEST_POINTER;
        }
        else if (child->GetName() == "test-string")
        {
            if (grandchild->GetName() != "address") throw "address must come before value";
            input.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            input.text = process_str(grandchild->GetNodeContent(), error);
            if (error) throw "malformed string in test-string";

            input.type = TEST_STRING;
        }
        else if (child->GetName() == "test-array")
        {
            if (grandchild->GetName() != "address") throw "address must come before value";
            input.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            std::string array = grandchild->GetNodeContent().ToStdString();
            tokenize(array, input.array, ",");

            input.type = TEST_ARRAY;
        }
        else if (child->GetName() == "test-stdin")
        {
            if (grandchild->GetName() != "value") throw "value tag not found";
            input.io = process_str(grandchild->GetNodeContent(), error);
            if (error) throw "malformed string in test-stdin";

            input.type = TEST_IO;
        }
        else if (child->GetName() == "test-subr")
        {
            lc3_subr_input& subr = input.subroutine;
            if (grandchild->GetName() != "name") throw "name must come before stack";
            subr.name = grandchild->GetNodeContent();

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "stack") throw "stack must come before r7";
            subr.stack = grandchild->GetNodeContent();

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "r7") throw "r7 must come before r5";
            subr.r7 = grandchild->GetNodeContent();

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "r5") throw "r5 must come before params";
            subr.r5 = grandchild->GetNodeContent();

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "params") throw "params tag not found";
            std::string params = grandchild->GetNodeContent().ToStdString();
            tokenize(params, subr.params, ",");

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
            if (grandchild->GetName() != "address") throw "address must come before value";
            output.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            output.value = grandchild->GetNodeContent();

            output.type = TEST_VALUE;
        }
        else if (child->GetName() == "test-register")
        {
            if (grandchild->GetName() != "register") throw "register must come before value";
            output.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            output.registerval = grandchild->GetNodeContent();

            output.type = TEST_REGISTER;
        }
        else if (child->GetName() == "test-pc")
        {
            if (grandchild->GetName() != "value") throw "value tag not found";
            output.pcval = grandchild->GetNodeContent();

            output.type = TEST_PC;
        }
        else if (child->GetName() == "test-pointer")
        {
            if (grandchild->GetName() != "address") throw "address must come before value";
            output.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            output.pointer = grandchild->GetNodeContent();

            output.type = TEST_POINTER;
        }
        else if (child->GetName() == "test-string")
        {
            if (grandchild->GetName() != "address") throw "address must come before value";
            output.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            output.text = process_str(grandchild->GetNodeContent(), error);
            if (error) throw "malformed string in test-string";

            output.type = TEST_STRING;
        }
        else if (child->GetName() == "test-array")
        {
            if (grandchild->GetName() != "address") throw "address must come before value";
            output.address = grandchild->GetNodeContent();
            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "value") throw "value tag not found";
            std::string array = grandchild->GetNodeContent().ToStdString();
            tokenize(array, output.array, ",");

            output.type = TEST_ARRAY;
        }
        else if (child->GetName() == "test-stdout")
        {
            if (grandchild->GetName() != "value") throw "value tag not found";
            output.io = process_str(grandchild->GetNodeContent(), error);
            if (error) throw "malformed string in test-stdout";

            output.type = TEST_IO;
        }
        else if (child->GetName() == "test-subr")
        {
            lc3_subr_output& subr = output.subroutine;
            if (grandchild->GetName() != "answer") throw "answer must come before locals";
            subr.answer = grandchild->GetNodeContent();

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "locals") throw "locals must come before points";
            std::string locals = grandchild->GetNodeContent().ToStdString();
            tokenize(locals, subr.locals, ",");

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "points")
                throw "points must come before deductions-per-mistake";

            {
                wxXmlNode* ggchild = grandchild->GetChildren();
                if (ggchild->GetName() != "answer") throw "answer in points tag must come before params";
                subr.points_answer = wxAtoi(ggchild->GetNodeContent());

                ggchild = getNextNode(ggchild);
                if (ggchild->GetName() != "params") throw "params in points tag must come before r7";
                subr.points_params = wxAtoi(ggchild->GetNodeContent());

                ggchild = getNextNode(ggchild);
                if (ggchild->GetName() != "r7") throw "r7 in points tag must come before r5";
                subr.points_r7 = wxAtoi(ggchild->GetNodeContent());

                ggchild = getNextNode(ggchild);
                if (ggchild->GetName() != "r5") throw "r5 in points tag must come before locals";
                subr.points_r5 = wxAtoi(ggchild->GetNodeContent());

                ggchild = getNextNode(ggchild);
                if (ggchild->GetName() != "locals") throw "locals in points tag not found";
                subr.points_locals = wxAtoi(ggchild->GetNodeContent());
            }

            grandchild = getNextNode(grandchild);
            if (grandchild->GetName() != "deductions-per-mistake") throw "deductions-per-mistake tag not found";
            subr.deductions_edist = wxAtoi(grandchild->GetNodeContent());

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

            if (!subr_input) throw "Used output type of test-subr without specifying subroutine input.";

           subr.params = subr_input->subroutine.params;
           subr.r7 = subr_input->subroutine.r7;
           subr.r5 = subr_input->subroutine.r5;
           subr.stack = subr_input->subroutine.stack;

           // Post processing part II calculate total points
           output.points = subr.points_answer + subr.points_locals + subr.points_params + subr.points_r5 + subr.points_r7;
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

        output.cmp_type = GetCompareType(output.type, mode);

        test.output.push_back(output);

        child = getNextNode(child);
    }

    return true;
}

int XmlTestParser::GetCompareType(int type, const wxString& mode)
{
    // -1, all 0 integral, 1 string, 2 array
    int detected_type = 0;
    int real_type = 0;
    // see lc3_cmp enum
    int comp_type = -1;

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
    {
        std::stringstream oss;
        oss << "Unknown comparison condition ";
        oss << mode;
        throw oss.str();
    }

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
    }

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
        throw "Used a condition that is not applicable for the types given";

    return comp_type;
}
