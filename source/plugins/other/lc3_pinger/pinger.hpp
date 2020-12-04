#ifndef PINGER_HPP
#define PINGER_HPP

#include <lc3.hpp>
#include <lc3_pinger/lc3_pinger_api.h>

#define PINGER_MAJOR_VERSION 1
#define PINGER_MINOR_VERSION 6

class PingerPlugin : public Plugin
{
public:
    PingerPlugin(unsigned short ping_interval, unsigned int prio, unsigned char vec);
    void OnTick(lc3_state& state) override;
private:
    unsigned int interval;
    unsigned int priority;
    unsigned int ticks;
    unsigned char int_vector;
};

extern "C"
{
    LC3_PINGER_API Plugin* create_plugin(const PluginParams& params);
    LC3_PINGER_API void destroy_plugin(Plugin* ptr);
}

#endif

