# Create a library called "libPhotos" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(PHOTOS_DIR       "Photos.3.52")
set(PHOTOS_SRC_DIR   "${PHOTOS_DIR}/src/")
#set(PHOTOS_EXAMPLE_DIR   "${PHOTOS_DIR}/examples/")

set(PHOTOS_EVENT_RECORD_INTERFACE_DIR   "${PHOTOS_SRC_DIR}/eventRecordInterfaces")
set(PHOTOS_FORTRAN_INTERFACE_DIR        "${PHOTOS_SRC_DIR}/photosFortranInterfaces")
set(PHOTOS_C_INTERFACE_DIR              "${PHOTOS_SRC_DIR}/photosCInterfaces")
set(PHOTOS_FORTRAN_DIR                  "${PHOTOS_SRC_DIR}/photos-fortran")
set(PHOTOS_UTILITIES_DIR                "${PHOTOS_SRC_DIR}/utilities")


set(INCLUDE_DIRECTORIES
   ./
   ${PHOTOS_EVENT_RECORD_INTERFACE_DIR}
   ${PHOTOS_FORTRAN_INTERFACE_DIR}
   ${PHOTOS_C_INTERFACE_DIR}
   ${PHOTOS_FORTRAN_DIR}
   ${PHOTOS_UTILITIES_DIR}
   ${HEPMC_INCLUDE_DIR}
   ${PYTHIA8_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
   ${HEPMC_LIB_DIR}
   ${PYTHIA8_LIB_DIR}
   ${FORTRAN_LIBDIR}
)

link_directories( ${LINK_DIRECTORIES})

set (CMAKE_Fortran_FLAGS "-ffixed-line-length-0")

set(PHOTOS_FORTRAN_SRCS
   ${PHOTOS_FORTRAN_INTERFACE_DIR}/forW-ME.f
   ${PHOTOS_FORTRAN_INTERFACE_DIR}/forZ-ME.f
   ${PHOTOS_FORTRAN_INTERFACE_DIR}/Photos_make.f
   ${PHOTOS_FORTRAN_DIR}/photos.f
)

set(PHOTOS_CXXINT_SRCS
   ${PHOTOS_FORTRAN_INTERFACE_DIR}/PH_HEPEVT_Interface.cxx
   ${PHOTOS_C_INTERFACE_DIR}/PhotosParticle.cxx
   ${PHOTOS_C_INTERFACE_DIR}/PhotosBranch.cxx
   ${PHOTOS_C_INTERFACE_DIR}/Photos.cxx
   ${PHOTOS_C_INTERFACE_DIR}/PhotosEvent.cxx
   ${PHOTOS_UTILITIES_DIR}/Log.cxx
   ${PHOTOS_UTILITIES_DIR}/PhotosRandom.cxx
   ${PHOTOS_EVENT_RECORD_INTERFACE_DIR}/PhotosHEPEVTEvent.cxx
   ${PHOTOS_EVENT_RECORD_INTERFACE_DIR}/PhotosHepMCEvent.cxx
   ${PHOTOS_EVENT_RECORD_INTERFACE_DIR}/PhotosHEPEVTParticle.cxx
   ${PHOTOS_EVENT_RECORD_INTERFACE_DIR}/PhotosHepMCParticle.cxx
)

if(RULE_CHECKER_FOUND)
  CHECK_RULES("${PHOTOS_FORTRAN_SRCS}" "${INCLUDE_DIRECTORIES}" PHOTOS_RULES)
  CHECK_RULES("${PHOTOS_CXXINT_SRCS}" "${INCLUDE_DIRECTORIES}" PHOTOS_RULES)
endif(RULE_CHECKER_FOUND)

add_library(PhotosFortran SHARED ${PHOTOS_FORTRAN_SRCS})
target_link_libraries(PhotosFortran ${Fortran_UseLib})
set_target_properties(PhotosFortran PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
set_target_properties(PhotosFortran PROPERTIES LINK_FLAGS "-fPIC")

add_library(PhotosCxxInterface SHARED ${PHOTOS_CXXINT_SRCS})
target_link_libraries(PhotosCxxInterface PhotosFortran HepMC)
set_target_properties(PhotosCxxInterface PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES})
set_target_properties(PhotosCxxInterface PROPERTIES LINK_FLAGS "-fPIC")

# # Version for pythia example - HepMC, Pythia8 and MC-Tester required
# photos_pythia_example.exe: photos_pythia_example.cxx
#         $(CC) $(CFLAGS) -I$(PHOTOSLOCATION)/include -I$(PYTHIALOCATION)/include \
#         -I$(MCTESTERLOCATION)/include -I$(HEPMCLOCATION)/include $(ROOTINC) -c $<
#         $(CC) -o $@ $(LDFLAGS) $(<:.cxx=.o) $(ROOTLIB) \
#         $(PYTHIA_LIBS) $(MC_TESTER_LIBS) $(HEPMC_LIBS) $(PHOTOS_LIBS) \
#         -Wl,-rpath,$(PREFIX)/lib \
#         -Wl,-rpath,$(PHOTOSLOCATION)/lib \
#         -Wl,-rpath,$(ROOTLIBPATH) \
#         -Wl,-rpath,$(HEPMCLOCATION)/lib \
#         -Wl,-rpath,$(PYTHIALOCATION)/lib \
#         -Wl,-rpath,$(MCTESTERLOCATION)/lib
# 
# # Version for photos gun example - HepMC and Pythia8 required
# single_photos_gun_example.exe: single_photos_gun_example.cxx
#         $(CC) $(CFLAGS) -I$(PHOTOSLOCATION)/include -I$(PYTHIALOCATION)/include \
#         -I$(HEPMCLOCATION)/include -c $<
#         $(CC) -o $@ $(LDFLAGS) $(<:.cxx=.o) $(HEPMC_LIBS) $(PYTHIA_LIBS) $(PHOTOS_LIBS) \
#         -Wl,-rpath,$(PREFIX)/lib \
#         -Wl,-rpath,$(PHOTOSLOCATION)/lib \
#         -Wl,-rpath,$(HEPMCLOCATION)/lib \
#         -Wl,-rpath,$(PYTHIALOCATION)/lib

#add_executable(single_photos_gun_example ${PHOTOS_EXAMPLE_DIR}/single_photos_gun_example.cxx)
#set_target_properties(single_photos_gun_example PROPERTIES OUTPUT_NAME single_photos_gun_example)
#set_target_properties(single_photos_gun_example PROPERTIES LINK_FLAGS "-fPIC")
#target_link_libraries(single_photos_gun_example ${ROOT_LIBRARIES} PhotosFortran PhotosCxxInterface HepMC  pythia8  lhapdfdummy hepmcinterface ${Fortran_UseLib} )

# 
# # Version for photos LCG example - HepMC and Pythia8 required
# photosLCG_pythia_example.exe: photosLCG_pythia_example.cxx
#         $(CC) $(CFLAGS) -I$(PHOTOSLOCATION)/include -I$(PYTHIALOCATION)/include \
#         -I$(HEPMCLOCATION)/include -c $<
#         $(CC) -o $@ $(LDFLAGS) $(<:.cxx=.o) $(HEPMC_LIBS) $(PYTHIA_LIBS) $(PHOTOS_LIBS) \
#         -Wl,-rpath,$(PREFIX)/lib \
#         -Wl,-rpath,$(PHOTOSLOCATION)/lib \
#         -Wl,-rpath,$(HEPMCLOCATION)/lib \
#         -Wl,-rpath,$(PYTHIALOCATION)/lib

#add_executable(photosLCG_pythia_example ${PHOTOS_EXAMPLE_DIR}/photosLCG_pythia_example.cxx)
#set_target_properties(photosLCG_pythia_example PROPERTIES OUTPUT_NAME photosLCG_pythia_example)
#set_target_properties(photosLCG_pythia_example PROPERTIES LINK_FLAGS "-fPIC")
#target_link_libraries(photosLCG_pythia_example ${ROOT_LIBRARIES} PhotosFortran PhotosCxxInterface HepMC  pythia8  lhapdfdummy hepmcinterface ${Fortran_UseLib} )

# 
# # Version for photos standalone example - only HepMC required
# photos_standalone_example.exe: photos_standalone_example.cxx
#         $(CC) $(CFLAGS) -I$(PHOTOSLOCATION)/include -I$(HEPMCLOCATION)/include -c $<
#         $(CC) -o $@ $(LDFLAGS) $(<:.cxx=.o) $(HEPMC_LIBS) $(PHOTOS_LIBS) \
#         -Wl,-rpath,$(PREFIX)/lib \
#         -Wl,-rpath,$(PHOTOSLOCATION)/lib \
#         -Wl,-rpath,$(HEPMCLOCATION)/lib

#add_executable(photos_standalone_example ${PHOTOS_EXAMPLE_DIR}/photos_standalone_example.cxx)
#set_target_properties(photos_standalone_example PROPERTIES OUTPUT_NAME photos_standalone_example)
#set_target_properties(photos_standalone_example PROPERTIES LINK_FLAGS "-fPIC")
#target_link_libraries(photos_standalone_example PhotosFortran PhotosCxxInterface HepMC ${Fortran_UseLib} )

# 
# # Version for photos HEPEVT example - no other libraries are required
# photos_hepevt_example.exe: photos_hepevt_example.cxx
#         $(CC) $(CFLAGS) -I$(PHOTOSLOCATION)/include -c $<
#         $(CC) -o $@ $(LDFLAGS) $(<:.cxx=.o) $(HEPMC_LIBS) $(PHOTOS_LIBS) \
#         -Wl,-rpath,$(PREFIX)/lib \
#         -Wl,-rpath,$(PHOTOSLOCATION)/lib \
#         -Wl,-rpath,$(HEPMCLOCATION)/lib

#add_executable(photos_hepevt_example ${PHOTOS_EXAMPLE_DIR}/photos_hepevt_example.cxx)
#set_target_properties(photos_hepevt_example PROPERTIES OUTPUT_NAME photos_hepevt_example)
#set_target_properties(photos_hepevt_example PROPERTIES LINK_FLAGS "-fPIC")
#target_link_libraries(photos_hepevt_example PhotosFortran PhotosCxxInterface HepMC ${Fortran_UseLib} )

################ install ###################
install(TARGETS PhotosFortran DESTINATION ${CMAKE_BINARY_DIR}/lib)
install(TARGETS PhotosCxxInterface DESTINATION ${CMAKE_BINARY_DIR}/lib)
