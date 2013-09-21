#ifndef XML_TEST_PARSER_HPP
#define XML_TEST_PARSER_HPP

#include <string>
#include "lc3_test.hpp"
#include <wx/tokenzr.h>
#include <wx/xml/xml.h>
#include <wx/wx.h>

class XmlTestParser
{
	public:
        static XmlTestParser& Instance()
        {
            static XmlTestParser singleton;
            return singleton;
        }
		~XmlTestParser();
		bool LoadTestSuite(lc3_test_suite& suite, const std::string filename);
	private:
        XmlTestParser() {};                                     // Private constructor
        XmlTestParser(const XmlTestParser&);                    // Prevent copy-construction
        XmlTestParser& operator=(const XmlTestParser&);         // Prevent assignment
        bool LoadTest(lc3_test_suite& suite, wxXmlNode* root);
        bool LoadTestInput(lc3_test& suite, wxXmlNode* root);
        bool LoadTestOutput(lc3_test& suite, wxXmlNode* root);
        int GetCompareType(int type, const wxString& mode);

};

inline XmlTestParser& XmlTestParser()
{
    return XmlTestParser::Instance();
}

#endif
