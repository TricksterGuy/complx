#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <lc3_all.hpp>
#include <random>

#define RANDOM_MAJOR_VERSION 1
#define RANDOM_MINOR_VERSION 3

class RandomPlugin : public DeviceRegisterPlugin
{
public:
    RandomPlugin(unsigned short address, unsigned int seed) :
        DeviceRegisterPlugin(RANDOM_MAJOR_VERSION, RANDOM_MINOR_VERSION, "Random Generator plugin", address), generator(seed),
        distribution(-32768, 32767) {}
    virtual short OnRead(lc3_state& state);
    virtual void OnWrite(lc3_state& state, short value);
private:
    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution;
};

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

