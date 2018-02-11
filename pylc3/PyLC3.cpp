#include "PyLC3.hpp"

bool LC3State::load(const std::string& filename)
{
    try
    {
        std::vector<code_range> ranges;
        LC3AssembleOptions options;
        options.multiple_errors = true;
        options.warnings_as_errors = false;
        options.process_debug_comments = true;
        options.enable_warnings = false;
        options.disable_plugins = false;
        lc3_assemble(state, filename, ranges, options);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "%s", e.what().c_str());
        return false;
    }
    return true;
}

LC3Test LC3TestSuite::get_test(unsigned int id) const
{
    if (id >= suite.tests.size())
    {
        return LC3Test();
    }
    return LC3Test(suite.tests[id]);
}
