#!/bin/bash
# test for architecture
arch=$(uname -s | tr '[A-Z]' '[a-z]')
chip=$(uname -m | tr '[A-Z]' '[a-z]')
echo "Generating Dart.cfg file"
echo ~
source ~/.bashrc
echo "PANDAROOT_SETTINGS=$PANDAROOT_SETTINGS"
source $PANDAROOT_SETTINGS/configSettings.sh
if [ "$arch" == "linux" ]; then
	if [ -f /etc/os-release ]; then
	    # freedesktop.org and systemd
	    . /etc/os-release
	    OS=$NAME
	    VER=$VERSION_ID
	elif type lsb_release >/dev/null 2>&1; then
	    # linuxbase.org
	    OS=$(lsb_release -si)
	    VER=$(lsb_release -sr)
	elif [ -f /etc/lsb-release ]; then
	    # For some versions of Debian/Ubuntu without lsb_release command
	    . /etc/lsb-release
	    OS=$DISTRIB_ID
	    VER=$DISTRIB_RELEASE
	elif [ -f /etc/debian_version ]; then
	    # Older Debian/Ubuntu/etc.
	    OS=Debian
	    VER=$(cat /etc/debian_version)
	elif [ -f /etc/SuSe-release ]; then
	    . /etc/SuSe-release
	    OS=SuSe
	    VER=$VERSION_ID
	elif [ -f /etc/redhat-release ]; then
	    . /etc/redhat-release
	    OS=$NAME
	    VER=$VERSION_ID
	else
	    # Fall back to uname, e.g. "Linux <version>", also works for BSD, etc.
	    OS=$(uname -s)
	    VER=$(uname -r)
	fi
elif [ "$arch" == "darwin" ]; then
	OS=OSX
	VER='sw_version -productVersion'
else
	# Fall back to uname, e.g. "Linux <version>", also works for BSD, etc.
	OS=$(uname -s)
	VER=$(uname -r)
fi
#Replace space with -
OS_String="${OS// /-}"

echo "export LINUX_FLAVOUR=$OS_String-$VER"
echo "export BUILDDIR=$PWD/../build"
echo "export SOURCEDIR=$PWD"
echo "export SIMPATH=$SIMPATH"
echo "export FAIRROOTPATH=$FAIRROOTPATH"
echo "export FAIRSOFT_VERSION=$FAIRSOFT_VERSION"
echo "export FAIRROOT_VERSION=$FAIRROOT_VERSION"
echo "#!/bin/bash" > Dart.cfg
echo "export LINUX_FLAVOUR=$OS_String-$VER" >> Dart.cfg
echo "export BUILDDIR=$PWD/../build" >> Dart.cfg
echo "export SOURCEDIR=$PWD" >> Dart.cfg
echo "export SIMPATH=$SIMPATH" >> Dart.cfg
echo "export FAIRROOTPATH=$FAIRROOTPATH" >> Dart.cfg
echo "export FAIRSOFT_VERSION=$FAIRSOFT_VERSION" >> Dart.cfg
echo "export FAIRROOT_VERSION=$FAIRROOT_VERSION" >> Dart.cfg
