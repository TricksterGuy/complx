complx
======
[![Travis CI Build Status](https://travis-ci.org/TricksterGuy/complx.svg?branch=master)](https://travis-ci.org/TricksterGuy/complx)

Extensible LC-3 Simulator (GUI and CLI), Assembler, and Autograder/Test Framework written in C++. Mainly used in CS2110 at Georgia Tech.  Maintained by Brandon (bwhitehead0308 [AT] gmail [DOT] com)

# Screenshot
![Main Screen](https://github.com/TricksterGuy/complx/blob/master/doc/main_screen.png)

# Overview
complx is a suite of educational tools for learning lc3 assembly. It includes both a gui and cli based simulator (named complx and comp respectively), an assembler (as2obj), a very simple program that runs lc3 assembly files and spits out whats printed to the console (lc3runner), and a framework for testing and grading lc3 assembly code (lc3test).  Complx also be extended with plugins that add additional functionality to the LC3.  The tools also come with a C++ interface to the LC3 (liblc3). Complx was written using the wxWidgets library.

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
* Autograder framework lc3test which allows instructors/users to define tests in xml format and assign points (and partial credit) for each test.
* Ability to extend complx via plugins
  * Currently complx ships with 5 plugins
    1. Black and white Display device
    2. Colored Display device
    3. Random number generator that can be seeded via a write
    4. Plugin that changes opcode 0xD with a multiplication instruction
    5. Plugin that adds a new trap that performs division and modulus
  * Plugins can add new device registers, traps, send interrupts, and add a new instruction

# Installation from Packages via apt (Recommended! Only for Ubuntu 16.04.)
Execute these commands
* sudo add-apt-repository ppa:tricksterguy87/ppa-gt-cs2110
* sudo apt update
* sudo apt install complx-tools

# Installation from Source via scripts
You can run one of the install scripts depending on your platform, which does everything listed in the below section.
* Ubuntu 14.04 and above sudo ./install.sh

# Installation from Source manually
From the root directory of where you have the source code.
 1. Ensure you have a C++ compiler (sudo apt-get install build-essential)
 2. Ensure you have CMake installed (sudo apt-get install cmake)
 3. Install dependency wxWidgets 3.0 (sudo apt-get install libwxgtk3.0-dev)
 4. Create a build directory (mkdir build && cd build)
 5. Generate a Makefile via cmake (cmake ..)
 6. Build the program (make)
 7. Install the program (sudo make install)
 8. Run ldconfig (sudo ldconfig)

