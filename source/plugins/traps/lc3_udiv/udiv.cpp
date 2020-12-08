#include "udiv.hpp"

#include <memory>
#include <sstream>

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    unsigned char vector = params.read_uchar_required("vector");

    instance = std::make_unique<UdivPlugin>(vector);

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

void UdivPlugin::OnExecute(lc3_state& state, lc3_state_change& changes)
{
    changes.changes = LC3_MULTI_CHANGE;
    changes.info.push_back(lc3_change_info({true, 0, static_cast<uint16_t>(state.regs[0])}));
    changes.info.push_back(lc3_change_info({true, 1, static_cast<uint16_t>(state.regs[1])}));

    int16_t r0 = state.regs[0];
    int16_t r1 = state.regs[1];

    if (r1 != 0)
    {
        state.regs[0] = r0 / r1;
        state.regs[1] = r0 % r1;
    }
    else
    {
        state.regs[0] = -1;
        state.regs[1] = -1;
    }
}
