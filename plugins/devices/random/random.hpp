#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <lc3_all.hpp>
#include <random>

#define RANDOM_MAJOR_VERSION 1
#define RANDOM_MINOR_VERSION 5

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

extern "C" Plugin* create_plugin(const std::map<std::string, std::string>& params);
extern "C" void destroy_plugin(Plugin* ptr);

#endif

