# Build Vc librariy which comes from an external source

Include(ExternalProject)

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-vc
                OUTPUT_VARIABLE ROOT_HAS_VC
               )
String(STRIP ${ROOT_HAS_VC} ROOT_HAS_VC)
Set(ROOT_HAS_VC ${ROOT_HAS_VC} PARENT_SCOPE)

If(NOT ${ROOT_HAS_VC})
  Message(STATUS "Root does not support VC. Compile the internal version.")

#old way?
  ExternalProject_Add(vc
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/vc
    PREFIX vc
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
    GIT_REPOSITORY https://github.com/VcDevel/Vc.git
    GIT_TAG 0.7.5
  )
  Add_Library(Vc STATIC IMPORTED GLOBAL)
  Set_Property(TARGET Vc PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libVc.a)
  add_dependencies(Vc vc)
  Set(VC_INCLUDE_DIRS
      ${CMAKE_SOURCE_DIR}/external/vc
      ${CMAKE_SOURCE_DIR}/external/vc/include/
      PARENT_SCOPE
     )
  Install(FILES ${CMAKE_BINARY_DIR}/lib/libVc.a DESTINATION lib)



# new way?

# Build Vc as external project

#set(_LIBDIR_DEFAULT "lib")
#set(Vc_VERSION "0.7.5")
##set(Vc_VERSION "1.3.2")
#set(Vc_SRC_URL "https://github.com/VcDevel/Vc")
#set(Vc_DESTDIR "${CMAKE_BINARY_DIR}/external/VC-prefix")
#set(Vc_ROOTDIR "${Vc_DESTDIR}/${CMAKE_BINARY_DIR}")
#set(Vc_LIBNAME "${CMAKE_STATIC_LIBRARY_PREFIX}Vc${CMAKE_STATIC_LIBRARY_SUFFIX}")
#set(Vc_LIBRARY "${Vc_ROOTDIR}/${_LIBDIR_DEFAULT}/${Vc_LIBNAME}")

#ExternalProject_Add(VC
  #GIT_REPOSITORY ${Vc_SRC_URL}
  #GIT_TAG ${Vc_VERSION}
  #BUILD_IN_SOURCE 0
  #BUILD_BYPRODUCTS ${Vc_LIBRARY}
  #LOG_DOWNLOAD 1 LOG_CONFIGURE 1 LOG_BUILD 1 LOG_INSTALL 1
  #CMAKE_ARGS -G ${CMAKE_GENERATOR}
             #-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
             #-DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
             #-DCMAKE_C_FLAGS=${CMAKE_C_FLAGS}
             #-DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
             #-DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
             #-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
  #INSTALL_COMMAND env DESTDIR=${Vc_DESTDIR} ${CMAKE_COMMAND} --build . --target install
#)

#add_library(Vc STATIC IMPORTED)
#set_target_properties(Vc PROPERTIES IMPORTED_LOCATION ${Vc_LIBRARY})
#add_dependencies(Vc VC)

#set(Vc_LIBRARIES Vc)
#set(Vc_INCLUDE_DIR "${Vc_ROOTDIR}/include")
#set(Vc_CMAKE_MODULES_DIR "${Vc_ROOTDIR}/${_LIBDIR_DEFAULT}/cmake/Vc")

#find_package_handle_standard_args(Vc
  #FOUND_VAR Vc_FOUND
  #REQUIRED_VARS Vc_INCLUDE_DIR Vc_LIBRARIES Vc_CMAKE_MODULES_DIR
  #VERSION_VAR Vc_VERSION
#)

#install(DIRECTORY ${Vc_ROOTDIR}/ DESTINATION ".")

#if(Vc_FOUND)
  ## Missing from VcConfig.cmake
  #set(VC_INCLUDE_DIRS ${Vc_INCLUDE_DIR} PARENT_SCOPE)
  #set(Vc_LIB_DIR ${Vc_ROOTDIR}/${_LIBDIR_DEFAULT} PARENT_SCOPE)
#endif()




EndIf()
