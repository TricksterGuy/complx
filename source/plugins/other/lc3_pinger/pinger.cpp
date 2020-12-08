#include "pinger.hpp"

#include <cstdlib>
#include <ctime>
#include <memory>

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    unsigned int interval = params.read_uint_required("interval");
    /// TODO add ability to check ranges of read in params.
    unsigned int priority = params.read_uint_required("priority");
    unsigned char vec = params.read_uchar_required("vector");

    instance = std::make_unique<PingerPlugin>(interval, priority, vec);

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

PingerPlugin::PingerPlugin(uint16_t ping_interval, unsigned int prio, unsigned char vec) :
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
