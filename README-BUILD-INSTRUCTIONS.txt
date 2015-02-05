Note before you begin.  I assume some familarity with the linux terminal.  This is similar to the windows command prompt for you people on windows.  If you feel your terminal experience is low then I suggest you read up on the terminal (google it).  I also suggest in this case you download nautilus-open-terminal.

Also if you are using ubuntu do yourself a favor and install the package nautilus-open-terminal by executing the following command in the terminal (To get a terminal click the ubuntu icon on the top left hand corner of the screen and type Terminal in the search box)

sudo apt-get install nautilus-open-terminal

When you restart your computer/linux you should now be able to right click in any directory and bring up a terminal already pointed at the directory where you opened the terminal.

Automated Install instructions for ubuntu 14.04 and up
------------------------------------------------------
Just run sudo ./install.sh from a terminal

Manual Install instructions for ubuntu 14.04 and up
----------------------------------------------------
This is an expedited process and all you have to do is copy and paste (its Ctrl+Shift+V in the terminal) these 8 simple commands.

This is assuming you haven't cloned the repo yet if you have then you can skip lines 2 and 3 and just bring a terminal to the root of the source tree (see below definition if you don't know what that is)

sudo apt-get install git build-essential libwxgtk3.0-dev
git clone https://github.com/TricksterGuy/complx.git
cd complx
make
sudo make install
sudo ldconfig

If there are no error messages to run complx all you need to do is type complx
(Note you may have to run sudo complx the first time and then you may run just complx from then on.  Some crazy ubuntu problem probably...)

for an explanation of what you are doing here

1. You fetch packages git build-essential and libwxgtk3.0-dev
git allows you to use git from the command line (don't know what git is then do a google search).
build-essential installs programs essential for building things (mainly the c++ compiler)
libwxgtk3.0-dev is a dependency of complx you will need this to compile it.

2. Clone my repository from github.  You are more than welcome to look at the source code.

3. Change directory to complx the root of the source tree.

4. Invoke make which will follow the rules present in the file 'Makefile' and build complx and all of the other programs.  You will be exposed to make again later in the semester.

5. Install the programs and data to a known directory.

6. This step is required so complx will run correctly.  If you really want to know complx uses shared libraries (think dll files on windows) and this is needed to let your system know that it should update the list of libraries available on the system. 



Automatic Install instructions for ubuntu 13.10 and earlier
-----------------------------------------------------------
Abhijit Murthy was kind enough to write a script to do the manual instructions below.  I've changed it a little bit for my purposes.  If this works you should go thank him.

Just issue the following command

sudo ./install_script_ubuntu_13_10.sh

In the same directory as the root of the source tree.  If you do not know where this is it is the same directory where this file is located on your computer.
If you have installed nautilus-open-terminal you should be able to right click on this directory and select open terminal.  If you are still confused about this
type ls in the terminal and you should get the following *similar* output.

as2obj               complx.png                liblc3
asm.png              complx-tips.txt           Makefile
ChangeLog            install.sh                PattPatelAppA.pdf
complx               lc3plugins                README-BUILD-INSTRUCTIONS.txt
ComplxChangeLog.txt  lc3runner                 README.md
complx.desktop       lc3test
Complx.pdf           LC3TestDocumentation.pdf

See how the output includes install.sh and README-BUILD-INSTRUCTIONS.txt this is where you want to be.

The script should work on Linux Mint, Ubuntu, Debian squeeze and wheezy.  Your mileage may vary with other debian/ubuntu based systems.
If you want to support another distro then write an install script and send it to me.

Manual Install Instructions for ubuntu 13.10 and under begin here
--------------------------------------
IMPORTANT I STRONGLY SUGGEST YOU COPY AND PASTE THESE COMMANDS INTO THE TERMINAL AND DOUBLECHECK THAT IT IS CORRECT BEFORE ISSUING IT.
TO PASTE FROM THE CLIPBOARD INTO THE TERMINAL USE CTRL+SHIFT+V INSTEAD OF CTRL+V.

0: Please note what the codename of your ubuntu is as you will need to know later in the setup.  For most of you it should be saucy
Previous version names are raring (13.04) quantal (12.10) and precise (12.04).
If you are using Linux Mint Petra (1)6 should match with saucy Olivia (15) should match with raring 14 with quantal and so on.
If you do not know this type the following in a terminal "lsb_release -a"

You should get output similar to

brandon@ubuntu:~/Downloads/simplx$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 13.10
Release:	13.10
Codename:	saucy

1: Install the following package build-essential by executing the following command. 
sudo apt-get install build-essential
This gives you access to utiliies essential for building stuff.

2: Now we are going to install some dependancies for compiling complx and friends, usually I would give you a package since unfortunately the libraries needed to compile/run complx aren't available on the default repositories accessible on ubuntu, but I've found a repository that hosts the dependancies. So now the following commands will add the repository so you can install the dependancies  You can also compile wxWidgets 3.0.x from source however that is beyond the scope of this file.

  1. sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
  2. Remember step 0 where I told you to remember the code name here is where you will use it.  Replace saucy in the next command with your code name. Please note the space between ubuntu/ and saucy!
     sudo apt-add-repository 'deb http://repos.codelite.org/wx3.0/ubuntu/ saucy universe'

     ALTERNATIVE to this step.
     Open the file /etc/apt/sources.list
     Add the following lines to the end of this file
     
     deb http://repos.codelite.org/wx3.0/ubuntu/ saucy universe
     deb-src http://repos.codelite.org/wx3.0/ubuntu/ saucy universe
     
     replace saucy with whatever silly codename your linux is using.
  3. Run sudo apt-get update.  This will update the list of packages available.
  4. Install the dependancies with the following command
     sudo apt-get install libwxbase3.0-0-unofficial libwxbase3.0-dev libwxgtk3.0-0-unofficial libwxgtk3.0-dev wx3.0-headers wx-common

3: Now bring a terminal to the directory of the source tree (i.e. where this file is located along with folders complx, liblc3, lc3runner, as2obj) and just type the following command see above in automatic instructions if you still don't know where this is)
   make

4. If all goes well then it should not have given you any error messages.  If you got a compiler warning do let me (email: bwhitehead0308@gmail.com) or a TA know (TA will just pass it along to me).  If you get a compiler error check to make sure you followed the steps correctly and/or get a TA to debug your issue.

5. If everything is all well and good type the following two commands and you should be set up
   sudo make install
   sudo ldconfig

The first installs the programs and any required files to /usr/local/XXX
The second is required since one of the files installed is a shared library (windows users think of DLL files) and this command updates the list of shared libraries on the system and where they can be found.

Troubleshooting
---------------
Here is the part where I yell at you for making mistakes.  You'll get used to it.

Nothing yet :)

