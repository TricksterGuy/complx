Note before you begin.  I assume some familarity with the linux terminal.  This is similar to the windows command prompt for you people on windows.  If you feel your terminal experience is low then I suggest you read up on the terminal (google it).  I also suggest in this case you download nautilus-open-terminal.

Steps to install this from source.

0: Please note what the codename of your ubuntu is as you will need to know later in the setup.  For most of you it should be raring
Previous version names are quantal (12.10) and precise (12.04).  If you are using Linux Mint Olivia (15) should match with raring 14 with quantal and so on.
If you do not know this type the following in a terminal "lsb_release -a"

You should get output similar to

brandon@ubuntu:~/Downloads/simplx$ lsb_release -a
No LSB modules are available.
Distributor ID:	Ubuntu
Description:	Ubuntu 13.04
Release:	13.04
Codename:	raring

Also if you are using ubuntu do yourself a favor and install the package nautilus-open-terminal by executing the following command in the terminal (To get a terminal click the ubuntu icon on the top left hand corner of the screen and type Terminal in the search box)

sudo apt-get install nautilus-open-terminal

When you restart your computer/linux you should now be able to right click in any directory and bring up a terminal already pointed at the directory where you opened the terminal.


Instructions begin here
-----------------------
1: Install the following package build-essential by executing the following command. 
sudo apt-get install build-essential
This gives you access to utiliies essential for building stuff.

2: Now we are going to install some dependancies for compiling complx and friends, usually I would give you a package since unfortunately the libraries needed to compile/run complx aren't available on the default repositories accessible on ubuntu, but I've found a repository that hosts the dependancies. So now the following commands will add the repository so you can install the dependancies  You can also compile wxWidgets 2.9.x from source however that is beyond the scope of this file.

  1. sudo apt-key adv --fetch-keys http://repos.codelite.org/CodeLite.asc
  2. Remember step 0 where I told you to remember the code name here is where you will use it.  Replace raring in the next command with your code name.
     sudo apt-add-repository 'deb http://repos.codelite.org/wx2.9/ubuntu/ raring universe'
  3. Run sudo apt-get update.  This will update the list of packages available.
  4. Install the dependancies with the following command
     sudo apt-get install libwxbase2.9-0-unofficial libwxbase2.9-dev libwxgtk2.9-0-unofficial libwxgtk2.9-dev wx2.9-headers wx-common

3: Now bring a terminal to the directory of the source tree (i.e. where this file is located along with folders complx, liblc3, lc3runner, as2obj) and just type the following command
   make

4. If all goes well then it should not have given you any error messages.  If you got a compiler warning do let me (email: bwhitehead0308@gmail.com) or a TA know (TA will just pass it along to me).  If you get a compiler error check to make sure you followed the steps correctly and/or get a TA to debug your issue.

5. If everything is all well and good type the following two commands and you should be set up
   sudo make install
   sudo ldconfig

The first installs the programs and any required files to /usr/local/XXX
The second is required since one of the files installed is a shared library (windows users think of DLL files) and this command updates the list of shared libraries on the system and where they can be found.

Troubleshooting
---------------
Here is the part where I yell at you for making mistakes.  You'll get used to it

1. i got a unable to fetch warning from the sudo apt-get update
Ok not yelling at you for this one, try again in a few minutes.

2. I get a Malformed line at XXX line in source list /etc/apt/sources.list
Congratulations for not following instructions correctly!  You mistyped the command "sudo apt-add-repository 'deb http://repos.codelite.org/wx2.9/ubuntu/ raring universe'"
And now for your punishment you must go and edit the file /etc/apt/sources.list
sudo gedit /etc/apt/sources.list
Please scroll to the line causing the problem and remove it.  In addition remove any line that includes "http://repos.codelite.org/wx2.9/ubuntu/"
Now you can either try the command again or you can manually add them by adding these two lines in its place

deb http://repos.codelite.org/wx2.9/ubuntu/ raring universe
deb-src http://repos.codelite.org/wx2.9/ubuntu/ raring universe

replace raring with whatever silly codename ubuntu is using.

3. I get this error when I type make
make[1]: g++: Command not found
make[1]: *** [ExpressionEvaluator.o] Error 127
make[1]: Leaving directory `/home/owen/Downloads/complx-master/liblc3'

Congratulations you also did not follow instructions correctly. You skipped this step (and the first step no less!)

"Install the following package build-essential by executing the following command. 
sudo apt-get install build-essential"

4. I get this error when I type make
make: *** No targets specified and no makefile found.  Stop.

For this I point you to this part in the instructions.
Now bring a terminal to the <<<directory of the source tree (i.e. where this file is located along with folders complx, liblc3, lc3runner, as2obj)>>>

Learn how to use the terminal.  To know if you are in the correct directly please type the following command
ls
and you should see a file named Makefile along with the directories listed above.
If you don't see those files then you will need to use cd (change directory) to get to that directory.

Also if you followed step 0 you can alternatively right click in that same directory right click and select Open Terminal.
