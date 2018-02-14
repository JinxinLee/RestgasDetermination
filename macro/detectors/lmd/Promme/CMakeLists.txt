set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR}
${GEANT3_INCLUDE_DIR}
${CLHEP_INCLUDE_DIR}
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/geobase
${CMAKE_SOURCE_DIR}/parbase
${CMAKE_SOURCE_DIR}/base
${CMAKE_SOURCE_DIR}/base/steer
${CMAKE_SOURCE_DIR}/base/event
${CMAKE_SOURCE_DIR}/field
${CMAKE_SOURCE_DIR}/pnddata
${CMAKE_SOURCE_DIR}/pnddata/SdsData
${CMAKE_SOURCE_DIR}/pnddata/TrackData
${CMAKE_SOURCE_DIR}/detectors/passive
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/PndTools/buffers
${CMAKE_SOURCE_DIR}/tracking/riemannfit
${CMAKE_SOURCE_DIR}/PndTools/generalTools
${CMAKE_SOURCE_DIR}/PndTools/
${CMAKE_SOURCE_DIR}/detectors/mvd
${CMAKE_SOURCE_DIR}/detectors/mvd/MvdTracking
${CMAKE_SOURCE_DIR}/detectors/mvd/MvdMC
${CMAKE_SOURCE_DIR}/detectors/sds
${CMAKE_SOURCE_DIR}/detectors/sds/SdsMC
${CMAKE_SOURCE_DIR}/detectors/sds/SdsDigi
${CMAKE_SOURCE_DIR}/detectors/sds/SdsReco
${CMAKE_SOURCE_DIR}/detectors/sds/SdsTools
${CMAKE_SOURCE_DIR}/detectors/lmd
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdMC
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdDigi
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdReco
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdTrack
${CMAKE_SOURCE_DIR}/detectors/lmd/LmdTools
${CMAKE_SOURCE_DIR}/genfit
${CMAKE_SOURCE_DIR}/recotasks
${CMAKE_SOURCE_DIR}/trackbase
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/GeaneTrackRep
${CMAKE_SOURCE_DIR}/tracking/GenfitTools/trackrep/LSLtrackRep
${CMAKE_SOURCE_DIR}/geane
${CMAKE_SOURCE_DIR}/tpc
${CMAKE_SOURCE_DIR}/tracking/riemannfit
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
target_link_libraries(resolution_and_acceptance ${ROOT_LIBRARIES} MathMore LmdTrk)

#add_executable(radiation radiation.cxx)
#target_link_libraries(radiation ${ROOT_LIBRARIES} Lmd Sds Geane Geom Proof MathMore TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(scattered_particles scattered_particles.cxx)
target_link_libraries(scattered_particles ${ROOT_LIBRARIES} Lmd MathMore ) # MbsAPI generalTools Base GeoBase FairDB ParBase)

#add_executable(roman_crosscheck roman_crosscheck.cxx)
#target_link_libraries(roman_crosscheck ${ROOT_LIBRARIES} Lmd MathMore MbsAPI generalTools Base GeoBase FairDB ParBase)

add_executable(check_particle_path Check_particle_path.C)
target_link_libraries(check_particle_path ${ROOT_LIBRARIES} Lmd Sds MathMore) # Geane Geom Proof  TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(lmd_rad_dose_studies rad_dose_studies.C)
target_link_libraries(lmd_rad_dose_studies ${ROOT_LIBRARIES} MathMore Lmd Sds) # Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(lmd_time_based_studies time_based_studies.C)
target_link_libraries(lmd_time_based_studies ${ROOT_LIBRARIES} MathMore Lmd Sds) # Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(lmd_hit_noise_studies hit_noise_studies.C)
target_link_libraries(lmd_hit_noise_studies ${ROOT_LIBRARIES} MathMore Lmd Sds) # Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

#add_executable(lmd_online_monitoring_studies online_monitoring_studies.C)
#target_link_libraries(lmd_online_monitoring_studies ${ROOT_LIBRARIES} MathMore Lmd Sds) # Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

#add_executable(lmd_trafo_matrix_fit trafo_matrix_fit.C)
#target_link_libraries(lmd_trafo_matrix_fit ${ROOT_LIBRARIES} MathMore Lmd Sds) # Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(sandbox_promme sandbox.C)
target_link_libraries(sandbox_promme ${ROOT_LIBRARIES} Lmd Sds MathMore) # Geane Geom Proof MathMore TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

add_executable(createRootGeometry_beampipe createRootGeometry_beampipe.C)
# loading of libraries in the correct order is important!
# (-lSts -lGen -lRich -lTrd -lTof -lMvd -lGlobal -lKF -lDilept
#-lMuch -lBase -lField -lMCStack -lPassive -lGeoBase
#-lParBase)
target_link_libraries(createRootGeometry_beampipe ${ROOT_LIBRARIES} Geom GeoBase Lmd MathMore)

#add_executable(createRootGeometry_lmd_HV_MAPS create_Lmd_HV_MAPS.C)
#target_link_libraries(createRootGeometry_lmd_HV_MAPS ${ROOT_LIBRARIES} LmdTesting Geane Geom Proof TrkBase FairTools PndData VMC EG GeomPainter generalTools Base GeoBase FairDB ParBase)

#add_executable(interpolate_fieldmaps interpolate_fieldmaps.cxx)
#target_link_libraries(interpolate_fieldmaps ${ROOT_LIBRARIES} Base Field Passive Geom GeoBase)

add_executable(read_jost_fieldmaps read_jost_fieldmaps.cxx)
target_link_libraries(read_jost_fieldmaps ${ROOT_LIBRARIES} Base Field Passive Geom GeoBase)

add_executable(visualize_fieldmaps visualize_fieldmaps.cxx)
target_link_libraries(visualize_fieldmaps ${ROOT_LIBRARIES} Base Field Passive Geom GeoBase)

#add_executable(lumi_simbox runLumiSimBox.cxx)
#target_link_libraries(lumi_simbox ${ROOT_LIBRARIES} Lmd Sds Base ParBase Field Passive Geom GeoBase Gen Pluto PndData TrkBase generalTools MathMore)
# Gen dependencies:  EG MathCore Physics RIO Cint Core 
