#ifndef SECOND_TIMER_HPP
#define SECOND_TIMER_HPP

#include <lc3.hpp>
#include <lc3_second_timer/lc3_second_timer_api.h>

#define SECOND_TIMER_MAJOR_VERSION 1
#define SECOND_TIMER_MINOR_VERSION 6

///TODO complete this plugin
class SecondTimerPlugin : public Plugin
{
public:
    SecondTimerPlugin(unsigned short vector);
    short OnRead(lc3_state& state) override;
    void OnWrite(lc3_state& state, short value) override;
    unsigned int lastreadtime;
};

extern "C"
{
    LC3_SECOND_TIMER_API Plugin* create_plugin(const PluginParams& params);
    LC3_SECOND_TIMER_API void destroy_plugin(Plugin* ptr);
}

#endif

