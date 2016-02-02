# Create a library called "libEvtGenDirect" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${HEPMC_INCLUDE_DIR}
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
  ${EVTGEN_INCLUDE_DIR}
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/pgenerators/EvtGen/EvtGenDirect
)

if(EVTGEN_EXTERNAL)
   add_definitions("-DEVTGEN_EXTERNAL")
endif(EVTGEN_EXTERNAL)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
${HEPMC_LIB_DIR}
${FORTRAN_LIBDIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
   PndEvtGenDirect.cxx
)

set(LINKDEF  EvtGenDirectLinkDef.h)
set(LIBRARY_NAME EvtGenDirect)

set(DEPENDENCIES Base EvtGen EvtGenExternal)

GENERATE_LIBRARY()

