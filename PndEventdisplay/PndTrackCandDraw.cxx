// -------------------------------------------------------------------------
// -----                        PndTrackCandDraw source file                  -----
// -----                  Created 03/01/08  by M. Al-Turany            -----
// -------------------------------------------------------------------------

#include "PndTrackCandDraw.h"
#include "FairHit.h"
//#include "TClonesArray.h"
#include "TObjArray.h"
#include "TEveManager.h"
#include "TEveBoxSet.h"
#include "TrackCand.h"
#include "FairRootManager.h"
#include "PndTpcCluster.h"
#include "PndLhePidTrack.h"
//#include "PndTpcLheTrack.h"

#include "PndDetectorList.h"
#include <iostream>
using std::cout;
using std::endl;

// -----   Standard constructor   ------------------------------------------
PndTrackCandDraw::PndTrackCandDraw(const char* TrackCandName, Int_t iVerbose):FairHitDraw(TrackCandName, iVerbose)
{
}
// -------------------------------------------------------------------------
InitStatus PndTrackCandDraw::Init()
{
	FairBoxSetDraw::Init();

   if(fVerbose>1)  cout<<  "PndTrackCandDraw::Init()" << endl;
   FairRootManager* fManager = FairRootManager::Instance();
   fPixPointList = (TClonesArray *)fManager->GetObject("MVDHitsPixel");
   fStripPointList = (TClonesArray *)fManager->GetObject("MVDHitsStrip");
   fTpcClusterList = (TClonesArray *)fManager->GetObject("PndTpcCluster");
   fSttHelixList   = (TClonesArray *)fManager->GetObject("SttHelixHit");
   if (fTpcClusterList == 0)
     fTpcClusterList = (TClonesArray*)fManager->GetObject("PndTpcClusterMerged");
   if(fPixPointList==0){
      cout << "PndTrackCandDraw::Init()  branch MVDHitsPixel Not found! Task will be deactivated "<< endl;
      //SetActive(kFALSE);
    }
   if(fStripPointList==0){
      cout << "PndTrackCandDraw::Init()  branch MVDHitsStrip Not found! Task will be deactivated "<< endl;
      //SetActive(kFALSE);
    }
   if(fTpcClusterList==0){
      cout << "PndTrackCandDraw::Init()  branch PndTpcCluster or PndTpcClusterMerged Not found! Task will be deactivated "<< endl;
      //SetActive(kFALSE);
    }
   if(fSttHelixList==0){
      cout << "PndTrackCandDraw::Init()  branch SttHelixList Not found! Task will be deactivated "<< endl;
      //SetActive(kFALSE);
   }

   if(fVerbose>2){
     cout<<  "PndTrackCandDraw::Init() get pix points list" <<  fPixPointList<< endl;
     cout<<  "PndTrackCandDraw::Init() get strip points list" <<  fStripPointList<< endl;
     cout<<  "PndTrackCandDraw::Init() get tpc cluster list" << fTpcClusterList<<endl;
     cout<<  "PndTrackCandDraw::Init() get stt helix list" << fSttHelixList<<endl;

   }
   fq=0;
}
// -------------------------------------------------------------------------


void PndTrackCandDraw::AddBoxes(TEveBoxSet* set, TObject* obj, Int_t i)
{
	   TrackCand *tc;
	   PndLhePidTrack *pidtc;
//	   PndTpcLheTrack *lhetc;
//	   PndHit *p;
	//   Reset();
	 //  cout<<  "PndTrackCandDraw::Exec() " << fPointList->GetEntriesFast() << endl;
	      if( 0 == strcmp(obj->ClassName() , "TrackCand") ) {
	        tc=(TrackCand *)obj;
	        std::cout<<"fTrackCandList is full of TrackCands"<<std::endl;
	      } else if ( 0 == strcmp(obj->ClassName() , "PndLhePidTrack") ) {
	        pidtc=(PndLhePidTrack *)fTrackCandList->At(i);
	        tc=pidtc->GetTrackCand();
	        std::cout<<"fTrackCandList is full of PndLhePidTracks"<<std::endl;
	      } else if ( 0 == strcmp(obj->ClassName(), "PndTpcLheTrack")){
	    	  std::cout<<"fTrackCandList is full of PndTpcLheTracks"<<std::endl;
//	    	  lhetc = (PndTpcLheTrack*)obj;
//	    	  tc = lhetc->GetTrackCand();
	      }
	      else {
	        std::cout<<"fTrackCandList is full of UNKNOWN "<<obj->ClassName()<<"?"<<std::endl;
	      }
	      if(0==tc) return;


	      for (Int_t j = 0; j < tc->getNHits(); j++){
	        unsigned int detId, hitId;
	        tc->getHit(j, detId, hitId);
	        TVector3 point = GetVector(detId, hitId);
	        set->AddBox(point.X(), point.Y(), point.Z());
	        set->DigitValue(i);
	      }
}


TVector3 PndTrackCandDraw::GetVector(Int_t detId, Int_t hitId)
{
	FairHit * p;
	//if (detId == kTpcCluster){
	if (detId == 3){
		PndTpcCluster* cluster = (PndTpcCluster*)fTpcClusterList->At(hitId);
		return (cluster->pos());
	}

	else if (detId == kMVDHitsStrip || detId == kMVDHitsPixel || detId == kSttHelixHit)
	{
		if (detId == kMVDHitsPixel)
		{
			p = (FairHit *) fPixPointList->At(hitId);
		}
		else if (detId == kMVDHitsStrip)
		{
			p = (FairHit *) fStripPointList->At(hitId);
		}
		else if (detId == kSttHelixHit){
			p = (FairHit *) fSttHelixList->At(hitId);
		}
		return (TVector3(p->GetX(), p->GetY(), p->GetZ()));
	}
	else
		std::cout
				<< "-E- PndTrackCandDraw::GetVector : Unknown Detector with ID: "
				<< detId << std::endl;
	return TVector3();
}

// -----   Destructor   ----------------------------------------------------
PndTrackCandDraw::~PndTrackCandDraw()
{
}
// -------------------------------------------------------------------------



ClassImp(PndTrackCandDraw)



