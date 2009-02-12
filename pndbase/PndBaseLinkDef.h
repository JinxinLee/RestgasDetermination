// $Id: ZdcLinkDef.h,v 1.1.1.1 
//2005/06/23 07:14:26 dbertini Exp $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class PndStack+;
#pragma link C++ class PndMCTrack+;

#pragma link C++ class PndDskCerenkov+;

#pragma link C++ class PndDrcPDPoint+; 
#pragma link C++ class PndDrcBarPoint+;
#pragma link C++ class PndDrcHit+;

#pragma link C++ class PndGemHit+;
#pragma link C++ class PndGemMCPoint+;

#pragma link C++ class  PndMdtTrk+;
#pragma link C++ class  PndMdtPoint+;
#pragma link C++ class  PndMdtHit+;

#pragma link C++ class  PndSttGeomPoint+;
#pragma link C++ class  PndSttHelixHit+;  
#pragma link C++ class  PndSttHit+;  
#pragma link C++ class  PndSttHitInfo+;  
#pragma link C++ class  PndSttPoint+;  
#pragma link C++ class  PndSttTrack+;


#endif

