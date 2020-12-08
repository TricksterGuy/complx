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

bool parse_integral(const std::string& value, int64_t& var, int64_t min, int64_t max)
{
    char* end;

    if (value[0] == 'x')
        var = strtol(value.c_str() + 1, &end, 16);
    else
        var = strtol(value.c_str(), &end, 0);

    return *end == 0 && var >= min && var <= max;
}

bool parse_integral(const std::string& value, uint64_t& var, uint64_t max)
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

LC3PluginException::LC3PluginException(const std::string& plugin, const std::string& plugin_path, const std::string& msg) : name(plugin), path(plugin_path), message(msg)
{
    error = form_error_message();
}

std::string LC3PluginException::form_error_message() const noexcept
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

    if (!read_string(key, value))
        return false;

    if (!parse_boolean(value, var))
        throw LC3PluginException(name, full_path, form_error_string("boolean", key, value));

    return true;
}

bool PluginParams::read_char(const std::string& key, char& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    int64_t dummy;
    if (!parse_integral(value, dummy, CHAR_MIN, CHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("char", key, value));

    var = static_cast<char>(dummy);

    return true;
}

bool PluginParams::read_int16_t(const std::string& key, int16_t& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    int64_t dummy;
    if (!parse_integral(value, dummy, SHRT_MIN, SHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int16_t", key, value));

    var = static_cast<int16_t>(dummy);

    return true;
}

bool PluginParams::read_int(const std::string& key, int& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    int64_t dummy;
    if (!parse_integral(value, dummy, INT_MIN, INT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int", key, value));

    var = static_cast<int>(dummy);

    return true;
}

bool PluginParams::read_int64_t(const std::string& key, int64_t& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    if (!parse_integral(value, var, LONG_MIN, LONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int64_t", key, value));

    return true;
}

bool PluginParams::read_uchar(const std::string& key, unsigned char& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    uint64_t dummy;
    if (!parse_integral(value, dummy, UCHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned char", key, value));

    var = static_cast<unsigned char>(dummy);

    return true;
}

bool PluginParams::read_uint16_t(const std::string& key, uint16_t& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    uint64_t dummy;
    if (!parse_integral(value, dummy, USHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("uint16_t", key, value));

    var = static_cast<uint16_t>(dummy);

    return true;
}

bool PluginParams::read_uint(const std::string& key, unsigned int& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    uint64_t dummy;
    if (!parse_integral(value, dummy, UINT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned int", key, value));

    var = static_cast<unsigned int>(dummy);

    return true;
}

bool PluginParams::read_uint64_t(const std::string& key, uint64_t& var) const
{
    std::string value;

    if (!read_string(key, value))
        return false;

    if (!parse_integral(value, var, ULONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("uint64_t", key, value));

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

    int64_t var = 0;

    if (!parse_integral(value, var, CHAR_MIN, CHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("char", key, value));

    return static_cast<char>(var);
}

int16_t PluginParams::read_int16_t_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    int64_t var = 0;

    if (!parse_integral(value, var, SHRT_MIN, SHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int16_t", key, value));

    return static_cast<int16_t>(var);
}

int PluginParams::read_int_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    int64_t var = 0;

    if (!parse_integral(value, var, INT_MIN, INT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int", key, value));

    return static_cast<int>(var);
}

int64_t PluginParams::read_int64_t_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    int64_t var = 0;

    if (!parse_integral(value, var, LONG_MIN, LONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("int64_t", key, value));

    return var;
}

unsigned char PluginParams::read_uchar_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    uint64_t var = 0;

    if (!parse_integral(value, var, UCHAR_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned char", key, value));

    return static_cast<unsigned char>(var);
}

uint16_t PluginParams::read_uint16_t_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    uint64_t var = 0;

    if (!parse_integral(value, var, USHRT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("uint16_t", key, value));

    return static_cast<uint16_t>(var);
}

unsigned int PluginParams::read_uint_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    uint64_t var = 0;

    if (!parse_integral(value, var, UINT_MAX))
        throw LC3PluginException(name, full_path, form_error_string("unsigned int", key, value));

    return static_cast<unsigned int>(var);
}

uint64_t PluginParams::read_uint64_t_required(const std::string& key) const
{
    const auto& value = read_string_required(key);

    uint64_t var = 0;

    if (!parse_integral(value, var, ULONG_MAX))
        throw LC3PluginException(name, full_path, form_error_string("uint64_t", key, value));

    return var;
}

const std::string& PluginParams::read_string_required(const std::string& key) const
{
    if (!has_key(key))
        throw LC3PluginException(name, full_path, form_not_found_string(key));

    return params.at(key);
}
