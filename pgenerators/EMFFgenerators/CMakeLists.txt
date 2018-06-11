# Create a library called "libEMFFGEN" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${BASE_INCLUDE_DIRECTORIES}
)

set(INCLUDE_DIRECTORIES
${CMAKE_SOURCE_DIR}/fairtools
${CMAKE_SOURCE_DIR}/pgenerators/EMFFgenerators
${BASE_INCLUDE_DIRECTORIES}
)

Include_Directories( ${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

set(SRCS
mzfunctions.cxx
mzfunctions_pp_to_pipi_vandewi.cxx
mzfunctions_pp_to_leplep_vandewi.cxx
ranlxd.cxx
ranlxs.cxx
PndPiPiGenerator.cxx
PndLepLepGenerator.cxx
)

set(LINKDEF EMFFGENLinkDef.h)

set(LIBRARY_NAME EMFFGEN)

set(DEPENDENCIES Base)

GENERATE_LIBRARY()
