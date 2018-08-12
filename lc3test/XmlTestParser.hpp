#ifndef XML_TEST_PARSER_HPP
#define XML_TEST_PARSER_HPP

#include <string>
#include "lc3_test.hpp"
#include "tinyxml2.h"

struct XmlTestParserException
{
    XmlTestParserException(const std::string& msg, const tinyxml2::XMLElement* child) : message(msg), node(child) {}
    std::string what()
    {
        std::ostringstream oss;
        if (node->GetLineNum() != -1)
            oss << "Found at line: " << node->GetLineNum() << " ";

        oss << message;
        return oss.str();
    }
    std::string message;
    const tinyxml2::XMLElement* node;
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
    bool LoadTestSuite(lc3_test_suite& suite, tinyxml2::XMLDocument* doc);
    bool LoadTest(lc3_test_suite& suite, tinyxml2::XMLElement* root);
    bool LoadTestInput(lc3_test& suite, tinyxml2::XMLElement* root);
    bool LoadTestOutput(lc3_test& suite, tinyxml2::XMLElement* root);
};

inline XmlTestParser& XmlTestParser()
{
    return XmlTestParser::Instance();
}
#endif
