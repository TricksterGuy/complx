#ifndef XML_TEST_PARSER_HPP
#define XML_TEST_PARSER_HPP

#include <string>
#include "lc3_test.hpp"
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/wx.h>

struct XmlTestParserException
{
    XmlTestParserException(const wxString& msg, const wxXmlNode* child) : message(msg), node(child) {}
    std::string what()
    {
        if (node->GetLineNumber() != -1)
            return wxString::Format("Found at line: %d %s", node->GetLineNumber(), message).ToStdString();
        else
            return message.ToStdString();
    }
    wxString message;
    const wxXmlNode* node;
};

class XmlTestParser
{
public:
    static XmlTestParser& Instance()
    {
        static XmlTestParser singleton;
        return singleton;
    }
    bool LoadTestSuite(lc3_test_suite& suite, const std::string& filename);
    bool LoadTestSuiteData(lc3_test_suite& suite, const std::string& data);
private:
    XmlTestParser() {};                                     // Private constructor
    XmlTestParser(const XmlTestParser&);                    // Prevent copy-construction
    XmlTestParser& operator=(const XmlTestParser&);         // Prevent assignment
    bool LoadTestSuite(lc3_test_suite& suite, wxInputStream& stream);
    bool LoadTest(lc3_test_suite& suite, wxXmlNode* root);
    bool LoadTestInput(lc3_test& suite, wxXmlNode* root);
    bool LoadTestOutput(lc3_test& suite, wxXmlNode* root);
};

inline XmlTestParser& XmlTestParser()
{
    return XmlTestParser::Instance();
}
#endif
