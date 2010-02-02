// $Id: MvdLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


//data

//reco
#pragma link C++ class PndSdsIdealRecoTask+;
//#pragma link C++ class PndSdsRecoTask+;
#pragma link C++ class PndSdsPixelClusterTask+;
#pragma link C++ class PndSdsPixelClusterFinder+;
#pragma link C++ class PndSdsSimplePixelClusterFinder+;
//#pragma link C++ class PndSdsClusterTask+;
#pragma link C++ class PndSdsIdealClusterTask+;
#pragma link C++ class PndSdsStripClusterTask+;
#pragma link C++ class PndSdsStripClusterBuilder+;
#pragma link C++ class PndSdsSimpleStripClusterFinder+;
#pragma link C++ class PndSdsStripClusterFinder+;
#pragma link C++ class PndSdsChargeWeightingAlgorithms+;
//tools
//#pragma link C++ class PndMvdRadDamList+;
//#pragma link C++ class PndMvdRadDamTask+;
//#pragma link C++ class PndMvdAnaRadDam+;



#endif

