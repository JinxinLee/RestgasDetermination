#!/bin/bash

##################################################################
# set the source and the build directory for ctest
# change this directories to your needs
# set the SIMPATH to your needs
# LINUX_FLAVOUR should be set to the distribution you are using
# eg Debian, SuSe etc.
# For example
# export SIMPATH=/misc/cbmsoft/Debian3.1/new
# export BUILDDIR=/misc/uhlig/test/mytest2/build_cbmroot
# export SOURCEDIR=/misc/uhlig/test/mytest2/cbmroot
# export LINUX_FLAVOUR=Debian
##################################################################
export SIMPATH=
export BUILDDIR=
export SOURCEDIR=
export LINUX_FLAVOUR=
###################################################################

if test  "x$SIMPATH" = "x" ; then
  echo ""
  echo "-- Error -- You don't set the needed variables in this shell script."
  echo "-- Error -- Please edit the script and do so."
  echo ""
  exit 1
fi

if [ "$#" -lt "1" ]; then
  echo ""
  echo "-- Error -- Please start script with the ctest model as an argument."
  echo "-- Error -- Possible arguments are Nightly and Experimental ."
  echo ""
  exit 1
fi
      
# set the ctest model to command line parameter
export ctest_model=$1

# extract information about the system and the machine and set
# environment variables used by ctest
SYSTEM=$(uname -o)
GCC_VERSION=$(gcc -dumpversion)
if test -z $GCC ; then
  COMPILER=gcc;
else
  COMPILER=$GCC;
fi
export LABEL=${LINUX_FLAVOUR}-$SYSTEM-$COMPILER$GCC_VERSION
export SITE=$(hostname -f)

echo "************************"
date
echo "LABEL: " $LABEL
echo "SITE: " $SITE
echo "Model: " ${ctest_model}
echo "************************"



# run ctest to clear the build directory, update the project from svn 
# and to create the config script for the project, submit results to
# the server
ctest -S $SOURCEDIR/PandaRoot_test1.cmake,${ctest_model} -V --VV --debug
          
# runing the config script sets all needed environment variables
. $BUILDDIR/config.sh

# since ctest didn't install the libraries one has to create logical
# links from the lib subdirectory to the libraries
# this is only a work around. better to do this inside ctest. 
cd $BUILDDIR
mkdir lib
cd lib
ln -s $BUILDDIR/base/libBase.so .
ln -s $BUILDDIR/cbmg4/libCbmG4.so .
ln -s $BUILDDIR/field/libField.so .
ln -s $BUILDDIR/generators/libGen.so .
ln -s $BUILDDIR/geobase/libGeoBase.so .
ln -s $BUILDDIR/parbase/libParBase.so .
ln -s $BUILDDIR/mcstack/libMCStack.so .
ln -s $BUILDDIR/passive/libPassive.so .
ln -s $BUILDDIR/dch/libDch.so .
ln -s $BUILDDIR/drc/drcprop/libDrcProp.so .
ln -s $BUILDDIR/drc/libDrc.so .
ln -s $BUILDDIR/emc/libEmc.so .
ln -s $BUILDDIR/hyp/libHyp.so .
ln -s $BUILDDIR/mdt/libMdt.so .
ln -s $BUILDDIR/mvd/libMvd.so .
ln -s $BUILDDIR/mvd/libMvdReco.so .
ln -s $BUILDDIR/mvd/libMvdTrk.so .
ln -s $BUILDDIR/pgenerators/libPGen.so .
ln -s $BUILDDIR/pgenerators/DpmEvtGen/libDpmEvtGen.so .
ln -s $BUILDDIR/stt/libStt.so .
ln -s $BUILDDIR/stt/sttreco/libSttReco.so .
ln -s $BUILDDIR/PndTools/riemannfit/libriemann.so .
ln -s $BUILDDIR/tpc/libtpc.so .
ln -s $BUILDDIR/tpc/tpcreco/libtpcreco.so .
ln -s $BUILDDIR/genfit/libgenfit.so .
ln -s $BUILDDIR/recotasks/librecotasks.so .
ln -s $BUILDDIR/trackbase/libTrkBase.so .
ln -s $BUILDDIR/trackrep/libtrackrep.so .
ln -s $BUILDDIR/tof/libTof.so .
ln -s $BUILDDIR/lhetrack/libLHETrack.so .
ln -s $BUILDDIR/rpc/libRpc.so .

# update from svn, build makefiles, build libraries, run tests and submit
# results to the server
cd $SOURCEDIR
ctest -S $SOURCEDIR/PandaRoot_test.cmake,${ctest_model} -V --VV --debug
