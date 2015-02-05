if [ "$(id -u)" != "0" ]; then
	echo "Sorry, you are not root. Use the command sudo ./install.sh"
	exit 1
fi

type apt-get >/dev/null 2>&1 || { echo >&2 "APT required, perhaps you don't have a Debian variant installed?"; exit 1; }

echo "Installing package build-essential"
apt-get install build-essential > /dev/null
echo "Installing wxWidgets 3.0"
apt-get install libwxgtk3.0-dev > /dev/null
echo "Building program"
make
echo "Installing program"
make install
echo "Running ldconfig"
ldconfig
echo "Done have a nice day!"
