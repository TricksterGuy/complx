#ifndef TIMER_HPP
#define TIMER_HPP

#include <ctime>

#include <lc3.hpp>
#include <lc3_timer/lc3_timer_api.h>

#define TIMER_MAJOR_VERSION 1
#define TIMER_MINOR_VERSION 6

///TODO complete this plugin
class LC3_TIMER_API TimerPlugin : public Plugin
{
public:
    TimerPlugin(unsigned short address, unsigned char vector);
    short OnRead(lc3_state& state, unsigned short address) override;
    void OnWrite(lc3_state& state, unsigned short address, short value) override;
    void OnTick(lc3_state& state) override;
    time_t lastreadtime;
    time_t time;
};

extern "C"
{
    LC3_TIMER_API Plugin* create_plugin(const PluginParams& params);
    LC3_TIMER_API void destroy_plugin(Plugin* ptr);
}

#endif

