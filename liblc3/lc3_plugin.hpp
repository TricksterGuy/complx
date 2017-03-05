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

/** Define version of lc3 any plugins that are not of the same version will be rejected. */
#define LC3_MAJOR_VERSION 1
#define LC3_MINOR_VERSION 4

/** Main class for complx's plugin system.
  *
  * Plugins can either be a new instruction (replacing opcode 0xD) => InstructionPlugin
  * a new device register akin to KBSR, KBDR, DDR, DSR, MCR => DeviceRegisterPlugin
  * a trap subroutine => TrapFunctionPlugin
  * or anything else use this class directly being sure to pass in LC3_OTHER as the type
  *
  * This class gives you the power to do things whenever:
  * 1. An instruction is issued (before fetch)
  * 2. At the end of instruction execution
  * 3. After a memory address is read from
  * 4. After a memory address is written to
  */
class Plugin
{
public:
    /** Constructor
      *
      * Takes in a version, a type of plugin (required) a description string
      * And if the plugin is capable of sending interrupts and its interrupt vector if it is capable
      */
    Plugin(unsigned int mymajor, unsigned int myminor, unsigned int _type = INVALID, const std::string& desc = "", bool _interrupt_cap = false, unsigned char _intvector = 0);
    virtual ~Plugin();

    /** Called at the beginning of the instruction execution cycle exactly before an instruction is fetched
      * This is not called when the user back steps.
      */
    virtual void OnTick(lc3_state& state) {};
    /** Called at the end of the instruction execution cycle exactly after an instruction is executed
      * This is not called when the user back steps.
      */
    virtual void OnTock(lc3_state& state) {};
    /** Called when a memory address is read from.
      * This allows you to do some updating of lc3 state when memory is read from
      * Note this function will be called AFTER memory is read,
      * If you want to dynamically perform the read then you must use DeviceRegisterPlugin#OnRead
      * The use case for this is to subscribe for reads for an address, this is called any time a load instruction is executed.
      */
    virtual void OnMemoryRead(lc3_state& state, unsigned short address) {};
    /** Called when a memory address is written to.
      * This allows you to do some updating of lc3 state when memory is written to
      * Note this function will be called AFTER memory is written,
      * If you want to dynamically perform the write then you must use DeviceRegisterPlugin#OnWrite
      * The use case for this is to subscribe for writes for an address, this is called any time a store instruction is executed.
      */
    virtual void OnMemoryWrite(lc3_state& state, unsigned short address, short value) {};
    /** Gets Major Version */
    unsigned int GetMajorVersion() const
    {
        return major;
    };
    /** Gets Minor Version */
    unsigned int GetMinorVersion() const
    {
        return minor;
    };
    /** Gets plugin type */
    unsigned int GetPluginType() const
    {
        return type;
    };
    /** Gets description of this plugin */
    const std::string& GetDescription() const
    {
        return desc;
    };
    /** Is this plugin capable of sending interrupts to lc3 */
    bool IsInterruptCapable() const
    {
        return interrupt_cap;
    };
    /** Gets the interrupt vector used by this plugin */
    unsigned char GetInterruptVector() const
    {
        return intvector;
    };
    /** Able to run in lc3test */
    virtual bool AvailableInLC3Test() const
    {
        return true;
    }

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

/** Represents a device register a special memory address that will be bound to your plugin.
  *
  * The behavior of this will be akin to KBSR, KBDR, DSR, DDR, and MCR.
  * One address (> 0xFE00) will be marked as a special address any reads or write to that address will call OnRead and OnWrite respectively
  * Allowing you to update the device register state.
  *
  * You must implement the functions OnRead and OnWrite doing whatever you please.
  *
  * Binding the device register to multiple addresses is not supported at this time, you will need to have multiple device plugins one per address.
  * You can however subscribe to reads/writes from all addresss via OnMemoryRead/Write.
  */
class DeviceRegisterPlugin : public Plugin
{
public:
    DeviceRegisterPlugin(unsigned int major, unsigned int minor, const std::string& desc, unsigned short address, bool _interrupt_cap = false, unsigned char _intvector = 0);
    virtual ~DeviceRegisterPlugin();

    /** Called when a memory address is about to be read allowing you to return any value you want dynamically. */
    virtual short OnRead(lc3_state& state) = 0;
    /** Called when a memory address is about to be written to allowing you to do whatever you want with the value. */
    virtual void OnWrite(lc3_state& state, short value) = 0;
    /** Gets thte address the device register was bound to */
    unsigned short GetAddress() const
    {
        return address;
    };

private:
    unsigned short address;
};

/** Represents a trap subroutine bound to a specific trap vector.
  *
  * The behavior of this will be akin to GETC, OUT, IN, PUTS, PUTSP, and HALT
  * One location in the trap vector table (< 0xFF) will be marked as a special trap anytime TRAP xVector is called it will execute code you want.
  * As an added bonus hooks into the assembler was made, you may also customize your plugin to have a cool pseudoinstruction dedicated to it
  *
  * You must implement functions GetTrapName returning the psuedoinstuction name, and OnExecute doing whatever you please.
  * Please be a good citizen when implementing OnExecute, fill out any previous state you changed in the lc3_state_change object passed in or else backstepping will not work.
  */
class TrapFunctionPlugin : public Plugin
{
public:
    TrapFunctionPlugin(unsigned int major, unsigned int minor, const std::string& desc, unsigned char vector, bool _interrupt_cap = false, unsigned char _intvector = 0);
    virtual ~TrapFunctionPlugin();
    /** Gets special trap name so saying TrapName in assembly code automatically assembles to 0xF0<VECTOR8> */
    virtual std::string GetTrapName() const = 0;
    /** Gets trap vector plugin is bound to */
    unsigned char GetTrapVector() const
    {
        return vector;
    }
    /** Handles execution of the trap.  Please fill out any changes made to lc3_state to the lc3_state_change object */
    virtual void OnExecute(lc3_state& state, lc3_state_change& changes) = 0;
private:
    unsigned char vector;
};

struct LC3AssembleContext;

// For Instruction Coloring.  These entries have a color and a number of bits to color. Going from most significant to least significant bits.
struct RLEColorEntry
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char length;
};

/** Represents a brand new instruction replacing the ERROR instruction opcode 0xD
  *
  * Only one instruction plugin can be used at a time.
  * Implementation of this plugin is a little more involved and hooks were added into the assembler to make it work.
  *
  * You will need to implement DoAssembleOne to handling the assembling of this new instruction which will be called
  * whenever the opcode matches the result of GetOpcode exactly and found during assembling.
  *
  * Implement OnDissassemble to handle producing an assembly version of the new instruction (alternative a human readable version if the level is high enough).
  *
  * Implement OnDecode to handle filling out the lc3_instr object for use in OnExecute.
  *
  * Implement OnExecute to do whatever you wish being sure to fill out the lc3_state_change object or else backstepping will not work.
  *
  * Optionally implement GetInstructionColoring to make the instructions parameters look nice in the BinaryColumn in the MemoryView.
  */
class InstructionPlugin : public Plugin
{
public:
    InstructionPlugin(unsigned int major, unsigned int minor, const std::string& desc, bool _interrupt_cap = false, unsigned char _intvector = 0);
    virtual ~InstructionPlugin();
    /** A note to the assembler what the opcode of the new instruction is called */
    virtual std::string GetOpcode() const = 0;
    /** Called when an instruction needs to be assembled, the entire part of the line containing the instruction is passed in see LC3AssembleContext */
    virtual unsigned short DoAssembleOne(lc3_state& state, LC3AssembleContext& context) = 0;
    /** Called when the new instruction is decoded, fill out the lc3_instr object passed in. */
    virtual void OnDecode(lc3_state& state, unsigned short data, lc3_instr& instr) = 0;
    /** Called when the new instruction is executed.  Do whatever you want.  Be sure to mark any changes to lc3_state in the lc3_state_change object passed in */
    virtual void OnExecute(lc3_state& state, lc3_instr& instruction, lc3_state_change& changes) = 0;
    /** Called when a disassembled version of the new instruction is requested */
    virtual std::string OnDisassemble(lc3_state& state, lc3_instr& instr, unsigned int level) = 0;
    /** Optional called to get the instruction coloring for this instruction.
      * return an array containing several color entries
      * For example to color the instruction [opcode4 - green] [data12 red]
      * "return" {{0, 255, 0, 4}, {255, 0, 0, 12}}
      */
    virtual std::vector<RLEColorEntry> GetInstructionColoring(unsigned short instr) const;
};

/** Typedef of function that handles creation of a plugin */
typedef Plugin* (*PluginCreateFunc)(const PluginParams&);
/** Typedef of function that handles destroying a plugin */
typedef void (*PluginDestroyFunc)(Plugin*);


struct PluginInfo
{
    std::string filename;
    PluginCreateFunc create;
    PluginDestroyFunc destroy;
    void* handle;
    Plugin* plugin;
};

/** Function for installing a lc3 plugin.
  * You should not need to call this directly.
  */
bool lc3_install_plugin(lc3_state& state, const std::string& filename, const PluginParams& params);
/** Function for uninstalling an lc3 plugin.
  * You should not need to call this directly.
  */
bool lc3_uninstall_plugin(lc3_state& state, const std::string& filename);

#endif
