#ifndef LC3_PARAMS_HPP
#define LC3_PARAMS_HPP

#include <map>
#include <string>

typedef std::map<std::string, std::string> PluginParams;

/** lc3_params_has_key
  *
  * Tests if the params has a key set.
  * @param params Plugin parameters.
  * @param key String key to search for.
  * @return true if key exists, false otherwise.
  */
bool lc3_params_has_key(const PluginParams& params, const std::string& key);
/** lc3_params_get_value
  *
  * Gets the value associated with the key given.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @return The value associated with the key.
  */
std::string lc3_params_get_value(const PluginParams& params, const std::string& key);
/** lc3_params_read_bool
  *
  * Reads a boolean value.
  * Valid values for the value can be yes/no, true/false, 1/0.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if a boolean was able to be parsed.
  */
bool lc3_params_read_bool(const PluginParams& params, const std::string& key, bool& var);
/** lc3_params_read_char
  *
  * Reads a char, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_char(const PluginParams& params, const std::string& key, char& var);
/** lc3_params_read_short
  *
  * Reads a short, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_short(const PluginParams& params, const std::string& key, short& var);
/** lc3_params_read_int
  *
  * Reads a int, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_int(const PluginParams& params, const std::string& key, int& var);
/** lc3_params_read_long
  *
  * Reads a long, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_long(const PluginParams& params, const std::string& key, long& var);
/** lc3_params_read_uchar
  *
  * Reads an unsigned char, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_uchar(const PluginParams& params, const std::string& key, unsigned char& var);
/** lc3_params_read_ushort
  *
  * Reads an unsigned short, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_ushort(const PluginParams& params, const std::string& key, unsigned short& var);
/** lc3_params_read_uint
  *
  * Reads an unsigned int, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_uint(const PluginParams& params, const std::string& key, unsigned int& var);
/** lc3_params_read_ulong
  *
  * Reads an unsigned long, parsing the value as if by the strtol function.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_ulong(const PluginParams& params, const std::string& key, unsigned long& var);
/** lc3_params_read_string
  *
  * Reads a string.
  * @param params Plugin parameters.
  * @param key String key to get value for.
  * @param var Output parameter to store result.
  * @return True if value was able to be parsed.
  */
bool lc3_params_read_string(const PluginParams& params, const std::string& key, std::string& var);

#endif

