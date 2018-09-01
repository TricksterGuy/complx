#include "PyLC3.hpp"

bool LC3State::load(const std::string& filename, bool testing_mode, bool disable_plugins, bool process_debug_comments, bool multiple_errors, bool enable_warnings, bool warnings_as_errors)
{
    try
    {
        std::vector<code_range> ranges;
        LC3AssembleOptions options;
        if (testing_mode) 
        {
            state.input = &in;
            state.output = &out;
            state.max_stack_size = 0;
            state.max_call_stack_size = -1;
            state.in_lc3test = true;
        }

        options.multiple_errors = multiple_errors;
        options.warnings_as_errors = warnings_as_errors;
        options.process_debug_comments = process_debug_comments;
        options.enable_warnings = enable_warnings;
        options.disable_plugins = disable_plugins;

        lc3_assemble(state, filename, ranges, options);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "%s", e.what().c_str());
        return false;
    }
    return true;
}

bool LC3State::loadCode(const std::string& code)
{
    try
    {
        state.input = &in;
        state.output = &out;
        std::vector<code_range> ranges;
        LC3AssembleOptions options;
        state.max_stack_size = 0;
        state.max_call_stack_size = -1;
        state.in_lc3test = true;
        options.multiple_errors = false;
        options.warnings_as_errors = false;
        options.process_debug_comments = false;
        options.enable_warnings = false;
        options.disable_plugins = false;
        std::stringstream ss(code);
        lc3_assemble(state, ss, ranges, options);
    }
    catch (LC3AssembleException e)
    {
        fprintf(stderr, "%s", e.what().c_str());
        return false;
    }
    return true;
}
