complx
======

Extensible LC-3 Simulator written in C++.  Includes a Graphical User Interface and Command Line Interface simulator, Assembler, [Python Bindings](https://github.com/zucchini/pyLC3), and Python Autograder. Mainly used in CS2110 at Georgia Tech.  Maintained by Brandon (bwhitehead0308 [AT] gmail [DOT] com).

__Special Note__ Let me know if your school is using any portion of the tools provided here. The tools are subject to change every semester as new features are added or stuff is tweaked based on student feedback. I don't want to break anyone's setup, and would like to keep users in the loop with changes and suggest features.

# Overview
complx is a suite of educational tools for learning lc3 assembly. It includes both a gui and cli based simulator (named complx and comp respectively), an assembler (as2obj), python bindings via pyLC3, and an autograder framework written in python.  Complx also be extended with plugins that add additional functionality to the LC3.  The tools also come with a C++ interface to the LC3 (liblc3) along with python bindings to it via [pyLC3](https://github.com/zucchini/pyLC3). Complx, the GUI simulator, was written using the wxWidgets library.

# Installation

[Installing Prebuilt Binaries](https://github.com/TricksterGuy/complx/wiki/Installing-Prebuilt-Binaries)

[Build Instructions for Linux](https://github.com/TricksterGuy/complx/wiki/Building-for-Linux)

# Features
* Both a textual and graphical user interface available
* Support for all instructions including LC3 interrupts.
* Support for the LC-3 calling convention
* Comes with its own assembler
* Many options for running assembly code
  * Such as:
    * Backstepping/Undoing instructions
    * Stepping into/out/over a subroutine
    * Running X number of instructions
* Powerful Debugger
  * Ability to set breakpoints at specific locations
  * Ability to create watchpoints which trigger when a write to a register/memory location happens
  * Ability to set a temporary breakpoint/watchpoint that only triggers X times
  * Ability to mark a subroutine/trap as a blackbox and have the simulator automatically step over it
  * For the above ability to add a condition for the breakpoint/watchpoint to trigger
  * Ability to define all of the above in the assembly source file for easier debugging (and won't affect the grader)
  * Ability to view the activation stack and view subroutine parameters and rewind to a subroutine call
* Interface
  * Full view of LC-3 Memory with the ability to edit values in hexadecimal, decimal, binary, instruction, or add/edit/remove a label
  * Registers values can be edited and be displayed in different bases
  * Can also have multiple views of the lc-3 memory (useful for viewing the stack and the program at the same time)
  * Can rearrange memory view, and filter out memory addresses you don't care about
  * Console which accepts input and displays output (and various warnings)
  * Can view comments from code from within simulator
* Python bindings and a Python autograder framework via [pyLC3](https://github.com/zucchini/pyLC3)
* Ability to extend complx via plugins
  * Currently complx ships with 5 plugins
    1. Black and white Display device
    2. Colored Display device
    3. Random number generator that can be seeded via a write to its address
    4. Plugin that changes opcode 0xD with a multiplication instruction
    5. Plugin that adds a new trap that performs division and modulus
  * Plugins can add new device registers, traps, send interrupts, and add a new instruction

# Screenshot
Soon to come...
