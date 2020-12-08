#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <lc3.hpp>
#include <lc3_random/lc3_random_api.h>

#include <random>

#define RANDOM_MAJOR_VERSION 1
#define RANDOM_MINOR_VERSION 6

class LC3_RANDOM_API RandomPlugin : public Plugin
{
public:
    RandomPlugin(uint16_t address, unsigned int seed);
    int16_t OnRead(lc3_state& state, uint16_t addr) override;
    void OnWrite(lc3_state& state, uint16_t addr, int16_t value) override;
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

