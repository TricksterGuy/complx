#ifndef TIMER_HPP
#define TIMER_HPP

#include <lc3_plugin.hpp>
#include <lc3_parser.hpp>
#include <ctime>

#define TIMER_MAJOR_VERSION 1
#define TIMER_MINOR_VERSION 3

class TimerPlugin : public DeviceRegisterPlugin
{
    public:
        TimerPlugin(unsigned short address, unsigned char vector);
        ~TimerPlugin();
        virtual short OnRead(lc3_state& state);
        virtual void OnWrite(lc3_state& state, short value);
        virtual void OnTick(lc3_state& state);
        time_t lastreadtime;
        time_t time;
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

