#include "lc3/lc3_params.hpp"

#include <algorithm>
#include <cerrno>
#include <climits>
#include <cstdlib>
#include <sstream>

namespace
{

std::string form_error_string(const std::string& type, const std::string& key, const std::string& value)
{
    std::stringstream stream("Unable to read ");
    stream << type << " param: ";
    stream << key << " value: \"" << value << "\"";
    return stream.str();
}

std::string form_not_found_string(const std::string& key)
{
    std::stringstream stream("Could not read param: ");
    stream << key;
    return stream.str();
}

bool parse_integral(const std::string& value, long& var, long min, long max)
{
    char* end;

    if (value[0] == 'x')
        var = strtol(value.c_str() + 1, &end, 16);
    else
        var = strtol(value.c_str(), &end, 0);

    return *end == 0 && var >= min && var <= max;
}

bool parse_integral(const std::string& value, unsigned long& var, unsigned long max)
{
    char* end;

    if (value[0] == 'x')
        var = strtoul(value.c_str() + 1, &end, 16);
    else
        var = strtoul(value.c_str(), &end, 0);

    return *end == 0 && var <= max;
}

bool parse_boolean(const std::string& value, bool& var)
{
    std::string val = value;
    std::transform(val.begin(), val.end(), val.begin(), ::tolower);

    if (val == "true" || val == "yes" || val == "1" || val == "on")
        var = true;
    else if (val == "false" || val == "no" || val == "0" || val == "off")
        var = false;
    else
        return false;

    return true;
}

}


std::string LC3PluginException::what() const throw()
{
    std::stringstream stream;
    stream << "Plugin " << name << " failed to load!\n";
    stream << "-----";
    stream << "Path: " << path << "\n";
    stream << "Reason: " << message;
    return stream.str();
}


PluginParams::PluginParams(const std::string& plugin_name, const std::string& plugin_path, const std::map<std::string, std::string>& plugin_params) :
    name(plugin_name), full_path(plugin_path), params(plugin_params)
{

}

bool PluginParams::has_key(const std::string& key) const
{
    return params.find(key) != params.end();
}

std::string PluginParams::get_value(const std::string& key) const
{
    auto x = params.find(key);

    if (x == params.end())
        return "";

    return x->second;
}

bool PluginParams::read_bool(const std::string& key, bool& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    if (!parse_boolean(value, var))
        throw LC3PluginException(name, full_path, form_error_string("boolean", key, value));

    return true;
}

bool PluginParams::read_char(const std::string& key, char& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    long dummy;
    if (!parse_integral(value, dummy, CHAR_MIN, CHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("char", key, value));

    var = static_cast<char>(dummy);

    return true;
}

bool PluginParams::read_short(const std::string& key, short& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    long dummy;
    if (!parse_integral(value, dummy, SHRT_MIN, SHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("short", key, value));

    var = static_cast<short>(dummy);

    return true;
}

bool PluginParams::read_int(const std::string& key, int& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    long dummy;
    if (!parse_integral(value, dummy, INT_MIN, INT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int", key, value));

    var = static_cast<int>(dummy);

    return true;
}

bool PluginParams::read_long(const std::string& key, long& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    if (!parse_integral(value, var, LONG_MIN, LONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("long", key, value));

    return true;
}

bool PluginParams::read_uchar(const std::string& key, unsigned char& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    unsigned long dummy;
    if (!parse_integral(value, dummy, UCHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned char", key, value));

    var = static_cast<unsigned char>(dummy);

    return true;
}

bool PluginParams::read_ushort(const std::string& key, unsigned short& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    unsigned long dummy;
    if (!parse_integral(value, dummy, USHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned short", key, value));

    var = static_cast<unsigned short>(dummy);

    return true;
}

bool PluginParams::read_uint(const std::string& key, unsigned int& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    unsigned long dummy;
    if (!parse_integral(value, dummy, UINT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned int", key, value));

    var = static_cast<unsigned int>(dummy);

    return true;
}

bool PluginParams::read_ulong(const std::string& key, unsigned long& var) const
{
    std::string value;

    if (read_string(key, value) == false)
        return false;

    if (!parse_integral(value, var, ULONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned long", key, value));

    return true;
}

bool PluginParams::read_string(const std::string& key, std::string& var) const
{
    if (!has_key(key))
        return false;

    var = params.at(key);

    return true;
}

bool PluginParams::read_bool_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    bool var;

    if (!parse_boolean(value, var))
        throw LC3PluginException(name, full_path, form_error_string("boolean", key, value));

    return var;
}

char PluginParams::read_char_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    long var = 0;

    if (!parse_integral(value, var, CHAR_MIN, CHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("char", key, value));

    return static_cast<char>(var);
}

short PluginParams::read_short_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    long var = 0;

    if (!parse_integral(value, var, SHRT_MIN, SHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("short", key, value));

    return static_cast<short>(var);
}

int PluginParams::read_int_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    long var = 0;

    if (!parse_integral(value, var, INT_MIN, INT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int", key, value));

    return static_cast<int>(var);
}

long PluginParams::read_long_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    long var = 0;

    if (!parse_integral(value, var, LONG_MIN, LONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("long", key, value));

    return var;
}

unsigned char PluginParams::read_uchar_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    unsigned long var = 0;

    if (!parse_integral(value, var, UCHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned char", key, value));

    return static_cast<unsigned char>(var);
}

unsigned short PluginParams::read_ushort_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    unsigned long var = 0;

    if (!parse_integral(value, var, USHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned short", key, value));

    return static_cast<unsigned short>(var);
}

unsigned int PluginParams::read_uint_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    unsigned long var = 0;

    if (!parse_integral(value, var, UINT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned int", key, value));

    return static_cast<unsigned int>(var);
}

unsigned long PluginParams::read_ulong_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    unsigned long var = 0;

    if (!parse_integral(value, var, ULONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned long", key, value));

    return var;
}

const std::string& PluginParams::read_string_required(const std::string& key) const
{
    if (!has_key(key))
        throw LC3PluginException(name, full_path, form_not_found_string(key));

    return params.at(key);
}
