#ifndef LC3_PLUGIN_HPP
#define LC3_PLUGIN_HPP

#include <set>
#include <string>

#include "lc3/lc3.hpp"
#include "lc3/lc3_parser.hpp"
#include "lc3/lc3_params.hpp"

enum LC3_API PluginTypes
{
    INVALID = 0,
    LC3_INSTRUCTION,
    LC3_TRAP,
    LC3_OTHER,
    PLUGIN_TYPES_SIZE
};

/** Define version of lc3 any plugins that are not of the same version will be rejected. */
#define LC3_MAJOR_VERSION 1
#define LC3_MINOR_VERSION 6

/** Main class for complx's plugin system.
  *
  * Plugins can either be:
  *   a new instruction (replacing opcode 0xD) => subclass InstructionPlugin
  *   a new device register akin to KBSR, KBDR, DDR, DSR, MCR => subclass Plugin
  *   a trap subroutine => subclass TrapFunctionPlugin
  *   or anything else use this class directly being sure to pass in LC3_OTHER as the type
  *
  * This class gives you the power to do things whenever:
  *   1. An instruction is issued (before fetch)
  *   2. At the end of instruction execution
  *   3. After a memory address is read from
  *   4. After a memory address is written to
  */
class LC3_API Plugin
{
public:
    /** Constructor
      *
      * Takes in a version, a type of plugin (required) a description string
      * And if the plugin is capable of sending interrupts and its interrupt vector if it is capable
      */
    Plugin(unsigned int mymajor, unsigned int myminor, unsigned int _type = INVALID, const std::string& desc = "");
    virtual ~Plugin() {}

    /** Gets Major Version */
    unsigned int GetMajorVersion() const {return major;}
    /** Gets Minor Version */
    unsigned int GetMinorVersion() const {return minor;}
    /** Gets plugin type */
    unsigned int GetPluginType() const {return type;}
    /** Gets description of this plugin */
    const std::string& GetDescription() const {return desc;}
    /** Is this plugin capable of sending interrupts to lc3 */
    bool CanInterrupt() const {return !interrupts.empty();}
    /** Get the bound addresses the plugin will intercept reads/writes from */
    const std::set<unsigned short>& GetBoundAddresses() const {return addresses;}
    /** Get the bound interrupt vectors the plugin will send from */
    const std::set<unsigned char>& GetBoundInterrupts() const {return interrupts;}
    /** Able to run in lc3_test */
    virtual bool AvailableInLC3Test() const {return true;}

    /** OnRead
      *
      * Called when a memory address is about to be read allowing you to return any value you want dynamically.
      * For this function to be called, you must call BindAddress/BindAddressRange in your plugin's constructor.
      * @param state LC3State object.
      * @param address Address to be read from.
      * @return the data at that address, or some value dynamically.
      */
    virtual short OnRead(lc3_state& state, unsigned short address) {return state.mem[address];}
    /** OnWrite
      *
      * Called when a memory address is about to be written to allowing you to do whatever you want with the value.
      * For this function to be called, you must call BindAddress/BindAddressRange in your plugin's constructor.
      * @param state LC3State object.
      * @param address Address to written to.
      * @param value Value to write at address.
      */
    virtual void OnWrite(lc3_state& state, unsigned short address, short value) {state.mem[address] = value;}
    /** OnTick
      *
      * Called at the beginning of the instruction execution cycle exactly before an instruction is fetched.
      * This is not called when the user back steps.
      * @param state LC3State object.
      */
    virtual void OnTick(lc3_state& state) {}
    /** OnTock
      *
      * Called at the end of the instruction execution cycle exactly after an instruction is executed.
      * This is not called when the user back steps.
      * @param state LC3State object.
      */
    virtual void OnTock(lc3_state& state) {}

    /** Commit
      *
      * Users should not call this function directly.
      * Commits the bound addresses and interrupts.
      */
    void Commit() {locked = true;}

protected:
    /** BindAddresses
      *
      * Bind the plugin to subscribe for updates to an address.
      * Can be called multiple times to acquire multiple addresses.
      * This should only be called in your Plugin's constructor.
      * @param address Address to subscribe to.
      */
    void BindAddress(unsigned short address);
    /** BindNAddresses
      *
      * Bind the plugin to subscribe for updates for address range [address, address + length).
      * Can be called multiple times to acquire multiple addresses.
      * This should only be called in your Plugin's constructor.
      * @param address Starting address to subscribe to.
      * @param length Number of address to subscribe to starting from address.
      */
    void BindNAddresses(unsigned short address, unsigned short length);
    /** BindInterrupt
      *
      * Bind the plugin to use an Interrupt Vector.
      * Can be called multiple times to acquire multiple interrupt vectors.
      * This should only be called in your Plugin's constructor.
      * @param int_vector Interrupt vector to subscribe to.
      */
    void BindInterrupt(unsigned char int_vector);

private:
    Plugin& operator=(const Plugin& other);
    Plugin(const Plugin& other);
    unsigned int major;
    unsigned int minor;
    unsigned int type;
    std::string desc;
    bool locked;
    std::set<unsigned short> addresses;
    std::set<unsigned char> interrupts;
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
class LC3_API TrapFunctionPlugin : public Plugin
{
public:
    TrapFunctionPlugin(unsigned int major, unsigned int minor, const std::string& desc, unsigned char vector);
    virtual ~TrapFunctionPlugin() {}
    /** Gets special trap name so saying TrapName in assembly code automatically assembles to 0xF0<VECTOR8> */
    virtual std::string GetTrapName() const = 0;
    /** Gets trap vector plugin is bound to */
    unsigned char GetTrapVector() const {return vector;}
    /** OnExecute
      *
      * Handles execution of the trap.
      * Any changes made to lc3_state must be filled out in the lc3_state_change reference given.
      * @param state LC3State object.
      * @param changes Record of changes made to LC3State object as part of execution.
      */
    virtual void OnExecute(lc3_state& state, lc3_state_change& changes) = 0;
private:
    unsigned char vector;
};

struct LC3AssembleContext;

/**
  * For Instruction Coloring.
  * These determine the color of each bit in the Binary column in the GUI.
  * This can only control the least significant 12 bits of the binary representation.
  * These entries have a name, a color (r, g, b), and a number of bits to color.
  * The color can be overridden in the GUI via a config entry with name mapping to a color.
  * The ordering is from most significant to least significant bit.
  */
struct LC3_API RLEColorEntry
{
    std::string name = "";
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;
    unsigned char length = 0;
    bool operator==(const RLEColorEntry& other) const
    {
        return name == other.name && r == other.r && g == other.g && b == other.b && length == other.length;
    }
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
class LC3_API InstructionPlugin : public Plugin
{
public:
    InstructionPlugin(unsigned int major, unsigned int minor, const std::string& desc);
    virtual ~InstructionPlugin() {}
    /** A note to the assembler what the opcode of the new instruction is called */
    virtual std::string GetOpcode() const = 0;
    /** DoAssembleOne
      *
      * Called when an instruction needs to be assembled.
      * The entire part of the line containing the instruction is passed in.
      * @see LC3AssembleContext .
      * @param state LC3State object.
      * @param context LC3AssembleContext.
      */
    virtual unsigned short DoAssembleOne(lc3_state& state, LC3AssembleContext& context) = 0;
    /** OnDecode
      *
      * Called when the new instruction is decoded.
      * Fill out the lc3_instr object passed in.
      * @param state LC3State object.
      * @param data Bits to decode.
      * @param instr lc3_instr object to fill in.
      */
    virtual void OnDecode(lc3_state& state, unsigned short data, lc3_instr& instr) = 0;
    /** OnExecute
      *
      * Called when the new instruction is executed.
      * Changes to lc3_state must be recorded in the lc3_state_change object passed in
      * @param state LC3State object.
      * @param instruction Instruction data.
      * @param changes Record of changes made to LC3State object as part of execution.
      */
    virtual void OnExecute(lc3_state& state, lc3_instr& instruction, lc3_state_change& changes) = 0;
    /** OnDisassemble
      *
      * Called when a disassembled version of the new instruction is requested /
      * @param state LC3State object.
      * @param instruction Instruction data.
      * @param level Disassemble level.
      */
    virtual std::string OnDisassemble(lc3_state& state, lc3_instr& instr, unsigned int level) = 0;
    /** GetInstructionColoring
      *
      * Called to get the instruction coloring for this instruction.
      * Note that the opcode bits should not be included, the other 12 bits can be colored.
      * For example to color the instruction [data12 red]:
      * `return {{"MY_BITS", 255, 0, 0, 12}}`.
      * @param instr decoded bits for the instruction.
      * @return A vector containing several color entries.
      */
    virtual std::list<RLEColorEntry> GetInstructionColoring(unsigned short instr) const;
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

/** lc3_set_plugin_install_dir
  *
  * Sets the directory where plugins are searched for
  * @param dir Directory to search.
  */
void LC3_API lc3_set_plugin_install_dir(const std::string& dir);

/** lc3_install_plugin
  *
  * Installs a plugin given by the filename.
  * This should not be called directly.
  * @param state LC3State object.
  * @param filename Filename of the plugin minus the lib prefix and .so/.dll extension.
  * @param params Params to install the plugin, like the trap vector, or memory address to bind to.
  * @throws LC3PluginException if plugin fails to load for some reason.
  */
void LC3_API lc3_install_plugin(lc3_state& state, const std::string& filename, const std::map<std::string, std::string>& params);
/** lc3_uninstall_plugin
  *
  * Uninstalls a plugin given by the filename.
  * This should not be called directly.
  * @param state LC3State object.
  * @param filename Filename of the plugin minus the lib prefix and .so/.dll extension.
  */
bool LC3_API lc3_uninstall_plugin(lc3_state& state, const std::string& filename);

#endif
