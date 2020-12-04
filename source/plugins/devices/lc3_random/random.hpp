#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <lc3.hpp>
#include <lc3_random/lc3_random_api.h>

#include <random>

#define RANDOM_MAJOR_VERSION 1
#define RANDOM_MINOR_VERSION 6

class RandomPlugin : public Plugin
{
public:
    RandomPlugin(unsigned short address, unsigned int seed);
    short OnRead(lc3_state& state, unsigned short addr) override;
    void OnWrite(lc3_state& state, unsigned short addr, short value) override;
private:
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
};

extern "C"
{
    LC3_RANDOM_API Plugin* create_plugin(const PluginParams& params);
    LC3_RANDOM_API void destroy_plugin(Plugin* ptr);
}

#endif

