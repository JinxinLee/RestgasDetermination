#!/bin/bash
unamestr=`uname`
unameversion=`uname -r`
source $PANDAROOT_SETTINGS/configSettings.sh
echo "#!/bin/bash" > Dart.cfg
echo "export LINUX_FLAVOR=$unamestr$unameversion" >> Dart.cfg
echo "export BUILDDIR=$PWD/../build" >> Dart.cfg
echo "export SOURCEDIR=$PWD" >> Dart.cfg
echo "export SIMPATH=$SIMPATH" >> Dart.cfg
echo "export FAIRROOTPATH=$FAIRROOTPATH" >> Dart.cfg
echo "export FAIRSOFT_VERSION=$FAIRSOFT_VERSION" >> Dart.cfg
echo "export FAIRROOT_VERSION=$FAIRROOT_VERSION" >> Dart.cfg
