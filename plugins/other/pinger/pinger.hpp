#ifndef PINGER_HPP
#define PINGER_HPP

#include <lc3_all.hpp>
#define PINGER_MAJOR_VERSION 1
#define PINGER_MINOR_VERSION 5

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

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

