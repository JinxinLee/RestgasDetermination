# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${CLHEP_INCLUDE_DIR} 
${BOOST_INCLUDE_DIR}/mpi
${CMAKE_SOURCE_DIR}/geobase 
${CMAKE_SOURCE_DIR}/parbase 
${CMAKE_SOURCE_DIR}/base 
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/mcstack
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tpc/tpcparallel
${CMAKE_SOURCE_DIR}/tpc/tpcreco
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${BOOST_LIBRARY_DIR}
${CMAKE_BINARY_DIR}/lib
)
 
link_directories( ${LINK_DIRECTORIES})

# There is a problem in the RuleChecker parsing some files. So
# we define different sets of files for the RuleChecker and
# the normal library. This setting is needed until we solve the
# problem with the RuleChecker

set(TPCPARA_SRCS_RULECHECKER
    ../PndTpcGem.cxx
    ../PndTpcGas.cxx
   ../PndTpcPadPlane.cxx
   ../PndTpcPad.cxx
   ../PndTpcDigi.cxx
   ../PndTpcPadShapePool.cxx
   ../PndTpcClusterFinder.cxx
   ../PndTpcCluster.cxx	    
   ../PndTpcClusterCOG.cxx
   ../PndTpcClusterRadius.cxx 
   ../PndTpcClusterZ.cxx	    
   ../TORPadProcessor.cxx 		    
   ../TORPPState_Initial.cxx 	    
   ../TORPPState_Compare.cxx 	    
   ../TORPPState_Output.cxx 	    
   ../TORPPState_Send.cxx 		    
   ../TORPPState_Wait.cxx 
   ../TORPPState_End.cxx 
   ../PndTpcSectorProcessor.cxx 
   ../McId.cxx 	
   ../McIdCollection.cxx
   ../PndTpcAbsPadShape.cxx
   ../PndTpcDigiAge.cxx 
   ../PndTpcDigiMapper.cxx
   ../PndTpcPRLookupTable.cxx
   ../PndTpcPadShapePolygon.cxx
   ../PndTpcPadShapeSquare.cxx
)

SET(TPCPARA_SRCS ${TPCPARA_SRCS_RULECHECKER}) 		    


if(RULE_CHECKER_FOUND)
  CHECK_RULES("${TPCPARA_SRCS_RULECHECKER}" "${INCLUDE_DIRECTORIES}" TPCPARA_RULES)
endif(RULE_CHECKER_FOUND)

# fill list of header files from list of source files
# by exchanging the file extension
CHANGE_FILE_EXTENSION(*.cxx *.h TPCPARA_HEADERS "${TPCPARA_SRCS}")


set(TPCPARA_LINKDEF  tpcparaLinkDef.h)
set(TPCPARA_DICTIONARY ${CMAKE_CURRENT_BINARY_DIR}/tpcparaDict.cxx) 

ROOT_GENERATE_DICTIONARY("${TPCPARA_HEADERS}" "${TPCPARA_LINKDEF}" "${TPCPARA_DICTIONARY}" "${INCLUDE_DIRECTORIES}")

SET(TPCPARA_SRCS ${TPCPARA_SRCS} ${TPCPARA_DICTIONARY})

add_library(tpcpara SHARED ${TPCPARA_SRCS})
target_link_libraries(tpcpara ${ROOT_LIBRARIES})
set_target_properties(tpcpara PROPERTIES ${CBMROOT_LIBRARY_PROPERTIES})


############### MPI binaries ##################
#add_custom_command(OUTPUT TpcParaCFM
#                   COMMAND mpic++ -I${BOOST_INCLUDE_DIR}/mpi TpcParaCFM -L${BOOST_LIB_DIR} -ltpcpara -lboost_mpi-gcc-mt-1_35 -lboost_serialization-gcc-d-1_35.a
#03                    DEPENDS tpcpara)


################ binaries ##################
set(CMAKE_CXX_COMPILER mpicc)
ADD_EXECUTABLE(TpcParaCFM TpcParaCFM.cxx)
target_link_libraries(TpcParaCFM tpcpara)





################ install ###################
install(TARGETS tpcpara DESTINATION ${CMAKE_BINARY_DIR}/lib)
#install(TARGETS TpcParaCFM DESTINATION ${CMAKE_BINARY_DIR}/bin)