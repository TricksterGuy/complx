#include <cstdlib>
#include <ctime>
#include "timer.hpp"

static Plugin* instance = NULL;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance != NULL)
        return instance;

    unsigned short address;
    if (lc3_params_read_ushort(params, "address", address) == false)
    {
        fprintf(stderr, "Address param (address) not given or in incorrect format: %s\n", lc3_params_get_value(params, "address").c_str());
        return NULL;
    }

    unsigned char vector = 0;
    lc3_params_read_uchar(params, "vector", vector);

    //instance = new TimerPlugin(address, vector != 0, vector);
    fprintf(stderr, "Warning this plugin needs to be implemented see Brandon");
    return NULL;
}

void destroy_plugin(Plugin* ptr = NULL)
{
    if (ptr == instance)
    {
        delete instance;
        instance = NULL;
    }
}

TimerPlugin::TimerPlugin(unsigned short address, unsigned char int_vector) : Plugin(TIMER_MAJOR_VERSION, TIMER_MINOR_VERSION, LC3_OTHER, "Timer plugin")
{
    BindInterrupt(int_vector);
}

short TimerPlugin::OnRead(lc3_state& state, unsigned short address)
{
    /// TODO implement.
    return Plugin::OnRead(state, address);
}

void TimerPlugin::OnWrite(lc3_state& state, unsigned short address, short value)
{
    /// TODO implement.
    Plugin::OnWrite(state, address, value);
}

void TimerPlugin::OnTick(lc3_state& state)
{
    time_t now = ::time(NULL);
    if (now != lastreadtime)
    {
        lastreadtime = now;
        //struct tm* newtime = localtime(&now);
        //int hour = newtime->tm_hour;
        //int minute = newtime->tm_min;
        // TODO FINISH THIS
    }
}
