//
// Created by Cem Gokmen on 8/12/18.
//

#ifndef COMPLX_TOOLS_RESULTEMITTER_H
#define COMPLX_TOOLS_RESULTEMITTER_H

#include <sstream>
#include <lc3_all.hpp>

class ResultEmitter {
    public:
        virtual void lc3test_output_test_report(std::stringstream& oss, lc3_test_suite& suite, const std::string& filename);
};


#endif //COMPLX_TOOLS_RESULTEMITTER_H
