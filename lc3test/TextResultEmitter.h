//
// Created by Cem Gokmen on 8/12/18.
//

#ifndef COMPLX_TOOLS_TEXTRESULTEMITTER_H
#define COMPLX_TOOLS_TEXTRESULTEMITTER_H

#include <lc3_all.hpp>
#include <iostream>
#include <sstream>
#include "ResultEmitter.h"

struct suite_summary
{
    suite_summary() : grade(0), tests(0), executions(0), warnings(0) {}
    int grade;
    int tests;
    int executions;
    int warnings;
};

class TextResultEmitter: public ResultEmitter {
    std::stringstream* oss;
    std::vector<suite_summary> results;
    int num_passed;
    int runs;
    lc3_test_suite last_suite;

public:
    TextResultEmitter();
    ~TextResultEmitter();
    void add_suite(lc3_test_suite& suite, const std::string& filename);
    void emit_results(std::ostream& os);
};


#endif //COMPLX_TOOLS_TEXTRESULTEMITTER_H
