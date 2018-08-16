//
// Created by Cem Gokmen on 8/12/18.
//

#ifndef COMPLX_TOOLS_XMLRESULTEMITTER_H
#define COMPLX_TOOLS_XMLRESULTEMITTER_H

#include <lc3_all.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include "tinyxml2.h"
#include "ResultEmitter.h"

class XmlResultEmitter: public ResultEmitter {
    tinyxml2::XMLDocument* doc;
    tinyxml2::XMLElement* root;
    int runs;

    tinyxml2::XMLElement* add_test(lc3_test& test);

public:
    XmlResultEmitter();
    ~XmlResultEmitter();
    void add_suite(lc3_test_suite& suite, const std::string& filename);
    void emit_results(std::ostream& os);
};


#endif //COMPLX_TOOLS_XMLRESULTEMITTER_H
