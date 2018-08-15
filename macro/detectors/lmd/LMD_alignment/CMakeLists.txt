# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${BASE_INCLUDE_DIRECTORIES}
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
${CMAKE_SOURCE_DIR}/detectors/passive
${CMAKE_SOURCE_DIR}/tools/
${CMAKE_SOURCE_DIR}/tools/buffers
${CMAKE_SOURCE_DIR}/tracking/riemannfit
${CMAKE_SOURCE_DIR}/tools
${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
${CMAKE_SOURCE_DIR}/detectors/sds/SdsDigi
${CMAKE_SOURCE_DIR}/detectors/sds/SdsReco
${CMAKE_SOURCE_DIR}/detectors/sds/SdsTools
${CMAKE_SOURCE_DIR}/detectors/sds/SdsFEModel
${CMAKE_SOURCE_DIR}/detectors/sds
${CMAKE_SOURCE_DIR}/detectors/stt
${CMAKE_SOURCE_DIR}/detectors/stt/sttreco
${CMAKE_SOURCE_DIR}/detectors/fts
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdMC
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdReco
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdTrack
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdTools
${CMAKE_SOURCE_DIR}/detectors/lmd
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/RKTrackRep

${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recohits
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/recotasks

)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${GLPK_LIB_DIR}
#${CMAKE_BINARY_DIR}/lib
#${CMAKE_LIBRARY_PATH}
)

link_directories( ${LINK_DIRECTORIES})

################################## install #####################################
# install(TARGETS Lmd DESTINATION ${CMAKE_BINARY_DIR}/lib)
add_executable(hits_align_pixel CreateHitsForAlignmentPixel.C)
add_executable(analyze_alignment Analyze.cpp)

target_link_libraries(hits_align_pixel -lrt ${ROOT_LIBRARIES} Lmd GeoBase ParBase Geom PndData TrkBase  VMC EG GeomPainter generalTools FairTools  LmdReco LmdTrk Geane trackrep RecoHits genfitAdapters genfit SdsReco Sds Stt Fts Proof MathMore Minuit FairDB Base)
target_link_libraries(analyze_alignment -lrt ${ROOT_LIBRARIES} Lmd GeoBase ParBase Geom PndData TrkBase  VMC EG GeomPainter generalTools FairTools  LmdReco LmdTrk Geane trackrep RecoHits genfitAdapters genfit SdsReco Sds Stt Fts Proof MathMore Minuit FairDB Base)
