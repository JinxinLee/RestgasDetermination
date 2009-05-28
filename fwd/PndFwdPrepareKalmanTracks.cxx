// -------------------------------------------------------------------------
// -----          PndFwdPrepareKalmanTracks  source file               -----
// -----            Created 30.09.2008  by A. Wronska                  -----
// -----            Modified 03.04.2009 by R. Karabowicz               -----
// -----        based on the recotasks/demo code  by S.Neubert         -----
// -------------------------------------------------------------------------

// Panda Headers ----------------------
#include "FairRootManager.h"
#include "Track.h"
#include "TrackCand.h"
#include "PndMCTrack.h"
#include "LSLTrackRep.h"
#include "GeaneTrackRep.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "FairGeanePro.h"
#include "FairTrackParP.h"
#include "PndFwdPrepareKalmanTracks.h"
#include "PndGemMCPoint.h"
#include "PndGemTrack.h"
//#include "PndGemTrackMatch.h"
#include "PndGemHit.h"

#include "PndDchPoint.h"
#include "PndDchTrack.h"
#include "PndDchTrackMatch.h"
#include "PndDchHit.h"


// ROOT Class Headers --------
#include "TClonesArray.h"

// C/C++ Headers ----------------------
#include <algorithm>
#include <iostream>

//using namespace::std;

PndFwdPrepareKalmanTracks::PndFwdPrepareKalmanTracks()
  : FairTask("Translation of PndFwdTracks to Tracks"), fPersistence(kFALSE), fUseGeane(kFALSE), fUseMC(kFALSE), fPDG(11), fMinNofGemHits(6), fMinNofDchHits(30)
{
}


PndFwdPrepareKalmanTracks::~PndFwdPrepareKalmanTracks()
{
  if( 0!= fTrackArray){
    fTrackArray->Delete();
    delete fTrackArray;
  }
  if( 0!= fGeanePro)
    delete fGeanePro;
}

InitStatus
PndFwdPrepareKalmanTracks::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndFwdPrepareKalmanTracks::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  fGemHitArray = (TClonesArray*) ioman->GetObject("GEMHit");
  if(fGemHitArray==0){
    Error("PndFwdPrepareKalmanTracks::Init","gem hit array not found!");
    return kERROR;
  } 
  
  if(fUseMC){
    // open MCTruth array
    fMcArray=(TClonesArray*) ioman->GetObject("MCTrack");
    if(fMcArray==0){
      Error("PndFwdPrepareKalmanTracks::Init","mctrack-array not found!");
      return kERROR;
    } 
    // open point array
    fGemPointArray=(TClonesArray*) ioman->GetObject("GEMPoint");
    if(fGemPointArray==0){
      Error("PndFwdPrepareKalmanTracks::Init","gempoint-array not found!");
      return kERROR;
    } 

    // open point array
    fDchPointArray=(TClonesArray*) ioman->GetObject("PndDchPoint");
    if(fDchPointArray==0){
      Error("PndFwdPrepareKalmanTracks::Init","dchpoint-array not found!");
      return kERROR;
    } 

    // open input array of PndDchTrackMatches
    fDchTrackMatchArray = (TClonesArray*) ioman->GetObject("PndDchTrackMatch"); 
    if(fDchTrackMatchArray==0){
      Error("PndFwdPrepareKalmanTracks::Init","PndDchTrackMatch array not found!");
      return kERROR;
    }
  }
  
  // open input array of PndGemTracks
  fGemTrackArray = (TClonesArray*) ioman->GetObject("PndGemTrack"); 
  if(fGemTrackArray==0){
    Error("PndFwdPrepareKalmanTracks::Init","PndGemTrack array not found!");
    return kERROR;
  }

  // open input array of PndGemTracks
  fDchTrackArray = (TClonesArray*) ioman->GetObject("PndDchTrack"); 
  if(fDchTrackArray==0){
    Error("PndFwdPrepareKalmanTracks::Init","PndDchTrack array not found!");
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
PndFwdPrepareKalmanTracks::Exec(Option_t* opt)
{
  if(fVerbose>0){
    std::cout<<"\n\n-------------------------------------------------------"<<std::endl;
    std::cout<<"      PndFwdPrepareKalmanTracks::Exec                      "<<std::endl;
    std::cout<<"-------------------------------------------------------"<<std::endl;
  }
  if(fTrackArray==0) Fatal("PndFwdPrepareKalmanTracks::Exec)","No TrackArray");
  fTrackArray->Delete();
  
  Int_t nofDchTracks = fDchTrackArray->GetEntriesFast();
  Int_t nofGemTracks = fGemTrackArray->GetEntriesFast();

  // loop over tracks
  std::cout << "Looping over " << nofGemTracks << " gem tracks" << endl;
  for (Int_t igemtr=0; igemtr<nofGemTracks; igemtr++){
    if(fVerbose>0)
      std::cout<<"PndFwdPrepareKalmanTracks::Exec(): Processing gem track id= "<<igemtr<<std::endl;
    PndGemTrack* gemtrack = (PndGemTrack*) fGemTrackArray->At(igemtr);
    Int_t nofGemHits = gemtrack->GetNofGemHits();
    if(fVerbose>0)
      std::cout<<"PndFwdPrepareKalmanTracks::Exec(): I found here "<<nofGemHits<<" hits \n";
    
    Int_t gemMCTrackID = (Int_t)gemtrack->GetParamLast()->GetZ()-1e6;
    std::cout << "!!!!!!!! THIS TRACK HAD MC ID " << gemMCTrackID << " !!!!!!" << std::endl;
    
    for (Int_t idchtr=0; idchtr<fDchTrackMatchArray->GetEntries(); idchtr++){
      PndDchTrackMatch* dchtrmatch = (PndDchTrackMatch*) fDchTrackMatchArray->At(idchtr);
      if ( dchtrmatch->GetMCTrackID() != gemMCTrackID ) continue;
      
      std::cout << "Found matching DCH track, recoId = " << dchtrmatch->GetRecTrackID() << "." << std::endl;

      PndDchTrack* dchtrack = (PndDchTrack*) fDchTrackArray->At(idchtr);
      Int_t nofDchCylHits = dchtrack->GetNofDchCylinderHits();

      TrackCand* cand = new TrackCand();
      for(Int_t ihit=0; ihit<nofGemHits; ihit++){
	Int_t globalHit = gemtrack->GetGemHitIndex(ihit);
	cand->addHit(2,globalHit);
      }
      for(Int_t ihit=0; ihit<nofDchCylHits; ihit++){
	Int_t globalCHitNu = dchtrack->GetDchCylinderHitIndex(ihit);
	cand->addHit(1,globalCHitNu);
      }
 
      for(Int_t ihit=0; ihit<cand->getNHits(); ihit++){
	unsigned int temp2= 1234, temp3=1234;
	cand->getHit(ihit, temp2, temp3);
	cout<<" "<<ihit<<" "<<temp2<<" "<<temp3<<endl;
      }
      
      if(cand->getNHits()<fMinNofGemHits+fMinNofDchHits)
	continue;
    
      Int_t pdg;
      Double_t q;
      TVector3 pos, mom;
    
      FairTrackParam* param = gemtrack->GetParamFirst();
      param->Position(pos);
      param->Momentum(mom);
      q = (param->GetQp()==0) ? 0 : param->GetQp()/TMath::Abs(param->GetQp());
      pdg = (Int_t)gemtrack->GetParamLast()->GetQp()-1e6;
    
      std::cout<<"positions and momenta... "<<std::endl;
      std::cout<<"position: " << std::flush;
      pos.Print();
      std::cout<<"momentum: " << std::flush;
      mom.Print();
      if(mom.Mag()>1e3){
	Error("PndFwdPrepareKalmanTracks::Exec","Track was incorrectly prefitted - abandoned in Kalman!");
	continue;
      }
      Double_t startPosAccuracy = 0.1;
      TVector3 poserr(startPosAccuracy,startPosAccuracy,startPosAccuracy);
      TVector3 startMomAccuracy(0.1,0.1,0.1);
      TVector3 momerr(mom.X()*startMomAccuracy.X(),
		      mom.Y()*startMomAccuracy.Y(),
		      mom.Z()*startMomAccuracy.Z());    
      mom.SetXYZ(mom.X()*gRandom->Gaus(1,startMomAccuracy.X()),
		 mom.Y()*gRandom->Gaus(1,startMomAccuracy.Y()),
		 mom.Z()*gRandom->Gaus(1,startMomAccuracy.Z()));
      TVector3 u(1.,0.,0.);
      TVector3 v(0.,1.,0.);

      pos.SetXYZ(pos.X()+gRandom->Gaus(0,startPosAccuracy),
		 pos.Y()+gRandom->Gaus(0,startPosAccuracy),
		 pos.Z()+gRandom->Gaus(0,startPosAccuracy));
      mom.SetMag(mom.Mag()+0.05);
      
      // create track-representation object and initialize with start values
      AbsTrackRep* rep=0;
      if(fUseGeane){
	DetPlane pl(pos,u,v);
	GeaneTrackRep *grep=new GeaneTrackRep(fGeanePro,pl,mom,poserr,momerr,q,pdg);
	grep->setPropDir(1); // propagate in flight direction
	//  if(fVerbose>0){
	std::cout<<" ^^^^^^^^^^^^^I prepare the following GeaneTrackRep:"<<std::endl;
	grep->Print();
	std::cout<<" ^^^^^^^^^^^^^End of GeaneTrackRep printout"<<std::endl;
	//}
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
  }
  if(fVerbose>0)
    std::cout<<fTrackArray->GetEntriesFast()<<" tracks created"<<std::endl;

  return;
}

ClassImp(PndFwdPrepareKalmanTracks)



