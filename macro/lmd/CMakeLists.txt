# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/GemData
${CMAKE_SOURCE_DIR}/pnddata/MdtData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/SttData
${CMAKE_SOURCE_DIR}/pnddata/FtsData
${CMAKE_SOURCE_DIR}/pnddata/SciTData
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/PndTools/buffers
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsReco
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/sds/SdsFEModel
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/stt
${CMAKE_SOURCE_DIR}/stt/sttreco
${CMAKE_SOURCE_DIR}/fts
${CMAKE_SOURCE_DIR}/lmd/LmdMC
${CMAKE_SOURCE_DIR}/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/lmd/LmdReco
${CMAKE_SOURCE_DIR}/lmd/LmdTrack
3${CMAKE_SOURCE_DIR}/lmd/LmdTools
${CMAKE_SOURCE_DIR}/lmd
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/RKTrackRep

${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/GenfitTools/recotasks

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
#${CMAKE_BINARY_DIR}/lib
#${CMAKE_LIBRARY_PATH}
)

link_directories( ${LINK_DIRECTORIES})

################################## install #####################################
# install(TARGETS Lmd DESTINATION ${CMAKE_BINARY_DIR}/lib)
add_executable(createRootGeometry_lmd_HV_MAPS create_Lmd_HV_MAPS.C)
#target_link_libraries(createRootGeometry_lmd_HV_MAPS ${ROOT_LIBRARIES} genfit genfitAdapters trackrep RecoHits Stt Fts Lmd LmdTrk Sds SdsReco Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase MathMore Minuit)
target_link_libraries(createRootGeometry_lmd_HV_MAPS ${ROOT_LIBRARIES} Lmd Sds Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase MathMore Minuit)

