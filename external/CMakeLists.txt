# Build Vc librariy which comes from an external source under the name VcLegacy

Include(ExternalProject)

Message(STATUS "ROOT-Vc would be too new, compile the external legacy version.")

# We have to split up the commands in order to get it working
Execute_process(COMMAND mkdir -pv ${CMAKE_SOURCE_DIR}/external/vc
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
Execute_process(COMMAND git submodule deinit -- external/vc
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
Execute_process(COMMAND git submodule update --init -- external/vc
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

#old way?
ExternalProject_Add(vc
                    SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/vc
                    PREFIX vc
                    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external/vc
#    GIT_REPOSITORY https://github.com/VcDevel/Vc.git
#    GIT_TAG 0.7.5
                   )
Add_Library(VcLegacy STATIC IMPORTED GLOBAL)
Set_Property(TARGET VcLegacy PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/external/vc/lib/libVc.a)
add_dependencies(VcLegacy vc)
Set(VCLEGACY_INCLUDE_DIRS
    ${CMAKE_SOURCE_DIR}/external/vc
    ${CMAKE_SOURCE_DIR}/external/vc/include/
    PARENT_SCOPE
   )
#Install(FILES ${CMAKE_BINARY_DIR}/external/vc/lib/libVc.a DESTINATION lib RENAME libVcLegacy.a)
