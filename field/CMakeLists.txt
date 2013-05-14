# Create a library called "libBase" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

set(INCLUDE_DIRECTORIES
${ROOT_INCLUDE_DIR} 
${BASE_INCLUDE_DIRECTORIES} 
${CMAKE_SOURCE_DIR}/field 
)

include_directories( ${INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
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
