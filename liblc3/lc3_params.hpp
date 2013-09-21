#ifndef LC3_PARAMS_HPP
#define LC3_PARAMS_HPP

#include <map>
#include <string>

typedef std::map<std::string, std::string> PluginParams;

bool lc3_params_has_key(const PluginParams& params, const std::string& key);
std::string lc3_params_get_value(const PluginParams& params, const std::string& key);

bool lc3_params_read_bool(const PluginParams& params, const std::string& key, bool& var);

bool lc3_params_read_char(const PluginParams& params, const std::string& key, char& var);
bool lc3_params_read_short(const PluginParams& params, const std::string& key, short& var);
bool lc3_params_read_int(const PluginParams& params, const std::string& key, int& var);
bool lc3_params_read_long(const PluginParams& params, const std::string& key, long& var);

bool lc3_params_read_uchar(const PluginParams& params, const std::string& key, unsigned char& var);
bool lc3_params_read_ushort(const PluginParams& params, const std::string& key, unsigned short& var);
bool lc3_params_read_uint(const PluginParams& params, const std::string& key, unsigned int& var);
bool lc3_params_read_ulong(const PluginParams& params, const std::string& key, unsigned long& var);

bool lc3_params_read_string(const PluginParams& params, const std::string& key, std::string& var);

#endif

