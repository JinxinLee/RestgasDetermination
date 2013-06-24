# The extension is already found.  Any number of sources could be listed here.

if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 OLD)
endif(COMMAND cmake_policy)

set(INCLUDE_DIRECTORIES
   ${EVTGEN_INCLUDE_DIR}
   ${HEPMC_INCLUDE_DIR}
   ${PYTHIA8_INCLUDE_DIR}
   ${ROOT_INCLUDE_DIR}
   ${PHOTOS_INCLUDE_DIR}
   ${TAUOLA_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
   ${ROOT_LIBRARY_DIR}
   ${HEPMC_LIB_DIR}
   ${PYTHIA8_LIB_DIR}
)

link_directories( ${LINK_DIRECTORIES})

## Set list of example binaries (actually, thats their source code name)
## binaries in EvtGenBase are not created...
#set(ALLEvtGen_exe irinaEvtGen ggEvtGen ) ### not used, outdated, useless...
set(ALLEvtGen_exe simpleEvtGen simpleEvtGenRO pandaEvtGen)

foreach(program ${ALLEvtGen_exe})
	add_executable(EvtGen_exe_${program} ${program}.cc)
	set_target_properties(EvtGen_exe_${program} PROPERTIES OUTPUT_NAME ${program})
	set_target_properties(EvtGen_exe_${program} PROPERTIES LINK_FLAGS "-fPIC")
#   set_target_properties(EvtGen_exe_${program} PROPERTIES LINKER_LANGUAGE "Fortran")
	target_link_libraries(EvtGen_exe_${program} ${ROOT_LIBRARIES} ${EVTGENLIBLIST} ${EVTGENEXTLIBLIST} ${PHOTOSLIBLIST} ${TAUOLALIBLIST} ${HEPMCLIBLIST} ${PYTHIALIBLIST} ${Fortran_UseLib} )
endforeach(program)

################ install ###################
foreach(program ${ALLEvtGen_exe})
	install(TARGETS EvtGen_exe_${program} DESTINATION ${CMAKE_BINARY_DIR}/bin)
endforeach(program)

