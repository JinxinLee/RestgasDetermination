// $Id: LmdLinkDef.h,v 1.0 M. Michel $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

//data
#pragma link C++ class PndSdsMCPoint+;
#pragma link C++ class PndSdsDigi+;
#pragma link C++ class PndSdsStrip+;

//SDS
#pragma link C++ class PndSdsDetector+;
#pragma link C++ class PndSdsTask+;

//LMD
#pragma link C++ class PndLmdDetector+;
#pragma link C++ class PndLmdGeo+;
#pragma link C++ class PndLmdContFact+;
#pragma link C++ class PndLmdAlignPar+;

//digi
#pragma link C++ class PndLmdPixelHitProducerFast+;
#pragma link C++ class PndLmdHybridHitProducer+;
#pragma link C++ class PndLmdStripHitProducer+;
#pragma link C++ class PndLmdDigiTask+;
#pragma link C++ class PndLmdHitProducerIdeal+;
#pragma link C++ class PndLmdNoiseProducer+;

#endif

