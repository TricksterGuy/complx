#ifndef UDIV_HPP
#define UDIV_HPP

#include <lc3.hpp>

#define UDIV_MAJOR_VERSION 1
#define UDIV_MINOR_VERSION 6

class LC3_API UdivPlugin : public TrapFunctionPlugin
{
public:
    UdivPlugin(unsigned char vector) : TrapFunctionPlugin(UDIV_MAJOR_VERSION, UDIV_MINOR_VERSION, "Division and Modulus Trap", vector) {}
    std::string GetTrapName() const override {return "UDIV";}
    void OnExecute(lc3_state& state, lc3_state_change& changes) override;
};

extern "C" LC3_API Plugin* create_plugin(const PluginParams& params);
extern "C" LC3_API void destroy_plugin(Plugin* ptr);

#endif

