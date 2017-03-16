# Create a library called "" which includes the source files given in
# the array .
# The extension is already found.  Any number of sources could be listed here.

Set(SYSTEM_INCLUDE_DIRECTORIES
  ${SYSTEM_INCLUDE_DIRECTORIES}
  ${GEANT3_INCLUDE_DIR}
  ${CLHEP_INCLUDE_DIR}
  ${BASE_INCLUDE_DIRECTORIES} 
)

Set(INCLUDE_DIRECTORIES
  ${CMAKE_SOURCE_DIR}/field
  ${CMAKE_SOURCE_DIR}/pnddata
  ${CMAKE_SOURCE_DIR}/pnddata/MvdData
  ${CMAKE_SOURCE_DIR}/pnddata/SdsData
  ${CMAKE_SOURCE_DIR}/pnddata/TrackData
  ${CMAKE_SOURCE_DIR}/passive
  ${CMAKE_SOURCE_DIR}/PndTools/generalTools
  ${CMAKE_SOURCE_DIR}/PndTrackers/riemannfit
  ${CMAKE_SOURCE_DIR}/PndTools/buffers
  ${CMAKE_SOURCE_DIR}/sds/SdsMC
  ${CMAKE_SOURCE_DIR}/sds/SdsTools
  ${CMAKE_SOURCE_DIR}/sds/SdsDigi
  ${CMAKE_SOURCE_DIR}/sds/SdsFEModel
  ${CMAKE_SOURCE_DIR}/sds/SdsReco
  ${CMAKE_SOURCE_DIR}/sds
  ${CMAKE_SOURCE_DIR}/trackbase
  ${CMAKE_SOURCE_DIR}/geane
  ${CMAKE_SOURCE_DIR}/PndTrackers/riemannfit
)

Include_Directories(${INCLUDE_DIRECTORIES})
Include_Directories(SYSTEM ${SYSTEM_INCLUDE_DIRECTORIES})

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})


########################### create libSds ######################################
set(SRCS

PndSdsTask.cxx

SdsDigi/PndSdsCalcFePixel.cxx
SdsDigi/PndSdsCalcPixel.cxx
SdsDigi/PndSdsCalcPixelDif.cxx
SdsDigi/PndSdsCalcStrip.cxx
#SdsDigi/PndSdsCalcStripDif.cxx
#MvdDigi/PndMvdDigiAna.cxx
SdsDigi/PndSdsHybridHitProducer.cxx
SdsDigi/PndSdsStripHitProducer.cxx
#SdsDigi/PndSdsStripHitProducerDif.cxx
SdsDigi/PndSdsHitProducerIdeal.cxx
#MvdDigi/PndMvdDigiTask.cxx
SdsDigi/PndSdsNoiseProducer.cxx

SdsDigi/PndSdsChargeConversion.cxx
SdsDigi/PndSdsIdealChargeConversion.cxx
SdsDigi/PndSdsTotChargeConversion.cxx
SdsDigi/PndSdsAdcChargeConversion.cxx
SdsDigi/PndSdsDigiPixelWriteoutBuffer.cxx

SdsDigi/PndSdsDigiPixelSorterTask.cxx
SdsDigi/PndSdsDigiPixelRingSorter.cxx

SdsDigi/PndSdsDigiStripWriteoutBuffer.cxx
SdsDigi/PndSdsDigiStripSorterTask.cxx
SdsDigi/PndSdsDigiStripRingSorter.cxx


SdsFEModel/PndSdsFE.cxx
SdsFEModel/PndSdsFESimple.cxx
SdsFEModel/PndSdsFEAmpModelSimple.cxx
SdsFEModel/PndSdsFESimple.cxx

SdsMC/PndSdsDetector.cxx
SdsMC/PndSdsGeo.cxx
SdsMC/PndSdsGeoPar.cxx
#SdsMC/PndSdsMaterialDet.cxx

#SdsTools/PndSdsContFact.cxx
SdsTools/PndSdsPixelDigiPar.cxx
SdsTools/PndSdsStripDigiPar.cxx
SdsTools/PndSdsTotDigiPar.cxx
#SdsTools/PndStringSeparator.cxx
#SdsTools/PndGeoHandling.cxx

)

set(LINKDEF  SdsLinkDef.h)
set(LIBRARY_NAME Sds)

set(DEPENDENCIES Base GeoBase ParBase PndData generalTools)

GENERATE_LIBRARY()
 

########################### create libSdsReco ##################################
set(SRCS

#SdsReco/PndSdsRecoTask.cxx
#SdsReco/PndSdsClusterTask.cxx
SdsReco/PndSdsPixelClusterFinder.cxx
SdsReco/PndSdsSimplePixelClusterFinder.cxx
SdsReco/PndSdsIdealPixelClusterFinder.cxx
SdsReco/PndSdsPixelClusterTask.cxx
SdsReco/PndSdsIdealClusterTask.cxx
SdsReco/PndSdsPixelBackMapping.cxx
SdsReco/PndSdsChargeWeightedPixelMapping.cxx
SdsReco/PndSdsStripClusterer.cxx
SdsReco/PndSdsSimpleStripClusterFinder.cxx
SdsReco/PndSdsStripAdvClusterFinder.cxx
SdsReco/PndSdsStripClusterTask.cxx
SdsReco/PndSdsIdealRecoTask.cxx
SdsReco/PndSdsChargeWeightingAlgorithms.cxx
SdsReco/PndSdsTimeWalkCorr.cxx
SdsReco/PndSdsTimeWalkCorrSimple.cxx
SdsReco/PndSdsTimeWalkCorrTask.cxx
SdsReco/PndSdsStripCorrelator.cxx

)

set(LINKDEF  SdsRecoLinkDef.h)
set(LIBRARY_NAME SdsReco)

set(DEPENDENCIES Base GeoBase ParBase PndData generalTools)

GENERATE_LIBRARY()
