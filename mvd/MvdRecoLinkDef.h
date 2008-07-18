// $Id: MvdLinkDef.h,v 1.0 Ralf Kliemt 08.Jan.2007 $

#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;


//data
#pragma link C++ class PndMvdRecoHit+;
#pragma link C++ class PndMvdPidCand+;
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

//pid
#pragma link C++ class PndMvdPidIdealTask+;
#pragma link C++ class PndMvdIdealPidAlgo+;
#pragma link C++ class PndMvdSimplePidAlgo+;
#pragma link C++ class PndMvdAdvancedPidAlgo+;

//tracking
// #pragma link C++ class PndMvdIdealTrackingTask+;
#pragma link C++ class PndMvdIdealTrackFinderTask+;
#pragma link C++ class PndMvdKalmanTask+;
//#pragma link C++ class PndMvdRiemannTrackFinderTask+;
//#pragma link C++ class PndMvdTrackFinderAnaTask+;


#endif

