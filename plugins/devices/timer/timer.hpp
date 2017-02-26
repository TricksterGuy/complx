#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>
#include <lc3_all.hpp>

#define TIMER_MAJOR_VERSION 1
#define TIMER_MINOR_VERSION 3

///TODO complete this plugin
class TimerPlugin : public DeviceRegisterPlugin
{
public:
    TimerPlugin(unsigned short address, unsigned char vector) : DeviceRegisterPlugin(TIMER_MAJOR_VERSION, TIMER_MINOR_VERSION, "Timer plugin", address, true, vector) {}
    virtual short OnRead(lc3_state& state);
    virtual void OnWrite(lc3_state& state, short value);
    virtual void OnTick(lc3_state& state);
    time_t lastreadtime;
    time_t time;
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

