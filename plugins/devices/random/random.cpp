#include <cstdlib>
#include <ctime>
#include "random.hpp"

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

    int initial_seed = 0;
    bool random_seed;
    lc3_params_read_int(params, "seed", initial_seed);

    if (lc3_params_read_bool(params, "randomseed", random_seed))
        initial_seed = time(NULL);

    instance = new RandomPlugin(address, initial_seed);

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

/** @brief OnRead
  *
  * @todo: document this function
  */
short RandomPlugin::OnRead(lc3_state& state)
{
    ///TODO rand_r is not defined on windows find alternative.
    short retVal = (short) rand_r(&seed);
    state.mem[GetAddress()] = retVal;
    return retVal;
}

/** @brief OnWrite
  *
  * @todo: document this function
  */
void RandomPlugin::OnWrite(lc3_state& state, short value)
{
    seed = (unsigned int) value;
}
