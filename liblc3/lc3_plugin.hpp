#ifndef LC3_PLUGIN_HPP
#define LC3_PLUGIN_HPP

#include "lc3.hpp"
#include "lc3_parser.hpp"
#include "lc3_params.hpp"

enum PluginTypes
{
    INVALID = 0,
    LC3_INSTRUCTION,
    LC3_DEVICE,
    LC3_TRAP,
    LC3_OTHER,
    PLUGIN_TYPES_SIZE
};

#define LC3_MAJOR_VERSION 1
#define LC3_MINOR_VERSION 3

class Plugin
{
    public:
        Plugin(unsigned int mymajor, unsigned int myminor, unsigned int _type = INVALID, const std::string& desc = "", bool _interrupt_cap = false, unsigned char _intvector = 0);
        virtual ~Plugin();

        virtual void OnTick(lc3_state& state) {};
        virtual void OnTock(lc3_state& state) {};
        virtual void OnMemoryRead(lc3_state& state, unsigned short address) {};
        virtual void OnMemoryWrite(lc3_state& state, unsigned short address, short value) {};
        unsigned int GetMajorVersion() const {return major;};
        unsigned int GetMinorVersion() const {return minor;};
        unsigned int GetPluginType() const {return type;};
        const std::string& GetDescription() const {return desc;};
        bool IsInterruptCapable() const {return interrupt_cap;};
        unsigned char GetInterruptVector() const {return intvector;};

    private:
        Plugin& operator=(const Plugin& other);
        Plugin(const Plugin& other);
        unsigned int major;
        unsigned int minor;
        unsigned int type;
        std::string desc;
        bool interrupt_cap;
        unsigned char intvector;
};

class DeviceRegisterPlugin : public Plugin
{
    public:
        DeviceRegisterPlugin(unsigned int major, unsigned int minor, const std::string& desc, unsigned short address, bool _interrupt_cap = false, unsigned char _intvector = 0);
        virtual ~DeviceRegisterPlugin();

        virtual short OnRead(lc3_state& state) = 0;
        virtual void OnWrite(lc3_state& state, short value) = 0;
        unsigned short GetAddress() const {return address;};

    private:
        unsigned short address;
};

class TrapFunctionPlugin : public Plugin
{
    public:
        TrapFunctionPlugin(unsigned int major, unsigned int minor, const std::string& desc, unsigned char vector, bool _interrupt_cap = false, unsigned char _intvector = 0);
        virtual ~TrapFunctionPlugin();
        virtual std::string GetTrapName() const = 0;
        unsigned char GetTrapVector() const {return vector;}
        virtual void OnExecute(lc3_state& state, lc3_state_change& changes) = 0;
    private:
        unsigned char vector;
};

struct LC3AssembleContext;

// For Instruction Coloring.  These entries have a color and a number of bits to color. Going from most significant to least significant.
struct RLEColorEntry
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char length;
};

class InstructionPlugin : public Plugin
{
    public:
        InstructionPlugin(unsigned int major, unsigned int minor, const std::string& desc, bool _interrupt_cap = false, unsigned char _intvector = 0);
        virtual ~InstructionPlugin();
        virtual void OnDecode(lc3_state& state, unsigned short data, lc3_instr& instr) = 0;
        virtual void OnExecute(lc3_state& state, lc3_instr& instruction, lc3_state_change& changes) = 0;
        virtual std::string OnDisassemble(lc3_state& state, lc3_instr& instr, unsigned int level) = 0;
        virtual std::string GetOpcode() const = 0;
        virtual unsigned short DoAssembleOne(lc3_state& state, LC3AssembleContext& context) = 0;
        virtual std::vector<RLEColorEntry> GetInstructionColoring(unsigned short instr) const;
};

typedef Plugin* (*PluginCreateFunc)(const PluginParams&);
typedef void (*PluginDestroyFunc)(Plugin*);

struct PluginInfo
{
    std::string filename;
    PluginCreateFunc create;
    PluginDestroyFunc destroy;
    void* handle;
    Plugin* plugin;
};

bool lc3_install_plugin(lc3_state& state, const std::string& filename, const PluginParams& params);
bool lc3_uninstall_plugin(lc3_state& state, const std::string& filename);

#endif
