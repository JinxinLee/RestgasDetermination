# Create a library called "libPhotos" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
)
 
link_directories( ${LINK_DIRECTORIES})

set(PHOTOS_SRCS
phcork.F
phint1.F
phint.F
phlupa.F
phoan1.F
phoan2.F
phobo3.F
phobos.F
phocha.F
phochk.F
phocin.F
phocor.F
phodo.F
phoene.F
phoerr.F
phofac.F
phoin.F
phoinf.F
phoini.F
phomak.F
phooma.F
phoout.F
phopre.F
phoran.F
phorep.F
phorin.F
phoro2.F
phoro3.F
phospi.F
photos.F
photos_get.F
photos_make.F
photos_set.F
photri.F
photwo.F
phtype.F
)

if(RULE_CHECKER_FOUND) 
  CHECK_RULES("${PHOTOSDIRECT_SRCS}" "${INCLUDE_DIRECTORIES}" PHOTOSDIRECT_RULES) 
endif(RULE_CHECKER_FOUND) 

add_library(Photos SHARED ${PHOTOS_SRCS}) 
target_link_libraries(Photos ${Fortran_UseLib}) 
set_target_properties(Photos PROPERTIES ${FAIRROOT_LIBRARY_PROPERTIES}) 
set_target_properties(Photos PROPERTIES LINK_FLAGS "-fPIC") 

################ install ################### 
install(TARGETS Photos DESTINATION ${CMAKE_BINARY_DIR}/lib)