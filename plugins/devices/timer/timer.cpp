#include <cstdlib>
#include <ctime>
#include "timer.hpp"

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

    unsigned char vector = 0;
    lc3_params_read_uchar(params, "vector", vector);

    //instance = new TimerPlugin(address, vector != 0, vector);
    fprintf(stderr, "Warning this plugin needs to be implemented see Brandon");
    return NULL;
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
short TimerPlugin::OnRead(lc3_state& state)
{
    return (short) time;
}

/** @brief OnWrite
  *
  * @todo: document this function
  */
void TimerPlugin::OnWrite(lc3_state& state, short value)
{
}

/** @brief OnTick
  *
  * @todo: document this function
  */
void TimerPlugin::OnTick(lc3_state& state)
{
    time_t now = ::time(NULL);
    if (now != lastreadtime)
    {
        lastreadtime = now;
        //struct tm* newtime = localtime(&now);
        //int hour = newtime->tm_hour;
        //int minute = newtime->tm_min;
        // TODO FINISH THIS
    }
}
