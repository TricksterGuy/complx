#include "lc3_plugin.hpp"
#include <dlfcn.h>
#include <algorithm>

// Ugh macros that expand to gnu_dev_major/minor.  Undefined!
#undef major
#undef minor

Plugin::Plugin(unsigned int mymajor, unsigned int myminor, unsigned int _type, const std::string& _desc, bool _interrupt_gen, unsigned char _intvector) : major(mymajor),
    minor(myminor), type(_type), desc(_desc), interrupt_cap(_interrupt_gen), intvector(_intvector)
{

}

Plugin::~Plugin()
{

}

DeviceRegisterPlugin::DeviceRegisterPlugin(unsigned int _major, unsigned int _minor, const std::string& desc, unsigned short _address, bool _interrupt_gen, unsigned char _intvector) :
    Plugin(_major, _minor, LC3_DEVICE, desc, _interrupt_gen, _intvector), address(_address)
{

}

DeviceRegisterPlugin::~DeviceRegisterPlugin()
{

}

TrapFunctionPlugin::TrapFunctionPlugin(unsigned int _major, unsigned int _minor, const std::string& desc, unsigned char _vector, bool _interrupt_gen, unsigned char _intvector) :
    Plugin(_major, _minor, LC3_TRAP, desc, _interrupt_gen, _intvector), vector(_vector)
{

}

TrapFunctionPlugin::~TrapFunctionPlugin()
{

}

InstructionPlugin::InstructionPlugin(unsigned int major, unsigned int minor, const std::string& desc, bool _interrupt_gen, unsigned char _intvector) :
    Plugin(major, minor, LC3_INSTRUCTION, desc, _interrupt_gen, _intvector)
{

}

InstructionPlugin::~InstructionPlugin()
{

}

std::vector<RLEColorEntry> InstructionPlugin::GetInstructionColoring(unsigned short instr) const
{
    // By default do nothing.
    std::vector<RLEColorEntry> answer;
    return answer;
}


/** lc3_install_plugin
  *
  * Installs a plugin given by the filename
  * @note the filename is minus the lib and .so extension.
  */
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

    // If going to generate interrupts and if interrupt slot already filled reject
    if (plugin->IsInterruptCapable() && state.interruptPlugin.find(plugin->GetInterruptVector()) != state.interruptPlugin.end())
    {
        fprintf(stderr, "Plugin %s interrupt vector x1%02x already used!\n", filename.c_str(), plugin->GetInterruptVector());
        return false;
    }

    DeviceRegisterPlugin* drplugin;
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
        case LC3_DEVICE:
            drplugin = dynamic_cast<DeviceRegisterPlugin*>(plugin);
            if (drplugin == NULL)
            {
                fprintf(stderr, "%s is not an Device Register Plugin!\n", filename.c_str());
                return false;
            }
            // Fail if not in correct area of memory < 0xFE00
            if (drplugin->GetAddress() < 0xFE00U)
            {
                fprintf(stderr, "%s: Device Register Plugins can only be mapped to addresses above xFE00!\n", filename.c_str());
                return false;
            }
            // Fail if something already mapped to this address.
            if (state.devicePlugins.find(drplugin->GetAddress()) != state.devicePlugins.end())
            {
                fprintf(stderr, "%s: A Device Register Plugin is already mapped to address %04x!\n", filename.c_str(), drplugin->GetAddress());
                return false;
            }
            state.devicePlugins[drplugin->GetAddress()] = drplugin;
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

    if (plugin->IsInterruptCapable())
        state.interruptPlugin[plugin->GetInterruptVector()] = plugin;

    return true;
}

/** lc3_uninstall_plugin
  *
  * Uninstalls a plugin given by the filename
  */
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
        case LC3_DEVICE:
            state.devicePlugins.erase(dynamic_cast<DeviceRegisterPlugin*>(infos.plugin)->GetAddress());
            break;
        case LC3_OTHER:
            state.plugins.erase(std::find(state.plugins.begin(), state.plugins.end(), infos.plugin));
            break;
        default:
            return false;
    }

    if (infos.plugin->IsInterruptCapable())
        state.interruptPlugin.erase(infos.plugin->GetInterruptVector());

    infos.destroy(infos.plugin);

    dlclose(infos.handle);

    state.filePlugin.erase(filename);

    return true;
}
