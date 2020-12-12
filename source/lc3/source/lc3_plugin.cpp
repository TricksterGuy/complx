#include "lc3/lc3_plugin.hpp"

#include <algorithm>
#include <dlfcn.h>
#include <sstream>

// Ugh macros that expand to gnu_dev_major/minor.  Undefined!
#undef major
#undef minor

#ifndef SHARED_LIBRARY_SUFFIX
#error "-DSHARED_LIBRARY_SUFFIX must be set to the suffix of dynamic libraries on this system."
#endif

#ifdef NDEBUG
static std::string FILENAME_SUFFIX;
#else
static std::string FILENAME_SUFFIX = "d";
#endif


#ifdef PLUGIN_INSTALL_PREFIX
static std::string PLUGIN_INSTALL_DIR = PLUGIN_INSTALL_PREFIX;
#else
static std::string PLUGIN_INSTALL_DIR;
#endif

void lc3_set_plugin_install_dir(const std::string& dir)
{
    PLUGIN_INSTALL_DIR = dir;
}

Plugin::Plugin(unsigned int mymajor, unsigned int myminor, unsigned int _type, const std::string& _desc) : major(mymajor),
    minor(myminor), type(_type), desc(_desc)
{

}

void Plugin::BindAddress(uint16_t address)
{
    addresses.insert(address);
}

void Plugin::BindNAddresses(uint16_t address, uint16_t length)
{
    if (address + length >= 0x10000U)
    {
        fprintf(stderr, "BindNAddresses address range (x%04x, x%05x) outside memory. Ignoring...\n", address, static_cast<int>(address) + length);
        return;
    }
    for (uint16_t i = address; i < address + length; i++)
        BindAddress(i);
}

void Plugin::BindInterrupt(unsigned char int_vector)
{
    interrupts.insert(int_vector);
}

TrapFunctionPlugin::TrapFunctionPlugin(unsigned int _major, unsigned int _minor, const std::string& desc, unsigned char _vector) :
    Plugin(_major, _minor, LC3_TRAP, desc), vector(_vector)
{

}

InstructionPlugin::InstructionPlugin(unsigned int major, unsigned int minor, const std::string& desc) :
    Plugin(major, minor, LC3_INSTRUCTION, desc)
{

}

std::list<RLEColorEntry> InstructionPlugin::GetInstructionColoring(uint16_t) const
{
    // By default do nothing. The GUI will fill this with the default unused bits color.
    std::list<RLEColorEntry> answer;
    return answer;
}

void lc3_install_plugin(lc3_state& state, const std::string& filename, const PluginParams& params)
{
    std::string realfilename = "lib" + filename + FILENAME_SUFFIX + SHARED_LIBRARY_SUFFIX;
    std::string full_path = realfilename;

    if (!PLUGIN_INSTALL_DIR.empty())
        full_path = PLUGIN_INSTALL_DIR + "/" + realfilename;

    void *hndl = dlopen(full_path.c_str(), RTLD_NOW | RTLD_GLOBAL);

    // Failed to load.
    if(hndl == nullptr)
        throw LC3PluginException(filename, full_path, dlerror());

    using RawPluginCreateFunc = Plugin*(const PluginParams&);
    PluginCreateFunc mkr = reinterpret_cast<RawPluginCreateFunc*>(dlsym(hndl, "create_plugin"));
    using RawPluginDestroyFunc = void(Plugin*);
    PluginDestroyFunc dstry = reinterpret_cast<RawPluginDestroyFunc*>(dlsym(hndl, "destroy_plugin"));

    // If failed to follow format (needs a creation and destruction function) not valid
    if (mkr == nullptr || dstry == nullptr)
        throw LC3PluginException(filename, full_path, "Plugin does not have correct creation/destruction functions or not found.");

    Plugin* plugin = mkr(PluginParams(filename, full_path, params));

    // If failed to create reject
    if (plugin == nullptr)
        throw LC3PluginException(filename, full_path, "Could not instantiate plugin");

    if (state.in_lc3test && !plugin->AvailableInLC3Test())
    {
        // Do not register, silently return.
        return;
    }

    PluginInfo infos;
    infos.filename = filename;
    infos.plugin = plugin;
    infos.create = mkr;
    infos.destroy = dstry;
    infos.handle = hndl;

    // If higher version or not the same major version then reject
    if (plugin->GetMajorVersion() != LC3_MAJOR_VERSION || plugin->GetMinorVersion() > LC3_MINOR_VERSION)
    {
        std::stringstream stream("Plugin is of incorrect version running version ");
        stream << LC3_MAJOR_VERSION << "." << LC3_MINOR_VERSION;
        stream << " was ";
        stream << plugin->GetMajorVersion() << "." << plugin->GetMinorVersion();

        throw LC3PluginException(filename, full_path, stream.str());
    }

    // Commit the addresses and interrupt vectors the plugin reserved.
    plugin->Commit();

    for (const auto& int_vector : plugin->GetBoundInterrupts()) {
        // If interrupt slot already filled reject
        if (state.interruptPlugin.find(int_vector) != state.interruptPlugin.end())
        {
            std::stringstream stream("Plugin interrupt vector x1");
            stream << std::hex << int_vector << " already used.";
            throw LC3PluginException(filename, full_path, stream.str());
        }
        state.interruptPlugin[int_vector] = plugin;
    }

    for (const auto& address : plugin->GetBoundAddresses())
    {
        // Fail if something already mapped to this address.
        if (state.address_plugins.find(address) != state.address_plugins.end())
        {
            std::stringstream stream("A plugin is already mapped to address x");
            stream << std::hex << address;
            throw LC3PluginException(filename, full_path, stream.str());
        }
        state.address_plugins[address] = plugin;
    }

    TrapFunctionPlugin* tfplugin;

    switch (plugin->GetPluginType())
    {
    case LC3_INSTRUCTION:
        // There can be only one!
        if (state.instructionPlugin != nullptr)
            throw LC3PluginException(filename, full_path, "There is already an instruction plugin loaded.");

        state.instructionPlugin = dynamic_cast<InstructionPlugin*>(plugin);
        // We should have a non null value.
        if (state.instructionPlugin == nullptr)
            throw LC3PluginException(filename, full_path, "Plugin is not an instruction plugin.");
        break;
    case LC3_TRAP:
        tfplugin = dynamic_cast<TrapFunctionPlugin*>(plugin);
        if (tfplugin == nullptr)
            throw LC3PluginException(filename, full_path, "Plugin is not a trap plugin");
        // Fail if something already mapped to this address.
        if (state.trapPlugins.find(tfplugin->GetTrapVector()) != state.trapPlugins.end())
        {
            std::stringstream stream("A Trap Function Plugin is already mapped to address x");
            stream << std::hex << tfplugin->GetTrapVector();

            throw LC3PluginException(filename, full_path, stream.str());
        }
        state.trapPlugins[tfplugin->GetTrapVector()] = tfplugin;
        break;
    case LC3_OTHER:
        state.plugins.push_back(plugin);
        break;
    default:
        std::stringstream stream("Unknown plugin type ");
        stream << plugin->GetPluginType();
        throw LC3PluginException(filename, full_path, stream.str());
    }
    // Register
    state.filePlugin[filename] = infos;
}

bool lc3_uninstall_plugin(lc3_state& state, const std::string& filename)
{
    if (state.filePlugin.find(filename) == state.filePlugin.end())
        return true;

    PluginInfo& infos = state.filePlugin[filename];

    switch(infos.plugin->GetPluginType())
    {
        case LC3_INSTRUCTION:
            state.instructionPlugin = nullptr;
            break;
        case LC3_TRAP:
            state.trapPlugins.erase(dynamic_cast<TrapFunctionPlugin*>(infos.plugin)->GetTrapVector());
            break;
        case LC3_OTHER:
            state.plugins.erase(std::find(state.plugins.begin(), state.plugins.end(), infos.plugin));
            break;
        default:
            return false;
    }

    for (const auto& int_vector : infos.plugin->GetBoundInterrupts())
        state.interruptPlugin.erase(int_vector);

    for (const auto& address : infos.plugin->GetBoundAddresses())
        state.address_plugins.erase(address);

    infos.destroy(infos.plugin);

    dlclose(infos.handle);

    state.filePlugin.erase(filename);

    return true;
}
