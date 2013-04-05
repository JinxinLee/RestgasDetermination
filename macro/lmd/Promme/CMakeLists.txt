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
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/passive
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/PndTools/buffers
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/mvd
${CMAKE_SOURCE_DIR}/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/mvd/MvdMC
${CMAKE_SOURCE_DIR}/sds
${CMAKE_SOURCE_DIR}/sds/SdsMC
${CMAKE_SOURCE_DIR}/sds/SdsDigi
${CMAKE_SOURCE_DIR}/sds/SdsReco
${CMAKE_SOURCE_DIR}/sds/SdsTools
${CMAKE_SOURCE_DIR}/lmd
${CMAKE_SOURCE_DIR}/lmd/LmdMC
${CMAKE_SOURCE_DIR}/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/lmd/LmdReco
${CMAKE_SOURCE_DIR}/lmd/LmdTrack
${CMAKE_SOURCE_DIR}/lmd/LmdTools
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/PndTools/riemannfit
${CMAKE_SOURCE_DIR}/generators
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${PLUTO_LIBRARY_DIR} # needed for box generator
#${CMAKE_BINARY_DIR}/lib
#${CMAKE_LIBRARY_PATH}
)

link_directories( ${LINK_DIRECTORIES})

################################## install #####################################
# install(TARGETS Lmd DESTINATION ${CMAKE_BINARY_DIR}/lib)
add_executable(resolution_and_acceptance Resolution_and_acceptance.cxx)
# loading of libraries in the correct order is important!
# (-lSts -lGen -lRich -lTrd -lTof -lMvd -lGlobal -lKF -lDilept
#-lMuch -lBase -lField -lMCStack -lPassive -lGeoBase
#-lParBase)
target_link_libraries(resolution_and_acceptance ${ROOT_LIBRARIES} LmdTesting Lmd Sds Geane Geom Proof MathMore TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(radiation radiation.cxx)
target_link_libraries(radiation ${ROOT_LIBRARIES} Lmd Sds Geane Geom Proof MathMore TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)


add_executable(check_particle_path Check_particle_path.C)
target_link_libraries(check_particle_path ${ROOT_LIBRARIES} LmdTesting Lmd Sds Geane Geom Proof MathMore TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(createRootGeometry_beampipe createRootGeometry_beampipe.C)
# loading of libraries in the correct order is important!
# (-lSts -lGen -lRich -lTrd -lTof -lMvd -lGlobal -lKF -lDilept
#-lMuch -lBase -lField -lMCStack -lPassive -lGeoBase
#-lParBase)
target_link_libraries(createRootGeometry_beampipe ${ROOT_LIBRARIES} Geom GeoBase)

#add_executable(createRootGeometry_lmd_HV_MAPS create_Lmd_HV_MAPS.C)
#target_link_libraries(createRootGeometry_lmd_HV_MAPS ${ROOT_LIBRARIES} LmdTesting Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

#add_executable(interpolate_fieldmaps interpolate_fieldmaps.cxx)
#target_link_libraries(interpolate_fieldmaps ${ROOT_LIBRARIES} Base Field Passive Geom GeoBase)

#add_executable(lumi_simbox runLumiSimBox.cxx)
#target_link_libraries(lumi_simbox ${ROOT_LIBRARIES} Lmd Sds Base ParBase Field Passive Geom GeoBase Gen Pluto PndData TrkBase generalTools MathMore)
# Gen dependencies:  EG MathCore Physics RIO Cint Core 