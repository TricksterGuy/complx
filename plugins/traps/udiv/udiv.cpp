#include "udiv.hpp"

static Plugin* instance = NULL;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance != NULL)
        return instance;

    unsigned char vector;
    if (lc3_params_read_uchar(params, "vector", vector) == false)
    {
        fprintf(stderr, "Vector param (vector) not given or in incorrect format: %s\n", lc3_params_get_value(params, "vector").c_str());
        return NULL;
    }

    instance = new UdivPlugin(vector);

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

/** @brief OnExecute
  *
  * @todo: document this function
  */
void UdivPlugin::OnExecute(lc3_state& state, lc3_state_change& changes)
{
    changes.changes = LC3_MULTI_CHANGE;
    changes.info.push_back((lc3_change_info)
    {
        true, 0, (unsigned short)state.regs[0]
    });
    changes.info.push_back((lc3_change_info)
    {
        true, 1, (unsigned short)state.regs[1]
    });

    short r0 = state.regs[0];
    short r1 = state.regs[1];

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
