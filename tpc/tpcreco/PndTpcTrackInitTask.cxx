//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcTrackInitTask
//      see PndTpcTrackInitTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//      Johannes Rauch
//      Felix Boehmer
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcTrackInitTask.h"

// C/C++ Headers ----------------------
#include <map>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannHit.h"

#include "PndTpcDigiPar.h"

#include "GFTrackCand.h"
#include "GFTrack.h"
#include "RKTrackRep.h"
#include "GeaneTrackRep.h"
#include "FairGeanePro.h"
#include "McIdCollection.h"
#include "TVector3.h"
#include "FairMCPoint.h"
#include "FairRunAna.h"
#include "GFDetPlane.h"
#include "TDatabasePDG.h"
#include "FairField.h"
#include "PndConstField.h"
#include "PndMultiField.h"
#include "PndFieldAdaptor.h"
#include "GFFieldManager.h"
#include "PndTrackCand.h"
#include "PndTrack.h"
#include "PndMCTrack.h"

#include "TFile.h"
#include "TH1I.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TVector3.h"
#include "TMath.h"
#include"TDatabasePDG.h"

#include "PndDetectorList.h"

#include <cmath>

using namespace std;

// Class Member definitions -----------
#define PDGDEFAULT 211

ClassImp(PndTpcTrackInitTask)

PndTpcTrackInitTask::PndTpcTrackInitTask()
  : FairTask("PndTpc Pattern Reco"), 
    _persistence(kFALSE),
    _persistencePnd(kFALSE),

    _clusterBranchName("PndTpcCluster"),
    _riemannBranchName("RiemannTrack"),
    _trackBranchName("TrackPreFit"),
    _trackCandBranchName("PndTrackCand"),
    _pndTrackBranchName("PndTrack"),

    _smoothing(true),
    _geane(false),

    _mcPid(false),
    _pdg(PDGDEFAULT),

    counter(0),
    Bz(0)
  {
    fVerbose = 0;
  }

PndTpcTrackInitTask::~PndTpcTrackInitTask(){}


InitStatus
PndTpcTrackInitTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  if(ioman==0){
    Error("PndTpcTrackInitTask::Init","RootManager not instantiated!");
    return kERROR;
  }

  // get input arrays
  _mcTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mcTrackArray==0){
    Error("PndTpcTrackInitTask::Init","MCTrack-array not found! Cannot use ideal PID");
    _mcPid=false;
  }
    
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  if(_clusterArray==0){
    Error("PndTpcTrackInitTask::Init","Cluster-array not found!");
    return kERROR;
  }

 _riemannTrackArray=(TClonesArray*) ioman->GetObject(_riemannBranchName);
  if(_riemannTrackArray==0){
    Error("PndTpcTrackInitTask::Init","RiemannTrack-array not found!");
    return kERROR;
  }

  // create and register output array
  _trackArray = new TClonesArray("GFTrack");
  ioman->Register(_trackBranchName,"GenFit",_trackArray,_persistence);

   _trackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register(_trackCandBranchName,"Tpc",_trackCandArray,_persistencePnd);

  _pndTrackArray = new TClonesArray("PndTrack");
  ioman->Register(_pndTrackBranchName,"Tpc",_pndTrackArray,_persistencePnd);


  //get the magnetic field for curvature seeding
  FairField* field=FairRunAna::Instance()->GetField();
  bool CField = dynamic_cast<PndConstField*>(field);
  bool MField = dynamic_cast<PndMultiField*>(field);
  GFFieldManager::getInstance()->init(new PndFieldAdaptor(field));
  if(MField) {
    Double_t O[3], B[3];
    O[0]=0; O[1]=0; O[2]=0;
    field->GetFieldValue(O,B);
    Bz=B[2];
    std::cerr<<"PndTpcTrackInitTask: "<<"No const field! Curvature seeding not valid... Setting Bz = "<<Bz<<std::endl;
  }
  else if(CField) {
    Bz=field->GetBz(0.,0.,0.);
    std::cerr<<"PndTpcTrackInitTask: "<<"const field! Setting Bz = "<<Bz<<std::endl;
  }
  else{
    Bz=20.;
    std::cerr<<"PndTpcTrackInitTask: "<<"default setting Bz = "<<Bz<<std::endl;
  }

  //init FairGeanePro
  if(_geane) gPro = new FairGeanePro();

  return kSUCCESS;
}


void
PndTpcTrackInitTask::SetParContainers() {

  std::cout<<"PndTpcClusterFinderTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



void
PndTpcTrackInitTask::Exec(Option_t* opt)
{
  std::cout << "PndTpcTrackInitTask::Exec; Event Number: " << counter++ << std::endl;
  
  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcTrackInitTask::Exec)","No TrackArray");
  _trackArray->Delete();
  
  if(_trackCandArray==0) Fatal("PndTpcTrackInitTask::Exec)","No PndTrackCandArray");
  _trackCandArray->Delete();

  if(_pndTrackArray==0) Fatal("PndTpcTrackInitTask::Exec)","No PndTrackArray");
  _pndTrackArray->Delete();
  


  // build GFTrackCands
  std::vector<GFTrackCand*> candlist;

  double pbackup = 2.;  // momentum value that is set when other initialisations fail


  // loop over Riemann tracks
  unsigned int nr = _riemannTrackArray->GetEntriesFast();
  std::cout << "Looping over " << nr << " riemann tracks" << std::endl;

  for(unsigned int itrk=0; itrk<nr; ++itrk){
    PndTpcRiemannTrack* trk = (PndTpcRiemannTrack*)_riemannTrackArray->At(itrk);
    
    int nhits=trk->getNumHits();

    if (fVerbose) std::cout<<"Tracklet "<<itrk;

    // check if track is pre-fitted
    if (! trk->isFitted()) {
      if (fVerbose) std::cout << " - skipping, track not prefitted" << std::endl;
      continue;
    }
    // check if track too steep
    double trackSinDip = trk->sinDip();
    if (TMath::Abs(trackSinDip)<0.01) {
      if (fVerbose) std::cout << " - skipping, sin(dip) too small: " << trackSinDip << std::endl;
      continue;
    }
    // ceck if momentum high enough
    double p = trk->getMom(Bz);
    if (Bz==0) p=pbackup;
    if(p<1.E-2) {  // 10 MeV ~ 3cm helix diameter
      if (fVerbose) std::cout << " - skipping, momentum too small: " << p*1E3 << " MeV" << std::endl;
      continue;
    }
    
    unsigned int trackId(trk->mcid().DominantID().mctrackID());
    int eventId(trk->mcid().DominantID().mceventID());
    
    if(eventId!=0)trackId+=10000;

    // check pdg
    int pdg(_pdg);
    if(_mcPid) pdg=((PndMCTrack*)(_mcTrackArray->At(trackId)))->GetPdgCode();
    
    double pdgCharge(TDatabasePDG::Instance()->GetParticle(pdg)->Charge()/3.);

    int winding(trk->winding()); // we look in z direction!


   if (pdgCharge < 0) {
      pdg *= -1;
      pdgCharge *= -1.;
    }
    if (winding > 0) {
      pdg *= -1;
      pdgCharge *= -1.;
    }
    if (Bz < 0) {
      pdg *= -1;
      pdgCharge *= -1.;
    }


  // check sorting
    bool invertTrack(true);

    TVector3 ps1=trk->getFirstHit()->cluster()->pos();
    TVector3 ps2=trk->getLastHit()->cluster()->pos();

    if(ps1.Z() < ps2.Z()-7) invertTrack = false;
    else if(ps1.Z() > ps2.Z()+7) invertTrack = true;
    else if (ps1.Perp()>ps2.Perp()+5) invertTrack = true;
    else if (ps1.Perp()<ps2.Perp()) invertTrack = false;
    else invertTrack = true;

    if (invertTrack){
      winding*=-1;
      pdg *= -1;
      pdgCharge *= -1;
    }


    TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(pdg);
    if(part == 0){
      if (fVerbose) std::cout << " - skipping, unknown PDG id: " << pdg;
      continue;
    }

    if (fVerbose) std::cout<<std::endl;

  // create GFTrackCands
    GFTrackCand* cand=new GFTrackCand();

    // fill hits into GFTrackCands and pndcands  and get seed values
    TVector3 pos1, direction;

    if(!invertTrack){
      for(unsigned int ih=0; ih<nhits; ++ih){
        cand->addHit(FairRootManager::Instance()->GetBranchId("PndTpcCluster"),trk->getHit(ih)->cluster()->index());
      }
      trk->getPosDirOnHelix(0, pos1, direction);
    }
    else { // invert track
      for(unsigned int ih=nhits; ih>0; --ih){
        cand->addHit(FairRootManager::Instance()->GetBranchId("PndTpcCluster"),trk->getHit(ih-1)->cluster()->index());
      }
      trk->getPosDirOnHelix(trk->getNumHits()-1, pos1, direction);
      direction *= -1.;
    }// finished filling hits



    TVector3 poserr(1,1,1);
    poserr*=trk->resolution();

    TVector3 mom(p * direction);
    TVector3 momerr(fabs(mom.X()),fabs(mom.Y()),fabs(mom.Z()));
    momerr *= trk->resolution()*5.;

    double trackR = trk->r();

    if (fVerbose) {
      double trackDip = trk->dip();
      std::cout<<" center of track "; trk->center().Print();
      std::cout<<" Radius of track [cm]: " << trackR;
      std::cout<<"\n Dip of track [deg]:   " << trackDip/TMath::Pi()*180;
      std::cout<<"\n seed values: ";
      std::cout<<"\n  start position: "; pos1.Print();
      std::cout<<"  momentum [GeV]: "<<p;
      std::cout<<"\n  p_perp [GeV]:   " << trackR*0.0003*TMath::Abs(Bz);
      std::cout<<"\n  direction: "; direction.Print();
      std::cout<<"  winding: "<<winding;
      std::cout<<"\n  invertTrack: "<<invertTrack;
      std::cout<<"\n  pdg id: "<<pdg<<std::endl;
    }

    // set seed values to cands
    cand->setCurv(1./trackR);
    cand->setDip(trk->dip());
    cand->setComplTrackSeed(pos1, mom, pdg, poserr, momerr*(1./p));
    cand->setMcTrackId(trackId);

    candlist.push_back(cand);


    //RK TRACKREP
    RKTrackRep* rkrep = new RKTrackRep(pos1, mom, poserr, momerr, pdg);

    // store GFTracks in output array
    GFTrack* gftrk=new((*_trackArray)[_trackArray->GetEntriesFast()]) GFTrack(rkrep);
    gftrk->setCandidate(*cand); // here the candidate is copied!

    //GEANE TACKREP
    if(_geane) {
      TVector3 u=mom.Orthogonal();
      u.SetMag(1.);
      TVector3 v=mom.Cross(u);
      v.SetMag(1.);
      GFDetPlane pl(pos1,u,v);

      GeaneTrackRep* grep = new GeaneTrackRep(gPro,pl,mom,poserr,momerr,pdgCharge,pdg);
      // add rep //and set as cardinal rep
      gftrk->addTrackRep(grep);
      //gftrk->setCardinalRep(gftrk->getNumReps()-1);
    }
    
    //SMOOTHING
    if(_smoothing) gftrk->setSmoothing(true);
    
  }// end loop over tracks
  

  std::cout<<"PndTpcTrackInitTask::Exec:: "
           <<candlist.size()<<" tracks setup."<<std::endl;
 
}


void
  PndTpcTrackInitTask::WriteHistograms(const TString& filename) {
  std::cerr<< "PndTpcTrackInitTask::WriteHistograms"<<std::endl;

  TFile* file=FairRootManager::Instance()->GetOutFile();
  file->mkdir("TrackInit");
  file->cd("TrackInit");
}
