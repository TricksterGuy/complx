#ifndef LC3_PARAMS_HPP
#define LC3_PARAMS_HPP

#include <lc3/lc3_api.h>
#include <map>
#include <string>

/** Exception thrown on loading / creating plugins */
class LC3PluginException : public std::exception
{
public:
    LC3PluginException(const std::string& plugin, const std::string& plugin_path, const std::string& msg);
    const char* what() const noexcept override { return error.c_str(); }
private:
    std::string form_error_message() const noexcept;
    std::string error;
    std::string name;
    std::string path;
    std::string message;
};

/** Class holding parameters and plugin information.
  * Plugins will be able to parse parameters in various types from the parameters map.
  */
class LC3_API PluginParams
{
public:
    PluginParams() = default;
    /** Constructor
      *
      * Constructs PluginParams with plugin name, path to plugin, and map of parameter values.
      * @param name Plugin name
      * @param plugin_path Full path to the associated plugin filename.
      * @param params Map of key, value pairs of parameters.
      */
    PluginParams(const std::string& name, const std::string& plugin_path, const std::map<std::string, std::string>& params);
    /** has_key
      *
      * Tests if the params has a key set.
      * @param key String key to search for.
      * @return true if key exists, false otherwise.
      */
    bool has_key(const std::string& key) const;
    /** get_value
      *
      * Gets the value associated with the key given.
      * @param key String key to get value for.
      * @return The value associated with the key or empty string if not found.
      */
    std::string get_value(const std::string& key) const;
    /** read_bool
      *
      * Reads an Optional boolean value.
      * Valid values for the value can be yes/no, on/off, true/false, 1/0.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_bool(const std::string& key, bool& var) const;
    /** read_char
      *
      * Reads a char, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_char(const std::string& key, char& var) const;
    /** read_int16_t
      *
      * Reads a int16_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_int16_t(const std::string& key, int16_t& var) const;
    /** read_int
      *
      * Reads a int, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_int(const std::string& key, int& var) const;
    /** read_int64_t
      *
      * Reads a int64_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_int64_t(const std::string& key, int64_t& var) const;
    /** read_uchar
      *
      * Reads an unsigned char, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_uchar(const std::string& key, unsigned char& var) const;
    /** read_uint16_t
      *
      * Reads an uint16_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_uint16_t(const std::string& key, uint16_t& var) const;
    /** read_uint
      *
      * Reads an unsigned int, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_uint(const std::string& key, unsigned int& var) const;
    /** read_uint64_t
      *
      * Reads an unsigned int64_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_uint64_t(const std::string& key, uint64_t& var) const;
    /** read_string
      *
      * Reads a string.
      * @param key String key to get value for.
      * @param var Output parameter to store result.
      * @return True if value was found and able to be parsed. False if not found.
      * @throws LC3PluginException if key was found, but unable to be parsed.
      */
    bool read_string(const std::string& key, std::string& var) const;


    /** read_bool_required
      *
      * Reads a boolean value.
      * Valid values for the value can be yes/no, on/off, true/false, 1/0.
      * @param key String key to get value for.
      * @return The boolean that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    bool read_bool_required(const std::string& key) const;
    /** read_char_required
      *
      * Reads a char, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @return The char that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    char read_char_required(const std::string& key) const;
    /** read_int16_t_required
      *
      * Reads a int16_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @return The int16_t that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    int16_t read_int16_t_required(const std::string& key) const;
    /** read_int_required
      *
      * Reads a int, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @return The int that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    int read_int_required(const std::string& key) const;
    /** read_int64_t_required
      *
      * Reads a int64_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @return The int64_t that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    int64_t read_int64_t_required(const std::string& key) const;
    /** read_uchar_required
      *
      * Reads an unsigned char, parsing the value as if by the strtoul function.
      * @param key String key to get value for.
      * @return The unsigned char that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    unsigned char read_uchar_required(const std::string& key) const;
    /** read_uint16_t_required
      *
      * Reads an uint16_t, parsing the value as if by the strtoul function.
      * @param key String key to get value for.
      * @return The uint16_t that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    uint16_t read_uint16_t_required(const std::string& key) const;
    /** read_uint_required
      *
      * Reads an unsigned int, parsing the value as if by the strtoul function.
      * @param key String key to get value for.
      * @return The unsigned int that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    unsigned int read_uint_required(const std::string& key) const;
    /** read_uint64_t_required
      *
      * Reads an uint64_t, parsing the value as if by the strtol function.
      * @param key String key to get value for.
      * @return The uint64_t that was read.
      * @throws LC3PluginException if key was not found, or could not be parsed.
      */
    uint64_t read_uint64_t_required(const std::string& key) const;
    /** read_string_required
      *
      * Reads a string.
      * @param key String key to get value for.
      * @return The string that was read.
      * @throws LC3PluginException if key was not found.
      */
    const std::string& read_string_required(const std::string& key) const;
private:
    std::string name;
    std::string full_path;
    std::map<std::string, std::string> params;
};

#endif

