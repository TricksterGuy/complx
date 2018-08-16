//
// Created by Cem Gokmen on 8/12/18.
//

#ifndef COMPLX_TOOLS_RESULTEMITTER_H
#define COMPLX_TOOLS_RESULTEMITTER_H

#include <sstream>
#include <lc3_all.hpp>

class ResultEmitter {
public:
    virtual ~ResultEmitter() = default;
    virtual void add_suite(lc3_test_suite& suite, const std::string& filename) = 0;
    virtual void emit_results(std::ostream& os) = 0;
};


#endif //COMPLX_TOOLS_RESULTEMITTER_H
