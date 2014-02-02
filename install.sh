#!/bin/bash
 
DISTRO=$(lsb_release -si)
CODE=$(lsb_release -sc)
CODELITEAPTKEY='http://repos.codelite.org/CodeLite.asc'
CODELITEAPTSOURCE="deb http://repos.codelite.org/wx3.0/ubuntu/ $CODE universe"
COMPLXREPO='https://github.com/TricksterGuy/complx.git'
 
#if we aren't on Ubuntu use the debian repository
if [ $DISTRO != "Ubuntu"  -a  $DISTRO != "LinuxMint" ]; then
	if [[ $DISTRO = "Debian" ]]; then
		echo >&2 "Detected debian release $CODE"
		CODELITEAPTSOURCE="deb http://repos.codelite.org/wx3.0/debian/ $CODE libs"
	else
		DEBIANVERSION=$(cat /etc/debian_version)
		if [[ "$DEBIANVERSION" == *"wheezy"* ]]; then
			echo >&2 "Detected debian release wheezy"
			CODELITEAPTSOURCE="deb http://repos.codelite.org/wx3.0/debian/ wheezy libs"
		elif [[ $DEBIANVERSION == *"squeeze"* ]]; then
			echo >&2 "Detected debian release squeeze"
			CODELITEAPTSOURCE="deb http://repos.codelite.org/wx3.0/debian/ squeeze libs"
		else
			echo >&2 "Your version of Debian is unsupported. Please follow the manual instructions."
			exit 1
		fi
	fi
else
	echo >&2 "Detected Ubuntu or Ubuntu variant"
fi

type apt-get >/dev/null 2>&1 || { echo >&2 "APT required, perhaps you don't have a Debian variant installed?"; exit 1; }

echo >&2 "Installing adding repo $CODELITEAPTKEY ..."
sudo apt-key adv --fetch-keys $CODELITEAPTKEY
sudo apt-add-repository "$CODELITEAPTSOURCE"
echo >&2 "Updating list of packages..."
sudo apt-get update
 
echo >&2 "Installing package build-essential..."
gksu apt-get install build-essential

echo >&2 "Installing wxwidgets 3.0..."
sudo apt-get install libwxbase3.0-0-unofficial libwxbase3.0-dev libwxgtk3.0-0-unofficial libwxgtk3.0-dev wx3.0-headers wx-common

echo >&2 "Building complx..."
make

echo >&2 "Installing complx..."
gksu make install

echo >&2 "Running ldconfig..."
gksu ldconfig
