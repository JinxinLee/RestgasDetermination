// $Id: LmdLinkDef.h,v 1.0 M. Michel $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


//data
/*#pragma link C++ class PndSdsMCPoint+;
#pragma link C++ class PndSdsDigi+;
#pragma link C++ class PndSdsDigiPixel+;
#pragma link C++ class PndSdsPixel+;
#pragma link C++ class PndSdsStrip+;
#pragma link C++ class PndSdsApvHit+;*/

//SDS
#pragma link C++ class PndSdsDetector+;
#pragma link C++ class PndSdsGeoHandling+;
//#pragma link C++ class PndSdsContFact+;

//mc
//#pragma link C++ class PndMvdContFact+;
#pragma link C++ class PndLmdDetector+;

//digi
//#pragma link C++ class PndLmdHybridHitProducer+;
#pragma link C++ class PndLmdStripHitProducer+;
//#pragma link C++ class PndLmdCalcPixel;
//#pragma link C++ class PndLmdCalcFePixel;
//#pragma link C++ class PndLmdCalcStrip;
#pragma link C++ class PndLmdDigiTask+;
/*#pragma link C++ class PndLmdPixelDigiPar+;
#pragma link C++ class PndLmdStripDigiPar+;
#pragma link C++ class PndLmdDigiAna+;*/
#pragma link C++ class PndLmdHitProducerIdeal+;
#pragma link C++ class PndLmdNoiseProducer+;

//#pragma link C++ class PndLmdGeoHandling+;
//#pragma link C++ class PndMvdCalcFePixel+;

/*#pragma link C++ class PndMvdMCEventAna+;
#pragma link C++ class PndMvdEventAna+;
#pragma link C++ class PndMvdDigiEventAna+;
#pragma link C++ class PndMvdEventAna+;
#pragma link C++ class PndMvdMCEventAna+;
#pragma link C++ class PndMvdDigiEventAna+;
#pragma link C++ class PndMvdAllDataEventAna+;
#pragma link C++ class PndMvdFileNameCreator+;
#pragma link C++ class PndMvdAccessRTDBTask+;


#pragma link C++ class PndGeoHitList+;
#pragma link C++ class PndEventDisplay+;
#pragma link C++ class PndMvdEventAnaTask+;
#pragma link C++ class PndMvdEventMerger+;
#pragma link C++ class PndMvdEventMergerTask+;
#pragma link C++ class PndMvdMSAnaTask+;*/

//#pragma link C++ class PndStringVector+;

//#pragma link C++ class PndMvdConvertApv+;
//#pragma link C++ class PndMvdConvertApvTask+;


#endif

