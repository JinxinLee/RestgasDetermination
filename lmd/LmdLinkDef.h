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

//mc
//#pragma link C++ class PndMvdContFact+;
#pragma link C++ class PndLmdDetector+;
#pragma link C++ class PndLmdGeo+;

//digi
//#pragma link C++ class PndLmdHybridHitProducer+;
#pragma link C++ class PndLmdStripHitProducer+;
#pragma link C++ class PndLmdDigiTask+;

#pragma link C++ class PndLmdHitProducerIdeal+;
#pragma link C++ class PndLmdNoiseProducer+;
#pragma link C++ class PndLmdContFact+;



#endif

