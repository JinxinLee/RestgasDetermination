// $Id: MvdLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


//data
#pragma link C++ class PndMvdCluster+;
#pragma link C++ class PndMvdHitInfo+;
#pragma link C++ class PndMvdHit+;

//reco
#pragma link C++ class PndMvdIdealRecoTask+;
#pragma link C++ class PndMvdRecoTask+;
#pragma link C++ class PndMvdPixelClusterTask+;
#pragma link C++ class PndMvdPixelClusterFinder+;
#pragma link C++ class PndMvdSimplePixelClusterFinder+;
#pragma link C++ class PndMvdClusterTask+;
#pragma link C++ class PndMvdIdealClusterTask+;
#pragma link C++ class PndMvdStripClusterTask+;
#pragma link C++ class PndMvdStripClusterBuilder+;



#endif

