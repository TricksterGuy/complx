#ifndef PINGER_HPP
#define PINGER_HPP

#include <lc3_all.hpp>
#define PINGER_MAJOR_VERSION 1
#define PINGER_MINOR_VERSION 3

class PingerPlugin : public Plugin
{
public:
    PingerPlugin(unsigned short ping_interval, unsigned int prio, unsigned short vec) :
        Plugin(PINGER_MAJOR_VERSION, PINGER_MINOR_VERSION, LC3_OTHER, "Pinger plugin", true, vec), interval(ping_interval), priority(prio), ticks(0) {}
    virtual void OnTick(lc3_state& state);
private:
    unsigned int interval;
    unsigned int priority;
    unsigned int ticks;
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

