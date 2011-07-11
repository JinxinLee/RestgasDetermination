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

ClassImp(PndTpcTrackInitTask)

PndTpcTrackInitTask::PndTpcTrackInitTask()
  : FairTask("PndTpc Pattern Reco"), 
    _persistence(kFALSE),
    _interactionZ(0.),
    _proxcut(2),
    _helixcut(0.4),
    _minpoints(5),
    _TTproxcut(2.),
    _TTdipcut(.01),
    _TThelixcut(.5),
    _TTplanecut(0.001),
    _riemannscale(24.6),
    _clusterBranchName("PndTpcCluster"),
    _smoothing(false),
    _geane(false),
    counter(0),
    Bz(0)
  {
    fVerbose = 0;
  }

PndTpcTrackInitTask::~PndTpcTrackInitTask(){}

void 
PndTpcTrackInitTask::SetSortingParameters(
                 bool sortingMode,
                 int sorting,
                 double interactionZ){
  _sortingMode=sortingMode;
  _sorting=sorting;
  _interactionZ=interactionZ;
}

void
PndTpcTrackInitTask::SetTrkFinderParameters(
                 double proxcut,
					       double helixcut,
					       unsigned int minpointsforfit){
  _proxcut=proxcut;
  _helixcut=helixcut;
  _minpoints=minpointsforfit;
}

void
PndTpcTrackInitTask::SetTrkMergerParameters(
					       double TTproxcut,
					       double TTdipcut,
					       double TThelixcut,
					       double TTplanecut){
  _TTproxcut=TTproxcut;
  _TTdipcut=TTdipcut;
  _TThelixcut=TThelixcut;
  _TTplanecut=TTplanecut;
}

InitStatus
PndTpcTrackInitTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcTrackInitTask::Init","RootManager not instantiated!");
      return kERROR;
    }

    
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  if(_clusterArray==0)
    {
      Error("PndTpcTrackInitTask::Init","Cluster-array not found!");
      return kERROR;
    }

 _riemannTrackArray=(TClonesArray*) ioman->GetObject(_riemannBranchName);
  if(_clusterArray==0)
    {
      Error("PndTpcTrackInitTask::Init","Cluster-array not found!");
      return kERROR;
    }

  // create and register output array
  _trackArray = new TClonesArray("GFTrack");
  ioman->Register(_trackBranchName,"GenFit",_trackArray,true);

   _trackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register(_trackCandBranchName,"Tpc",_trackCandArray,_persistence);

  _pndTrackArray = new TClonesArray("PndTrack");
  ioman->Register(_pndTrackBranchName,"Tpc",_pndTrackArray,_persistence);


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
    std::cerr<<"PndTpcTrackInitTask: "<<"No const field! Curvature seeding not valid... Setting Bz="<<Bz<<std::endl;
  }
  else if(CField) {
    Bz=field->GetBz(0.,0.,0.);
    std::cerr<<"PndTpcTrackInitTask: "<<"const field! Setting Bz="<<Bz<<std::endl;
  }
  else{
    Bz=20.;
    std::cerr<<"PndTpcTrackInitTask: "<<"default setting Bz="<<Bz<<std::endl;
  }

  //init gPro
  if(_geane){
    gPro = new FairGeanePro();
  }

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
  std::cout<<"PndTpcTrackInitTask::Exec; Event Number: "<<counter++<<std::endl;
  
  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcSimpleTrackInit::Exec)","No TrackArray");
  _trackArray->Delete();
  
  if(_pndTrackArray==0) Fatal("PndTpcSimpleTrackInit::Exec)","No PndTrackArray");
  _pndTrackArray->Delete();
  if(_trackCandArray==0) Fatal("PndTpcSimpleTrackInit::Exec)","No TrackCandArray");
  _trackCandArray->Delete();
  

  
  // fill riemannlist
  std::vector<PndTpcRiemannTrack*> friemannlist;
  unsigned int nr=_riemannTrackArray->GetEntries();
  for(unsigned int ir=0;ir<nr;++ir){
    friemannlist.push_back((PndTpcRiemannTrack*)_riemannTrackArray->At(ir));
  }

  // build GFTrackCands
  std::vector<GFTrackCand*> candlist;

  

  //int minhits = 10; // minimum hits needed to build pndtrackcands and GFTrackCands
  // if(minhits<_minpoints) minhits=_minpoints;
  double pbackup = 2.;  // momentum value that is set when other initialisations fail

  // loop over Riemann tracks
  std::cout<< "Looping over "<<nr<<" riemann tracks to write out" << std::endl;

  for(unsigned int itrk=0; itrk<nr; ++itrk){
    PndTpcRiemannTrack* trk=friemannlist[itrk];
    int nhits=trk->getNumHits();
    
    if (fVerbose) std::cout<<"Tracklet "<<itrk<<"   nhits = "<<nhits;

    // check if track too steep
    double trackSinDip = trk->sinDip();
    if (TMath::Abs(trackSinDip)<0.01) {
      if (fVerbose) std::cout<<" - skipping, sin(dip) too small: "<<trackSinDip<<std::endl;
      continue;
    }
    // ceck if momentum high enough
    double p = trk->getMom(Bz);
    if (Bz==0) p=pbackup;
    if(fabs(p)<1E-1) {
      if (fVerbose) std::cout<<" - skipping, momentum too small: "<<p*1E3<<" MeV"<<std::endl;
      continue;
    }

    // check pdg
    int winding = trk->winding(); // we look in z direction!
    int pdg = winding * 211; // Todo: pions hardcoded atm
    if(Bz<0) pdg *= -1;

    // if(_mcPid){ // monte carlo PID
//       unsigned int trackId = trk->mcid().DominantID().mctrackID();
//       int MCpdg = ((PndMCTrack*)(_mcTrackArray->At(trackId)))->GetPdgCode();

//       double pdgCharge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
//       double MCpdgCharge = TDatabasePDG::Instance()->GetParticle(MCpdg)->Charge();

//       if (pdgCharge*MCpdgCharge > -0.01) pdg = MCpdg; // also neutral particles may occur
//       else pdg = -1.*MCpdg;

//       // photon
//       if(pdg == -22) pdg = 22;

//       TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(pdg);
//       if(part == 0){
//         if (fVerbose) std::cout << " - skipping, unknown PDG id: " << pdg;
//         continue;
//       }
//     }

    if (fVerbose) std::cout<<std::endl;


    // store pndtracks and pndcands in output array
    PndTrackCand* pndcand=new((*_trackCandArray)[_trackCandArray->GetEntriesFast()]) PndTrackCand();
    PndTrack* pndtrack=new((*_pndTrackArray)[_pndTrackArray->GetEntriesFast()]) PndTrack();
    pndtrack->SetTrackCand(*pndcand);

    // create GFTrackCands
    GFTrackCand* cand=new GFTrackCand();

    // fill hits into GFTrackCands and pndcands from small to big Radius
    bool invertedTrack = false;
    double r1=trk->getHit(0)->cluster()->pos().Perp();
    double r2=trk->getHit(nhits-1)->cluster()->pos().Perp();
    if(r1<=r2){
      for(unsigned int ih=0; ih<nhits; ++ih){
        cand->addHit(FairRootManager::Instance()->GetBranchId("PndTpcCluster"),trk->getHit(ih)->cluster()->index());
        //pndcand->AddHit(FairRootManager::Instance()->GetBranchId("PndTpcCluster"),trk->getHit(ih)->cluster()->index(),trk->getHit(ih)->cluster()->pos().Mag()); // todo: fix issues
      }
    }
    else {
      for(unsigned int ih=nhits; ih>0; --ih){
        cand->addHit(FairRootManager::Instance()->GetBranchId("PndTpcCluster"),trk->getHit(ih-1)->cluster()->index());
        //pndcand->AddHit(FairRootManager::Instance()->GetBranchId("PndTpcCluster"),trk->getHit(ih-1)->cluster()->index(),trk->getHit(ih-1)->cluster()->pos().Mag());// todo: fix issues
      }
      invertedTrack = true;
    }// finished filling hits


    // get seed values

    TVector3 pos1, direction;

    // the start direction has to point opposite to the actual direction, I don't know why, but otherwise the charge is wrong
    if(invertedTrack) {
      trk->getPosDirOnHelix(trk->getNumHits()-1, pos1, direction);
    }
    else {
      trk->getPosDirOnHelix(0, pos1, direction);
      direction *= -1.;
      winding*=-1.;
    }

    TVector3 poserr(0.3,0.3,0.3);

    TVector3 mom = p * direction;
    TVector3 momerr(fabs(mom.X()),fabs(mom.Y()),fabs(mom.Z()));
    momerr *= 1./TMath::Sqrt(nhits);


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
      std::cout<<"\n  invertedTrack: "<<invertedTrack;
      std::cout<<"\n  pdg id: "<<pdg<<std::endl;
    }

    // set seed values to cands
    pndcand->setTrackSeed(pos1,direction,1./p);

    cand->setCurv(1./trackR); //  actually this is never used
    cand->setDip(trk->dip());

    //RK TRACKREP
    RKTrackRep* rkrep = new RKTrackRep(pos1, mom, poserr, momerr,pdg);
    

    candlist.push_back(cand);

    // check Monte Carlo Truth
    McIdCollection mcid;
    mcid.ClearData();
    for(unsigned int ic=0;ic<cand->getNHits();++ic){
      unsigned int detId;
      unsigned int hitId;
      cand->getHit(ic,detId,hitId);
      mcid.AddIDCollection(((PndTpcCluster*)_clusterArray->At(hitId))->mcId());
    }
  

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

      // charge (for geane)
      TParticlePDG * part = TDatabasePDG::Instance()->GetParticle(pdg);
      int q = int(part->Charge()/(3.));

      GeaneTrackRep* grep = new GeaneTrackRep(gPro,pl,mom,poserr,momerr,q,pdg);
      // add rep and set as cardinal rep
      gftrk->addTrackRep(grep);
      gftrk->setCardinalRep(gftrk->getNumReps()-1);
    }
    
    //SMOOTHING
    if(_smoothing)
      gftrk->setSmoothing(true);
    
  }// end loop over tracks
  

  std::cout<<"PndTpcTrackInitTask::Exec:: "
           <<candlist.size()<<" tracks setup."<<std::endl;

 
}

void
  PndTpcTrackInitTask::SetStoreHistograms(TString file) {
  std::cerr<<"PndTpcTrackInitTask::SetStoreHistograms() - empty implementation"<<std::endl;
}

void
  PndTpcTrackInitTask::WriteHistograms(const TString& filename) {
  std::cerr<< "PndTpcTrackInitTask::WriteHistograms"<<std::endl;

  TFile* file=FairRootManager::Instance()->GetOutFile();
  file->mkdir("TrackInit");
  file->cd("TrackInit");
}
