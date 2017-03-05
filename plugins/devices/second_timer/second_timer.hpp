#ifndef SECOND_TIMER_HPP
#define SECOND_TIMER_HPP

#include <lc3_all.hpp>

#define SECOND_TIMER_MAJOR_VERSION 1
#define SECOND_TIMER_MINOR_VERSION 4

///TODO complete this plugin
class SecondTimerPlugin : public DeviceRegisterPlugin
{
public:
    SecondTimerPlugin(unsigned short vector);
    virtual short OnRead(lc3_state& state);
    virtual void OnWrite(lc3_state& state, short value);
    unsigned int lastreadtime;
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

