
SET(CMAKE_CXX_FLAGS -g)
#SET(CMAKE_VERBOSE_MAKEFILE ON)
ADD_DEFINITIONS(-DTESTCHAMBER)
PROJECT(TestChamber)

ENABLE_TESTING()

INCLUDE_DIRECTORIES($ENV{ROOTSYS}/include
					./..
					./src)


SET(SOURCES 	../PndTpcSample.cxx 
		../McId.cxx 
		../PndTpcDigi.cxx
		../McIdCollection.cxx
		../PndTpcSimplePSAStrategy.cxx
		../PndTpcPSA_TOT1.cxx
		../PndTpcPSAplot.cxx
		../PndTpcFrontend.cxx
		../PndTpcDigiMapper.cxx
		../PndTpcGem.cxx
		../PndTpcGas.cxx
		../PndTpcPad.cxx
		../PndTpcPadPlane.cxx
		../PndTpcPadShapePool.cxx
		../PndTpcPadShapePolygon.cxx
		../PndTpcPadShapeSquare.cxx
		../PndTpcAbsPadShape.cxx
		../PndTpcPRLookupTable.cxx
		../PndTpcCluster.cxx
		../PndTpcClusterFinder.cxx
		../PndTpcClusterFinderSimple.cxx
		../PndTpcSectorProcessor.cxx
		../PndTpcDigiAge.cxx
		../PndTpcDigiAmplitude.cxx
		../TORPadProcessor.cxx  
		../TORPPState_Compare.cxx 
		../TORPPState_Initial.cxx  
		../TORPPState_Output.cxx  
		../TORPPState_Send.cxx  
		../TORPPState_Wait.cxx
		../TORPPState_End.cxx
		src/dataReader.C
		src/Pedestals.cxx
		src/Calib.cxx
		src/StdoutKiller.cxx
		src/ConfigFile.cpp
		src/TCabsHough.cxx
		src/TCcluster.cxx
		src/TCclusterSort.cxx
		src/TCtrack.cxx
		src/TChough1.cxx
		src/TChoughPatternReco.cxx
		src/TCalign.cxx
		src/TCevent.cxx
		testChamberDict.cxx) 

FIND_PROGRAM(ROOT_CINT_EXECUTABLE
			NAMES rootcint
			PATHS $ENV{ROOTSYS}/bin
			NO_DEFAULT_PATH
)

ADD_CUSTOM_COMMAND(OUTPUT testChamberDict.cxx
      COMMAND ${ROOT_CINT_EXECUTABLE}
      ARGS -f testChamberDict.cxx -c -DHAVE_CONFIG_H ${INCLUDE_DIRECTORIES}
	  ../PndTpcCluster.h ../PndTpcSample.h ../PndTpcDigi.h ../McId.h ../McIdCollection.h src/TCcluster.h src/TCtrack.h src/TCevent.h  src/testChamberLinkDef.h)


ADD_LIBRARY(testChamber SHARED ${SOURCES})


#add ROOT lib
#SET(ROOT_LIB $ENV{ROOTSYS}/lib)
#SET(ROOT_LIBS -L${ROOT_LIB} -lMinuit -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lGui -pthread -lm -ldl -rdynamic)
EXEC_PROGRAM(
                $ENV{ROOTSYS}/bin/root-config
                ARGS --glibs --cflags
                OUTPUT_VARIABLE ROOTCONFIG)
                
SET(ROOT_LIBS ${ROOTCONFIG} -lMinuit)

TARGET_LINK_LIBRARIES(testChamber ${ROOT_LIBS})


ADD_EXECUTABLE(padplaneFileMaker src/padplaneFileMaker.cxx)
TARGET_LINK_LIBRARIES(padplaneFileMaker -lm)

ADD_EXECUTABLE(testChamberReco src/testChamberReco.cxx)
TARGET_LINK_LIBRARIES(testChamberReco testChamber)
#TARGET_LINK_LIBRARIES(testChamberReco analysis)
TARGET_LINK_LIBRARIES(testChamberReco ${ROOT_LIBS})

