#include "random.hpp"

#include <cstdlib>
#include <ctime>
#include <memory>

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    unsigned short address = params.read_ushort_required("address");

    unsigned int seed = 0;
    params.read_uint("seed", seed);

    bool random_seed = false;
    params.read_bool("randomseed", random_seed);

    if (random_seed)
        seed = time(nullptr);

    instance.reset(new RandomPlugin(address, seed));

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

RandomPlugin::RandomPlugin(unsigned short address, unsigned int seed) :
    Plugin(RANDOM_MAJOR_VERSION, RANDOM_MINOR_VERSION, LC3_OTHER, "Random Generator plugin"), generator(seed),
    distribution(-32768, 32767)
{
    BindAddress(address);
}

short RandomPlugin::OnRead(lc3_state& state, unsigned short addr)
{
    short retVal = static_cast<short>(distribution(generator));
    state.mem[addr] = retVal;
    return retVal;
}

void RandomPlugin::OnWrite(lc3_state& /*state*/, unsigned short /*addr*/, short value)
{
    generator.seed(value);
}
