if [ "$(id -u)" != "0" ]; then
	echo "Sorry, you are not root. Use the command sudo ./install_script_fedora_21.sh"
	exit 1
fi

type yum >/dev/null 2>&1 || { echo >&2 "yum required, perhaps you don't have a Fedora variant installed?"; exit 1; }

echo "Installing Development Tools and Development Libraries"
yum -y groupinstall "Development Tools" "Development Libraries" > /dev/null
yum -y install gcc-c++ > /dev/null
echo "Installing wxWidgets 3.0"
yum -y install wxBase3 wxGTK3 wxGTK3-devel > /dev/null
echo "Creating link for wx-config"
ln -s /usr/libexec/wxGTK3/wx-config /usr/bin/wx-config
echo "Building program"
make
echo "Installing program"
make install
echo "Running ldconfig"
if ! grep -lq "^/usr/local/lib$" /etc/ld.so.conf; then
    echo "/usr/local/lib" >> /etc/ld.so.conf
fi
ldconfig
echo "Done, have a nice day!"
