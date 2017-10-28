#include <cstdlib>
#include <ctime>
#include "pinger.hpp"

static Plugin* instance = NULL;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance != NULL)
        return instance;

    unsigned int interval;
    if (lc3_params_read_uint(params, "interval", interval) == false)
    {
        fprintf(stderr, "Interval param (interval) not given or in incorrect format: %s\n", lc3_params_get_value(params, "interval").c_str());
        return NULL;
    }

    unsigned int priority = 4;
    if (lc3_params_read_uint(params, "priority", priority) == false)
    {
        fprintf(stderr, "Priority param (priority) not given or in incorrect format: %s\n", lc3_params_get_value(params, "priority").c_str());
        return NULL;
    }

    unsigned char vec;
    if (lc3_params_read_uchar(params, "vector", vec) == false)
    {
        fprintf(stderr, "Vector param (vector) not given or in incorrect format: %s\n", lc3_params_get_value(params, "vector").c_str());
        return NULL;
    }

    instance = new PingerPlugin(interval, priority, vec);

    return instance;
}

void destroy_plugin(Plugin* ptr = NULL)
{
    if (ptr == instance)
    {
        delete instance;
        instance = NULL;
    }
}

PingerPlugin::PingerPlugin(unsigned short ping_interval, unsigned int prio, unsigned char vec) :
        Plugin(PINGER_MAJOR_VERSION, PINGER_MINOR_VERSION, LC3_OTHER, "Pinger plugin"), interval(ping_interval), priority(prio), ticks(0), int_vector(vec)
{
    BindInterrupt(vec);
}


void PingerPlugin::OnTick(lc3_state& state)
{
    ticks++;
    if (ticks > interval)
    {
        ticks = 0;
        lc3_signal_interrupt_once(state, priority, int_vector);
    }
}
