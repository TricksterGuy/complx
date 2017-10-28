#include "lc3_plugin.hpp"
#include <dlfcn.h>
#include <algorithm>

// Ugh macros that expand to gnu_dev_major/minor.  Undefined!
#undef major
#undef minor

Plugin::Plugin(unsigned int mymajor, unsigned int myminor, unsigned int _type, const std::string& _desc) : major(mymajor),
    minor(myminor), type(_type), desc(_desc)
{

}

void Plugin::BindAddress(unsigned short address)
{
    addresses.insert(address);
}

void Plugin::BindNAddresses(unsigned short address, unsigned short length)
{
    if (address + length >= 0x10000U)
    {
        fprintf(stderr, "BindNAddresses address range (x%04x, x%05x) outside memory. Ignoring...\n", address, static_cast<int>(address) + length);
        return;
    }
    for (unsigned short i = address; i < address + length; i++)
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

std::vector<RLEColorEntry> InstructionPlugin::GetInstructionColoring(unsigned short instr) const
{
    // By default do nothing.
    std::vector<RLEColorEntry> answer;
    return answer;
}



bool lc3_install_plugin(lc3_state& state, const std::string& filename, const PluginParams& params)
{
    std::string realfilename = "lib" + filename + SO_SUFFIX;
    void *hndl = dlopen(realfilename.c_str(), RTLD_NOW);
    // Failed to load.
    if(hndl == NULL)
    {
        fprintf(stderr, "Plugin %s failed to load! Reason: %s\n", filename.c_str(), dlerror());
        return false;
    }

    PluginCreateFunc mkr = (PluginCreateFunc) dlsym(hndl, "create_plugin");
    PluginDestroyFunc dstry = (PluginDestroyFunc) dlsym(hndl, "destroy_plugin");

    // If failed to follow format (needs a creation and destruction function) not valid
    if (mkr == NULL || dstry == NULL)
    {
        fprintf(stderr, "Plugin %s does not have correct creation/destruction functions!\n", filename.c_str());
        return false;
    }

    Plugin* plugin = mkr(params);

    // If failed to create reject
    if (plugin == NULL)
    {
        fprintf(stderr, "Plugin %s failed to instantiate!\n", filename.c_str());
        return false;
    }

    if (state.in_lc3test && !plugin->AvailableInLC3Test())
    {
        // Do not register, silently return true
        return true;
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
        fprintf(stderr, "Plugin %s is of incorrect version running version %d.%d was %d.%d!\n", filename.c_str(),
                LC3_MAJOR_VERSION, LC3_MINOR_VERSION, plugin->GetMajorVersion(), plugin->GetMinorVersion());
        return false;
    }

    // Commit the addresses and interrupt vectors the plugin reserved.
    plugin->Commit();

    for (const auto& int_vector : plugin->GetBoundInterrupts()) {
        // If interrupt slot already filled reject
        if (state.interruptPlugin.find(int_vector) != state.interruptPlugin.end())
        {
            fprintf(stderr, "Plugin %s interrupt vector x1%02x already used!\n", filename.c_str(), int_vector);
            return false;
        }
        state.interruptPlugin[int_vector] = plugin;
    }

    for (const auto& address : plugin->GetBoundAddresses())
    {
        // Fail if something already mapped to this address.
        if (state.address_plugins.find(address) != state.address_plugins.end())
        {
            fprintf(stderr, "%s: A Plugin is already mapped to address x%04x!\n", filename.c_str(), address);
            return false;
        }
        state.address_plugins[address] = plugin;
    }

    TrapFunctionPlugin* tfplugin;

    switch (plugin->GetPluginType())
    {
    case LC3_INSTRUCTION:
        // There can be only one!
        if (state.instructionPlugin != NULL)
        {
            fprintf(stderr, "There can only be one Instruction Plugin %s not loaded!\n", filename.c_str());
            return false;
        }
        state.instructionPlugin = dynamic_cast<InstructionPlugin*>(plugin);
        // We should have a non NULL value.
        if (state.instructionPlugin == NULL)
        {
            fprintf(stderr, "%s is not an Instruction Plugin!\n", filename.c_str());
            return false;
        }
        break;
    case LC3_TRAP:
        tfplugin = dynamic_cast<TrapFunctionPlugin*>(plugin);
        if (tfplugin == NULL)
        {
            fprintf(stderr, "%s is not an Trap Function Plugin!\n", filename.c_str());
            return false;
        }
        // Fail if something already mapped to this address.
        if (state.trapPlugins.find(tfplugin->GetTrapVector()) != state.trapPlugins.end())
        {
            fprintf(stderr, "%s: A Trap Function Plugin is already mapped to address %02x!\n", filename.c_str(), tfplugin->GetTrapVector());
            return false;
        }
        state.trapPlugins[tfplugin->GetTrapVector()] = tfplugin;
        break;
    case LC3_OTHER:
        state.plugins.push_back(plugin);
        break;
    default:
        fprintf(stderr, "%s: Unknown Plugin Type!\n", filename.c_str());
        // Unknown type I don't support.
        return false;
    }

    // Register
    state.filePlugin[filename] = infos;
    return true;
}

bool lc3_uninstall_plugin(lc3_state& state, const std::string& filename)
{
    if (state.filePlugin.find(filename) == state.filePlugin.end())
        return true;

    PluginInfo& infos = state.filePlugin[filename];

    switch(infos.plugin->GetPluginType())
    {
        case LC3_INSTRUCTION:
            state.instructionPlugin = NULL;
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
