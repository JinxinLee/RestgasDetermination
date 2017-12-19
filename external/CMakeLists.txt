# Build Vc librariy which comes from an external source

Include(ExternalProject)

Execute_Process(COMMAND ${ROOT_CONFIG_EXECUTABLE} --has-vc
                OUTPUT_VARIABLE ROOT_HAS_VC
               )
String(STRIP ${ROOT_HAS_VC} ROOT_HAS_VC)
Set(ROOT_HAS_VC ${ROOT_HAS_VC} PARENT_SCOPE)

If(NOT ${ROOT_HAS_VC})
  Message(STATUS "Root does not support VC. Compile the internal version.")
  ExternalProject_Add(vc
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/external/vc
    PREFIX vc
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
    GIT_REPOSITORY https://github.com/VcDevel/Vc.git
    GIT_TAG 0.7.5
  )
  Add_Library(Vc STATIC IMPORTED GLOBAL)
  Set_Property(TARGET Vc PROPERTY IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/lib/libVc.a)
  Set(VC_INCLUDE_DIRS       
      ${CMAKE_SOURCE_DIR}/external/vc 
      ${CMAKE_SOURCE_DIR}/external/vc/include/
      PARENT_SCOPE
     )
  Install(FILES ${CMAKE_BINARY_DIR}/lib/libVc.a DESTINATION lib)
EndIf()
