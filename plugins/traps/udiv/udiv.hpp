#ifndef UDIV_HPP
#define UDIV_HPP

#include <lc3_plugin.hpp>
#include <lc3_parser.hpp>

#define UDIV_MAJOR_VERSION 1
#define UDIV_MINOR_VERSION 3

class UdivPlugin : public TrapFunctionPlugin
{
    public:
        UdivPlugin(unsigned char vector);
        ~UdivPlugin();
        std::string GetTrapName() const;
        void OnExecute(lc3_state& state, lc3_state_change& changes);
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

