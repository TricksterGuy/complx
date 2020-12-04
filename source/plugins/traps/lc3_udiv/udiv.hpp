#ifndef UDIV_HPP
#define UDIV_HPP

#include <lc3.hpp>
#include <lc3_udiv/lc3_udiv_api.h>

#define UDIV_MAJOR_VERSION 1
#define UDIV_MINOR_VERSION 6

class UdivPlugin : public TrapFunctionPlugin
{
public:
    UdivPlugin(unsigned char vector) : TrapFunctionPlugin(UDIV_MAJOR_VERSION, UDIV_MINOR_VERSION, "Division and Modulus Trap", vector) {}
    std::string GetTrapName() const override {return "UDIV";}
    void OnExecute(lc3_state& state, lc3_state_change& changes) override;
};

extern "C"
{
    LC3_UDIV_API Plugin* create_plugin(const PluginParams& params);
    LC3_UDIV_API void destroy_plugin(Plugin* ptr);
}

#endif

