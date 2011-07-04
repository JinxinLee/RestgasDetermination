//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRiemannTrackingTask
//      see PndTpcRiemannTrackingTask.hh for details
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
#include "PndTpcRiemannTrackingTask.h"

// C/C++ Headers ----------------------
#include <map>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRuntimeDb.h"
#include "TClonesArray.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannTrackFinder.h"
#include "PndTpcRiemannHit.h"

#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcHelixHTCorrelator.h"
#include "PndTpcProximityTTCorrelator.h"
#include "PndTpcDipTTCorrelator.h"
#include "PndTpcRiemannTTCorrelator.h"
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

ClassImp(PndTpcRiemannTrackingTask)

PndTpcRiemannTrackingTask::PndTpcRiemannTrackingTask()
  : FairTask("PndTpc Pattern Reco"), 
    _persistence(kFALSE),
    _riemannPersistence(kFALSE),
    _sortingMode(true), _sorting(3), _interactionZ(0.),
    _mergeTracks(true),
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
    _mcPid(true), // todo: remember to turn this off agan at some point
    counter(0),
    Bz(0)
  {
    fVerbose = 0;
  }

PndTpcRiemannTrackingTask::~PndTpcRiemannTrackingTask(){
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
}

void 
PndTpcRiemannTrackingTask::SetSortingParameters(
                 bool sortingMode,
                 int sorting,
                 double interactionZ){
  _sortingMode=sortingMode;
  _sorting=sorting;
  _interactionZ=interactionZ;
}

void
PndTpcRiemannTrackingTask::SetTrkFinderParameters(
                 double proxcut,
					       double helixcut,
					       unsigned int minpointsforfit){
  _proxcut=proxcut;
  _helixcut=helixcut;
  _minpoints=minpointsforfit;
}

void
PndTpcRiemannTrackingTask::SetTrkMergerParameters(
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
PndTpcRiemannTrackingTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _mcTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mcTrackArray==0)
    {
      Error("PndTpcdEdxTask::Init","MCTrack-array not found! Cannot use ideal PID");
      _mcPid=false;
    }
    
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  if(_clusterArray==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","Cluster-array not found!");
      return kERROR;
    }
  // Get input collection
/*
  _mvdArray=(TClonesArray*) ioman->GetObject("MVDPoint");

  if(_mvdArray==0)
    {
      Error("PndTpcRiemannTrackingTask::Init","mvd-array not found!");
    }

*/

  // create and register output array
  _trackArray = new TClonesArray("GFTrack");
  ioman->Register("TrackPreFit","GenFit",_trackArray,true);

  _riemannTrackArray = new TClonesArray("PndTpcRiemannTrack");
  ioman->Register("RiemannTrack","Tpc",_riemannTrackArray,_riemannPersistence);

  _riemannHitArray = new TClonesArray("PndTpcRiemannHit");
  ioman->Register("RiemannHit","Tpc",_riemannHitArray,_riemannPersistence);
    
  
  _trackCandArray = new TClonesArray("PndTrackCand");
  ioman->Register("PndTrackCandTpc","Tpc",_trackCandArray,_persistence);

  _pndTrackArray = new TClonesArray("PndTrack");
  ioman->Register("PndTrackTpc","Tpc",_pndTrackArray,_persistence);


  _trackfinder= new PndTpcRiemannTrackFinder();
  _trackfinder->setSorting(_sorting);
  _trackfinder->setInteractionZ(_interactionZ);
  _trackfinder->setSortingMode(_sortingMode);
  _trackfinder->setMinHitsForFit(_minpoints);
  _trackfinder->setScale(_riemannscale);
  _trackfinder->setTTProxcut(_TTproxcut);

  // Hit-Track Correlators
  _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut));
  _trackfinder->addCorrelator(new PndTpcHelixHTCorrelator(_helixcut));

  // Track-Track Correlators
  _trackfinder->addTTCorrelator(new PndTpcProximityTTCorrelator(_TTproxcut));
  _trackfinder->addTTCorrelator(new PndTpcDipTTCorrelator(_TTdipcut, _TThelixcut));
  _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));
 
  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",100,0,100);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1.01);
  _trackMcIdsH=new TH1D("trkmcids","# mcids in track",25,0,25);

  
  fnsectors= fpar->getPadPlane()->GetNSectors();
  std::cerr << "Found " << fnsectors << " sectors in padplane" << std::endl;
  for(unsigned int  isect=0;isect<fnsectors;++isect){
    fbuffermap[isect]=new std::vector<PndTpcCluster*>;
  }

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
    std::cerr<<"PndTpcRiemannTrackingTask: "<<"No const field! Curvature seeding not valid... Setting Bz="<<Bz<<std::endl;
  }
  else if(CField) {
    Bz=field->GetBz(0.,0.,0.);
    std::cerr<<"PndTpcRiemannTrackingTask: "<<"const field! Setting Bz="<<Bz<<std::endl;
  }
  else{
    Bz=20.;
    std::cerr<<"PndTpcRiemannTrackingTask: "<<"default setting Bz="<<Bz<<std::endl;
  }

  //init gPro
  if(_geane){
    gPro = new FairGeanePro();
  }

  return kSUCCESS;
}


void
PndTpcRiemannTrackingTask::SetParContainers() {

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
PndTpcRiemannTrackingTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcRiemannTrackingTask::Exec; Event Number: "<<counter++<<std::endl;

  // Reset output Arrays
  if(_trackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No TrackArray");
   _trackArray->Delete();
  
  if(_pndTrackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No PndTrackArray");
     _pndTrackArray->Delete();
  if(_trackCandArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No TrackCandArray");
     _trackCandArray->Delete();

  if(_riemannTrackArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No RiemannTrackArray");
     _riemannTrackArray->Delete();
  if(_riemannHitArray==0) Fatal("PndTpcSimpleRiemannTracking::Exec)","No RiemannHitArray");
     _riemannHitArray->Delete();

  // clean up friemannlist!
  for(int i=0; i<friemannlist.size(); ++i){
    if(friemannlist[i]!=NULL) delete friemannlist[i];
  }
  friemannlist.clear();

  for(unsigned int isect=0;isect<fnsectors;++isect) fbuffermap[isect]->clear();


  if (fVerbose) std::cerr<<"Fetching clusters from cluster branch..."<<std::endl;
  unsigned int ncl=_clusterArray->GetEntriesFast();
  for(unsigned int isect=0;isect<fnsectors;++isect)
    fbuffermap[isect]->reserve(ncl/fnsectors+10);
  for(unsigned int i=0; i<ncl; ++i){
    PndTpcCluster *cluster = (PndTpcCluster*)_clusterArray->At(i);
    unsigned int sectorId=cluster->sector();
    fbuffermap[sectorId]->push_back(cluster);
  }

  if (fVerbose) std::cerr << "Starting Pattern Reco..." << std::endl;


  std::vector<PndTpcRiemannTrack*> riemannTemp;

  // loop over sectors
  for(unsigned int isect=0;isect<fnsectors;++isect){
    if (fVerbose) std::cerr << "... building tracks in sector " << isect << std::endl;
    fcluster_buffer=fbuffermap[isect];
    _trackfinder->buildTracks(*fcluster_buffer,riemannTemp);
    //if(_doClean) _trackfinder->cleanTracks(friemannlist, _szcut, _planecut);
    if(_mergeTracks) _trackfinder->mergeTracks(riemannTemp);

    // copy tracklets of this sector to global list
    unsigned int ntrklts=riemannTemp.size();
    friemannlist.reserve(friemannlist.size()+ntrklts);
    for(unsigned int it=0;it<ntrklts;++it){
      friemannlist.push_back(riemannTemp[it]);
    }
    riemannTemp.clear();
  } // end loop over sectors

  //if(_doClean) _trackfinder->cleanTracks(friemannlist, _szcut, _planecut);

  if(_mergeTracks && fnsectors>1) {
    /*if(_sorting==3){
      _trackfinder->setSorting(2);
      _trackfinder->mergeTracks(friemannlist);
      _trackfinder->setSorting(_sorting);
    }*/
    _trackfinder->mergeTracks(friemannlist);
  }

  //if(_doClean && fnsectors>1) _trackfinder->cleanTracks(friemannlist, _szcut, _planecut);

  if (fVerbose) std::cerr << "Pattern Reco finished. "
                       << friemannlist.size() << " tracklets found." << std::endl;

  unsigned int _nbins=100;
    // analysing riemann tracks
    map<double,double> goodCl;
    for(unsigned int ib=0;ib<_nbins;++ib){
      double frac=1./(double)_nbins * (ib+1);
      goodCl[frac]=0;
    }
    //TGraph* gPE=new TGraph(nbins);
    unsigned int ntr=friemannlist.size();
    McIdCollection globalCol;
    for(unsigned int itr=0;itr<ntr;++itr){
      globalCol.AddIDCollection(friemannlist[itr]->mcid());
      map<double,double>::iterator it=goodCl.begin();
      while(it!=goodCl.end()){
        if(friemannlist[itr]->mcid().MaxRelWeight()>=it->first){
          it->second=it->second+1;
        }
        ++it;
      }// end loop over bins
    }// end loop over tracklets
    if (fVerbose) {
      cout << "Found " << globalCol.nIDs() << " mcids in tracklets" << endl;
      cout << "Purity: "<< endl;
      map<double,double>::iterator it=goodCl.begin();
    //unsigned int count=0;
    while(it!=goodCl.end()){
      //_gpurity->SetPoint(counter++, it->first, it->second /(double)ntr);
      cout << it->first << ":   " 
	   << it->second /(double)ntr*100. << "%" << endl;
      ++it;
    }// end loop over bins
  
    } // end if verbose


  // build GFTrackCands
  std::vector<GFTrackCand*> candlist;

  unsigned int nr=friemannlist.size();

  int minhits = 10; // minimum hits needed to build pndtrackcands and GFTrackCands
  if(minhits<_minpoints) minhits=_minpoints;
  double pbackup = 2.;  // momentum value that is set when other initialisations fail

  // loop over Riemann tracks
  std::cout<< "Looping over "<<nr<<" riemann tracks to write out" << std::endl;

  for(unsigned int itrk=0; itrk<nr; ++itrk){
    PndTpcRiemannTrack* trk=friemannlist[itrk];
    int nhits=trk->getNumHits();
    
    
    
    if (fVerbose) std::cout<<"Tracklet "<<itrk<<"   nhits = "<<nhits;

    // check if enough points
    if(nhits<minhits){
      if (fVerbose) std::cout<<" - skipping, not enough hits"<<std::endl;
      continue;
    }
    // check if track too steep
    double trackSinDip = trk->sinDip();
    if (TMath::Abs(trackSinDip)<0.01) {
      if (fVerbose) std::cout<<" - skipping, sin(dip) too small: "<<trackSinDip<<std::endl;
      continue;
    }
    // ceck if momentum high enough
    double p = trk->getMom(Bz);
    if (Bz==0) p=pbackup;
    if(p<1E-4) {
      if (fVerbose) std::cout<<" - skipping, momentum too small: "<<p*1E3<<" MeV"<<std::endl;
      continue;
    }
    if (fVerbose) std::cout<<std::endl;

    // store PndTpcRiemannTracks in output array
    new((*_riemannTrackArray)[_riemannTrackArray->GetEntries()]) PndTpcRiemannTrack(*trk);
    for(unsigned int ih=0;ih<nhits;++ih){
      PndTpcRiemannHit* hit=trk->getHit(ih);
      new ((*_riemannHitArray)[_riemannHitArray->GetEntries()]) PndTpcRiemannHit(*hit);
    }


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
    int winding = trk->winding(); // we look in z direction!

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

    // pdg
    int pdg = winding * 211; // Todo: pions hardcoded atm
    if(Bz<0) pdg *= -1;
    
    if(_mcPid){
      unsigned int trackId = trk->mcid().DominantID().mctrackID();
      int MCpdg = ((PndMCTrack*)(_mcTrackArray->At(trackId)))->GetPdgCode();

      double pdgCharge = TDatabasePDG::Instance()->GetParticle(pdg)->Charge();
      double MCpdgCharge = TDatabasePDG::Instance()->GetParticle(MCpdg)->Charge();
      if (fVerbose) {
        std::cout << "pdg: " << pdg << " charge: " << pdgCharge << std::endl;
        std::cout << "MC pid pdg: " << MCpdg << " charge: " << MCpdgCharge << std::endl;
      }

      if (pdgCharge*MCpdgCharge > 0.) pdg = MCpdg;
      else pdg = -1.*MCpdg;
      if (fVerbose) std::cout << "charge corrected MC pid pdg: " << pdg << std::endl;
    }


    double trackR = trk->r();

    if (fVerbose) {
      double trackDip = trk->dip();
      std::cout<<" center of track "; trk->center().Print();
      std::cout<<" Radius of track [cm]: " << trackR << std::endl;
      std::cout<<" Dip of track [deg]:   " << trackDip/TMath::Pi()*180 << std::endl;
      std::cout<<" seed values: "<<std::endl;
      std::cout<<"  start position: "; pos1.Print();
      std::cout<<"  momentum [GeV]: "<<p<<std::endl;
      std::cout<<"  p_perp [GeV]:   " << trackR*0.0003*TMath::Abs(Bz) <<std::endl;
      std::cout<<"  direction: "; direction.Print();
      std::cout<<"  winding: "<<winding<<std::endl;
      std::cout<<"  invertedTrack: "<<invertedTrack<<std::endl;
      std::cout<<"  pdg id: "<<pdg<<std::endl;
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
    _trackPurityH->Fill(mcid.MaxRelWeight());
    _trackMcIdsH->Fill(mcid.nIDs());
    _trackSizeH->Fill(cand->getNHits());


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
      if(part == 0){
        std::cerr << "PndTpcRiemannTrackingTask::Exec - unknown PDG id: " << pdg << std::endl;
        exit(1);
      }
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
  

  std::cout<<"PndTpcRiemannTrackingTask::Exec:: "
           <<candlist.size()<<" track candidates found."<<std::endl;

  _multiplicityHisto->Fill(candlist.size());

  








}

void
  PndTpcRiemannTrackingTask::SetStoreHistograms(TString file) {
  std::cerr<<"PndTpcRiemannTrackingTask::SetStoreHistograms() - empty implementation"<<std::endl;
}

void
  PndTpcRiemannTrackingTask::WriteHistograms(const TString& filename) {
  std::cerr<< "PndTpcRiemannTrackingTask::WriteHistograms"<<std::endl;

  TFile* file=FairRootManager::Instance()->GetOutFile();
  file->mkdir("RiemannTracking");
  file->cd("RiemannTracking");

  _multiplicityHisto->Write();
  delete _multiplicityHisto;
  _multiplicityHisto=NULL;

  _trackSizeH->Write();
  delete _trackSizeH;
  _trackSizeH=NULL;

  _trackPurityH->Write();
  delete _trackPurityH;
  _trackPurityH=NULL;

  _trackMcIdsH->Write();
  delete _trackMcIdsH;
  _trackMcIdsH=NULL;
}
