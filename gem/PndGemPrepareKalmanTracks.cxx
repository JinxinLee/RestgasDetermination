// -------------------------------------------------------------------------
// -----          PndGemPrepareKalmanTracks  source file               -----
// -----            Created 30.09.2008  by A. Wronska                  -----
// -----            Modified 03.04.2009 by R. Karabowicz               -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "FairRootManager.h"
#include "PndDetectorList.h"
#include "Track.h"
#include "TrackCand.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "PndGemPrepareKalmanTracks.h"
#include "PndGemMCPoint.h"
#include "PndGemTrack.h"
//#include "PndGemTrackMatch.h"
#include "PndGemHit.h"

// ROOT Class Headers --------
#include "TClonesArray.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>



PndGemPrepareKalmanTracks::PndGemPrepareKalmanTracks()
  : FairTask("Translation of PndGemTracks to Tracks"), fPersistence(kFALSE), fUseGeane(kFALSE), fUseMC(kFALSE), fPDG(11), fMinNofHits(6)
{
}


PndGemPrepareKalmanTracks::~PndGemPrepareKalmanTracks()
{
  if( 0!= fTrackArray){
    fTrackArray->Delete();
    delete fTrackArray;
  }
  if( 0!= fGeanePro)
    delete fGeanePro;
}

InitStatus
PndGemPrepareKalmanTracks::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndGemPrepareKalmanTracks::Init","RootManager not instantiated!");
      return kERROR;
    }
 
  AddHitBranch(1,"GEMHit");

  // open hit arrays
  std::map<unsigned int,TString>::iterator iter=fHitBranchNameMap.begin();
  while(iter!=fHitBranchNameMap.end()){
    TClonesArray* ar=(TClonesArray*) ioman->GetObject(iter->second);
    if(ar==0){
      Error("DemoPRTask::Init","hit-array %s not found!",iter->second.Data());
    }
    else{ 
      fHitBranchMap[iter->first] = ar;
    }
    ++iter;
  }//end loops over hit types
  fHitArray = fHitBranchMap[1];
  
  if(fUseMC){
    // open MCTruth array
    fMcArray=(TClonesArray*) ioman->GetObject("MCTrack");
    if(fMcArray==0){
      Error("PndGemPrepareKalmanTracks::Init","mctrack-array not found!");
      return kERROR;
    } 
    // open point array
    fGemPointArray=(TClonesArray*) ioman->GetObject("GEMPoint");
    if(fGemPointArray==0){
      Error("PndGemPrepareKalmanTracks::Init","gempoint-array not found!");
      return kERROR;
    } 
    // open input array of PndGemTrackMatches
    /*    fGemTrackMatchArray = (TClonesArray*) ioman->GetObject("PndGemTrackMatch"); 
	  if(fGemTrackMatchArray==0){
	  Error("PndGemPrepareKalmanTracks::Init","PndGemTrackMatch array not found!");
	  return kERROR;
	  }*/
  }
  
  // open input array of PndGemTracks
  fGemTrackArray = (TClonesArray*) ioman->GetObject("PndGemTrack"); 
  if(fGemTrackArray==0){
    Error("PndGemPrepareKalmanTracks::Init","PndGemTrack array not found!");
    return kERROR;
  }
  // create and register output array
  fTrackArray = new TClonesArray("Track"); 
  ioman->Register("FSTracks","GenFit",fTrackArray,fPersistence);
  
  // GeanePro will get Geometry and BField from the Run
  fGeanePro=new FairGeanePro();
  
  return kSUCCESS;
}


void
PndGemPrepareKalmanTracks::Exec(Option_t* opt)
{
  if(fVerbose>0){
    std::cout<<"\n\n-------------------------------------------------------"<<std::endl;
    std::cout<<"      PndGemPrepareKalmanTracks::Exec                      "<<std::endl;
    std::cout<<"-------------------------------------------------------"<<std::endl;
  }
  if(fTrackArray==0) Fatal("PndGemPrepareKalmanTracks::Exec)","No TrackArray");
  fTrackArray->Delete();
  
  Int_t nofGemTracks = fGemTrackArray->GetEntriesFast();
  // loop over tracks
  for (Int_t id=0; id<nofGemTracks; id++){
    if(fVerbose>0)
      std::cout<<"PndGemPrepareKalmanTracks::Exec(): Processing track id= "<<id<<std::endl;
    PndGemTrack* gemtrack = (PndGemTrack*) fGemTrackArray->At(id);
    Int_t nofHits = gemtrack->GetNofGemHits();
    if(fVerbose>0)
      std::cout<<"PndGemPrepareKalmanTracks::Exec(): I found here "<<nofHits<<" hits \n";
    
    TrackCand* cand = new TrackCand();
    for(Int_t ihit=0; ihit<nofHits; ihit++){
      Int_t globalHit = gemtrack->GetGemHitIndex(ihit);
      cand->addHit(kGEM,globalHit);
    }
    for(Int_t ihit=0; ihit<nofHits; ihit++){
      unsigned int temp2= 1234, temp3=1234;
      cand->getHit(ihit, temp2, temp3);
      cout<<" "<<ihit<<" "<<temp2<<" "<<temp3<<endl;
    }
    
    if(cand->getNHits()<fMinNofHits)
      continue;
    
    Int_t pdg = 211;
    Double_t q = 1.;
    TVector3 pos, mom;
    
    FairTrackParam* param = gemtrack->GetParamFirst();
    param->Position(pos);
    param->Momentum(mom);

    if ( fUseMC ) { 
      q = (param->GetQp()==0) ? 0 : param->GetQp()/TMath::Abs(param->GetQp());
      pdg = (Int_t)(gemtrack->GetParamLast()->GetQp()-1e6);
    }

    std::cout<<"pozycje i pedy "<<std::endl;
    pos.Print();
    mom.Print();
    cout << " ladunek " << q << " i pdg = " << pdg << endl;
    if(mom.Mag()>1e3){
      Error("PndGemPrepareKalmanTracks::Exec","Track was incorrectly prefitted - abandoned in Kalman!");
      continue;
    }
    Double_t startPosAccuracy = 0.5;
    TVector3 poserr(startPosAccuracy,startPosAccuracy,3.*startPosAccuracy);
    TVector3 startMomAccuracy(0.1,0.1,0.1);
    TVector3 momerr(mom.X()*startMomAccuracy.X(),
		    mom.Y()*startMomAccuracy.Y(),
		    mom.Z()*startMomAccuracy.Z());    
    TVector3 u(1.,0.,0.);
    TVector3 v(0.,1.,0.);
    mom.SetMag(mom.Mag()+0.1);    

    // create track-representation object and initialize with start values
    AbsTrackRep* rep=0;
    if(fUseGeane){
      DetPlane pl(pos,u,v);
      GeaneTrackRep *grep=new GeaneTrackRep(fGeanePro,pl,mom,poserr,momerr,q,pdg);
      grep->setPropDir(1); // propagate in flight direction
      if(fVerbose>0){
	std::cout<<" ^^^^^^^^^^^^^I prepare the following GeaneTrackRep:"<<std::endl;
	grep->Print();
	std::cout<<" ^^^^^^^^^^^^^End of GeaneTrackRep printout"<<std::endl;
      }
      rep = grep;
    }
    else {
      TVector3 dir=mom.Unit();
      double dxdz=dir.X()/dir.Z();
      double dydz=dir.Y()/dir.Z();
      double qp=q/mom.Mag();
      rep=new LSLTrackRep(pos.Z(),pos.X(),pos.Y(),dxdz,dydz,qp,
			  poserr.X(),poserr.Y(),0.1,0.1,0.1,NULL);
      if(fVerbose>0){
	std::cout<<" ^^^^^^^^^^^^^I prepare the following LSLTrackRep:"<<std::endl;
	rep->Print();
	std::cout<<" ^^^^^^^^^^^^^End of LSLTrackRep printout"<<std::endl;
      }
    }
    // create track object
    Track* trk=new((*fTrackArray)[fTrackArray->GetEntriesFast()]) Track(rep);
    trk->setCandidate(*cand);
    cout << "!!!!candidate " <<fTrackArray->GetEntriesFast() <<  " has " << cand->getNHits() << " hits" << endl;
  }// end loop over gemtracks
  if(fVerbose>0)
    std::cout<<fTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;
  return;
}

ClassImp(PndGemPrepareKalmanTracks)
