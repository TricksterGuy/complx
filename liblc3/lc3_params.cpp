#include <cstdlib>
#include <climits>
#include <algorithm>
#include <cerrno>
#include "lc3_params.hpp"

bool lc3_params_has_key(const PluginParams& params, const std::string& key)
{
    return params.find(key) != params.end();
}

std::string lc3_params_get_value(const PluginParams& params, const std::string& key)
{
    std::map<std::string, std::string>::const_iterator x = params.find(key);
    if (x == params.end())
        return "";

    return x->second;
}

bool lc3_params_read_bool(const PluginParams& params, const std::string& key, bool& var)
{
    std::map<std::string, std::string>::const_iterator x = params.find(key);
    if (x == params.end())
        return false;

    std::string val = x->second;
    std::transform(val.begin(), val.end(), val.begin(), (int (*)(int))std::tolower);

    if (val == std::string("true") || val == std::string("yes") || val == std::string("1"))
        var = true;
    else if (val == std::string("false") || val == std::string("no") ||val == std::string("0"))
        var = false;
    else
        return false;

    return true;
}

bool lc3_params_read_char(const PluginParams& params, const std::string& key, char& var)
{
    long dummy;
    if (lc3_params_read_long(params, key, dummy) == false)
        return false;

    if (dummy > CHAR_MAX || dummy < CHAR_MIN)
        return false;

    var = (char) dummy;
    return true;
}

bool lc3_params_read_short(const PluginParams& params, const std::string& key, short& var)
{
    long dummy;
    if (lc3_params_read_long(params, key, dummy) == false)
        return false;

    if (dummy > SHRT_MAX || dummy < SHRT_MIN)
        return false;

    var = (short) dummy;
    return true;
}

bool lc3_params_read_int(const PluginParams& params, const std::string& key, int& var)
{
    long dummy;
    if (lc3_params_read_long(params, key, dummy) == false)
        return false;

    if (dummy > INT_MAX || dummy < INT_MIN)
        return false;

    var = (int) dummy;
    return true;
}

bool lc3_params_read_long(const PluginParams& params, const std::string& key, long& var)
{
    std::map<std::string, std::string>::const_iterator x = params.find(key);
    if (x == params.end())
        return false;

    std::string val = x->second;

    if (val[0] == 'x')
        val = '0' + val;

    errno = 0;
    var = strtol(val.c_str(), NULL, 0);

    return (errno == 0);
}

bool lc3_params_read_uchar(const PluginParams& params, const std::string& key, unsigned char& var)
{
    unsigned long dummy;
    if (lc3_params_read_ulong(params, key, dummy) == false)
        return false;

    if (dummy > UCHAR_MAX)
        return false;

    var = (unsigned char) dummy;
    return true;
}

bool lc3_params_read_ushort(const PluginParams& params, const std::string& key, unsigned short& var)
{
    unsigned long dummy;
    if (lc3_params_read_ulong(params, key, dummy) == false)
        return false;

    if (dummy > USHRT_MAX)
        return false;

    var = (unsigned short) dummy;
    return true;
}

bool lc3_params_read_uint(const PluginParams& params, const std::string& key, unsigned int& var)
{
    unsigned long dummy;
    if (lc3_params_read_ulong(params, key, dummy) == false)
        return false;

    if (dummy > UINT_MAX)
        return false;

    var = (unsigned int) dummy;
    return true;
}

bool lc3_params_read_ulong(const PluginParams& params, const std::string& key, unsigned long& var)
{
    std::map<std::string, std::string>::const_iterator x = params.find(key);
    if (x == params.end())
        return false;

    std::string val = x->second;

    if (val[0] == 'x')
        val = '0' + val;

    errno = 0;
    var = strtoul(val.c_str(), NULL, 0);

    return (errno == 0);
}

bool lc3_params_read_string(const PluginParams& params, const std::string& key, std::string& var)
{
    std::map<std::string, std::string>::const_iterator x = params.find(key);
    if (x == params.end())
        return false;

    var = x->second;

    return true;
}
