#ifndef LOADING_OPTIONS_HPP
#define LOADING_OPTIONS_HPP

/** Method of initializing registers / memory when loading.
  * A nonzero value between -32767 thru 32767 means fill with that value.
  */
enum ValueInitializationMethod
{
    /** Zero out all */
    ZEROED = 0,
    /** Randomizes all */
    RANDOMIZE = 0x10000,
};

/** Cached options for File > Reload */
struct LoadingOptions
{
    /** File to load. */
    std::string file;
    /** Console input. */
    std::string console_input;
    /** How to initialize registers. */
    ValueInitializationMethod registers = RANDOMIZE;
    /** How to initialize memory. */
    ValueInitializationMethod memory = RANDOMIZE;
    /** Initial value of the PC. */
    unsigned short pc = 0x3000;
    /** Override true traps setting */
    bool true_traps = false;
    /** Override interrupts setting */
    bool interrupts = false;
    /** Override strict execution setting */
    bool strict_execution = true;
};

#endif
