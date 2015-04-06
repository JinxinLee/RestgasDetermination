# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES} 
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field 
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)
 
link_directories( ${LINK_DIRECTORIES})

set(SRCS
PndConstField.cxx       
PndFieldContFact.cxx 
PndFieldMap.cxx      
PndFieldMapData.cxx  
PndSolenoidMap.cxx   
PndFieldPar.cxx      
PndFieldCreator.cxx  
PndDipoleMap.cxx     
PndTransMap.cxx      
PndRegion.cxx        
PndDipolePar.cxx     
PndDipole1Par.cxx     
PndDipole2Par.cxx     
PndDipole3Par.cxx     
PndTransPar.cxx
PndSolenoidPar.cxx
PndSolenoid1Par.cxx
PndSolenoid2Par.cxx
PndSolenoid3Par.cxx   
PndSolenoid4Par.cxx
PndMapPar.cxx        
PndMultiFieldPar.cxx 
PndConstPar.cxx    
PndMultiField.cxx  
)

set(LINKDEF FieldLinkDef.h)
set(LIBRARY_NAME Field)

set(DEPENDENCIES Base ParBase)

GENERATE_LIBRARY()
