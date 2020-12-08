#include "random.hpp"

#include <cstdlib>
#include <ctime>
#include <memory>

static std::unique_ptr<Plugin> instance;

Plugin* create_plugin(const PluginParams& params)
{
    if (instance)
        return instance.get();

    uint16_t address = params.read_uint16_t_required("address");

    unsigned int seed = 0;
    params.read_uint("seed", seed);

    bool random_seed = false;
    params.read_bool("randomseed", random_seed);

    if (random_seed)
        seed = time(nullptr);

    instance = std::make_unique<RandomPlugin>(address, seed);

    return instance.get();
}

void destroy_plugin(Plugin* ptr)
{
    if (instance.get() == ptr)
        instance.reset();
}

RandomPlugin::RandomPlugin(uint16_t address, unsigned int seed) :
    Plugin(RANDOM_MAJOR_VERSION, RANDOM_MINOR_VERSION, LC3_OTHER, "Random Generator plugin"), generator(seed),
    distribution(-32768, 32767)
{
    BindAddress(address);
}

int16_t RandomPlugin::OnRead(lc3_state& state, uint16_t addr)
{
    auto retVal = static_cast<int16_t>(distribution(generator));
    state.mem[addr] = retVal;
    return retVal;
}

void RandomPlugin::OnWrite(lc3_state& /*state*/, uint16_t /*addr*/, int16_t value)
{
    generator.seed(value);
}
