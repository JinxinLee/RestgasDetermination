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

#define MINHITS 10
#define PDGDEFAULT 211

ClassImp(PndTpcRiemannTrackingTask)

PndTpcRiemannTrackingTask::PndTpcRiemannTrackingTask()
  : FairTask("PndTpc Pattern Reco"), 

    _persistence(kFALSE),
    _riemannPersistence(kFALSE),
    fnsectors(1),
    _maxRadius(100),

    _sortingMode(true),
    _sorting(3),
    _interactionZ(0.),

    _minpoints(4),
    _proxcut(1.9),
    _proxZstretch(1.6),
    _helixcut(0.3),

    _mergeTracks(true),
    _TTproxcut(7.0),
    _TTdipcut(.1),
    _TThelixcut(0.3),
    _TTplanecut(0.015),

    _MergeCurlers(false),
    _blowUp(1.),

    _skipCrossingAreas(true),

    _doMultistep(true),
    _minHitsZ(20),
    _minHitsPhi(15),

    _riemannscale(24.6),

    _clusterBranchName("PndTpcCluster"),
    _smoothing(true),
    _geane(false),

    _mcPid(false),
    _pdg(PDGDEFAULT),
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
PndTpcRiemannTrackingTask::SetMultistepParameters(bool doMultistep,
                 unsigned int minHitsZ,
                 unsigned int minHitsPhi){
  _doMultistep=doMultistep;
  _minHitsZ=minHitsZ;
  _minHitsPhi=minHitsPhi;
}

void
PndTpcRiemannTrackingTask::SetTrkFinderParameters(
                 double proxcut,
					       double helixcut,
					       unsigned int minpointsforfit,
					       double zStretch){
  _proxcut=proxcut;
  _helixcut=helixcut;
  _minpoints=minpointsforfit;
  _proxZstretch=zStretch;
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

  if(ioman==0){
    Error("PndTpcRiemannTrackingTask::Init","RootManager not instantiated!");
    return kERROR;
  }

  _mcTrackArray=(TClonesArray*) ioman->GetObject("MCTrack");
  if(_mcTrackArray==0){
    Error("PndTpcdEdxTask::Init","MCTrack-array not found! Cannot use ideal PID");
    _mcPid=false;
  }
    
  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);
  if(_clusterArray==0){
    Error("PndTpcRiemannTrackingTask::Init","Cluster-array not found!");
    return kERROR;
  }

  /*_mvdArray=(TClonesArray*) ioman->GetObject("MVDPoint");
  if(_mvdArray==0){
    Error("PndTpcRiemannTrackingTask::Init","mvd-array not found!");
  }*/

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

  _trackfinder->initTracks(false);
  _trackfinder->SkipCrossingAreas(_skipCrossingAreas);
  _trackfinder->SetSkipAndDelete(false);

  _trackfinder->setScale(_riemannscale);

  _trackfinder->setProxcut(_proxcut);
  _trackfinder->setTTProxcut(_TTproxcut);

  // Hit-Track Correlators
  _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut, _proxZstretch));
  _trackfinder->addCorrelator(new PndTpcHelixHTCorrelator(_helixcut));

  // Track-Track Correlators
  _trackfinder->addTTCorrelator(new PndTpcProximityTTCorrelator(_TTproxcut));
  _trackfinder->addTTCorrelator(new PndTpcDipTTCorrelator(_TTdipcut, _TThelixcut));
  _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));



  // for merging curling tracks with increased TT helixcut
  _trackfinderCurl= new PndTpcRiemannTrackFinder();
  _trackfinderCurl->setSorting(_sorting);
  _trackfinderCurl->setSortingMode(_sortingMode);
  _trackfinderCurl->setMinHitsForFit(_minpoints);
  _trackfinderCurl->setScale(_riemannscale);
  _trackfinderCurl->setMaxNumHitsForPR(_minpoints);

  _trackfinderCurl->setProxcut(_proxcut);
  _trackfinderCurl->setTTProxcut(2000.);

  // Track-Track Correlators
  _trackfinderCurl->addTTCorrelator(new PndTpcDipTTCorrelator(_TTdipcut, _blowUp*_TThelixcut));
  _trackfinderCurl->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));
 


  // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",100,0,100);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1.01);
  _trackMcIdsH=new TH1D("trkmcids","# mcids in track",25,0,25);


  // get the maximum radius
  _maxRadius = fpar->getRMax();
  
  fnsectors= fpar->getPadPlane()->GetNSectors();
  std::cerr << "Found " << fnsectors << " sectors in padplane; outer radius = " << _maxRadius << std::endl;
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
  if(_geane) gPro = new FairGeanePro();

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


  std::vector<PndTpcRiemannTrack*> riemannTempSec; // temporary storage, reused for every sector
  std::vector<PndTpcRiemannTrack*> riemannTempCirc; // temporary global storage for circle tracks
  std::vector<PndTpcRiemannTrack*> riemannTemp; // temporary global storage for arbitrary tracks

  unsigned int nTotCl(0);


  // loop over sectors
  for(unsigned int isect=0;isect<fnsectors;++isect){
    if (fVerbose) std::cerr << "\n... building tracks in sector " << isect << " from " << fbuffermap[isect]->size() << " clusters" << std::endl;

    fcluster_buffer=fbuffermap[isect];
    nTotCl += fcluster_buffer->size();

    unsigned int nErasedCl(0);


    if(_doMultistep){

      // find steep tracks
      _trackfinder->setSorting(2);
      _trackfinder->setMinHits(_minHitsZ);
      _trackfinder->SkipCrossingAreas(true);
      _trackfinder->SetSkipAndDelete(true);

      _trackfinder->buildTracks(*fcluster_buffer,riemannTempSec);

      // put found goodtracks into friemannlist and delete clusters from buffer
      unsigned int nGoodSteepTrks(0);
      for(unsigned int i=0; i<riemannTempSec.size(); ++i){
        // store good tracklets in friemannlist
        if (riemannTempSec[i]->getNumHits() > _minHitsZ){
          friemannlist.push_back(riemannTempSec[i]);

          // clear clusters
          for(unsigned int iCl=0; iCl < riemannTempSec[i]->getNumHits(); ++iCl){
            fcluster_buffer->erase( remove(fcluster_buffer->begin(), fcluster_buffer->end(),
                                    riemannTempSec[i]->getHit(iCl)->cluster()), fcluster_buffer->end() );
            ++nErasedCl;
          }
          ++nGoodSteepTrks;
        }
        else{ // delete bad tracklets
          riemannTempSec[i]->deleteHits();
          delete riemannTempSec[i];
        }
      }
      // clear riemannTempSec
      riemannTempSec.clear();

      if (fVerbose) std::cerr << "   found steep tracks: " <<  nGoodSteepTrks << std::endl;
      // end find steep tracks


      // find circle tracks
      _trackfinder->setSorting(5);
      _trackfinder->setMinHits(_minHitsPhi);
      _trackfinder->SkipCrossingAreas(true);
      _trackfinder->SetSkipAndDelete(true);

      _trackfinder->buildTracks(*fcluster_buffer,riemannTempSec);

      // put found goodtracks into friemannlist and delete clusters from buffer
      unsigned int nGoodCirlceTrks(0);
      for(unsigned int i=0; i<riemannTempSec.size(); ++i){
        // store good tracklets in friemannlist
        if (riemannTempSec[i]->getNumHits() > _minHitsPhi) {
          riemannTempCirc.push_back(riemannTempSec[i]);

          // clear clusters
          for(unsigned int iCl=0; iCl < riemannTempSec[i]->getNumHits(); ++iCl){
            fcluster_buffer->erase( remove(fcluster_buffer->begin(), fcluster_buffer->end(),
                                    riemannTempSec[i]->getHit(iCl)->cluster()), fcluster_buffer->end() );
            ++nErasedCl;
          }
          ++nGoodCirlceTrks;
        }
        else{ // delete bad tracklets
          riemannTempSec[i]->deleteHits();
          delete riemannTempSec[i];
        }
      }
      // clear riemannTempSec
      riemannTempSec.clear();


      // find circle tracks in other direction
      _trackfinder->setSorting(-5);

      _trackfinder->buildTracks(*fcluster_buffer,riemannTempSec);

      // put found goodtracks into friemannlist and delete clusters from buffer
      for(unsigned int i=0; i<riemannTempSec.size(); ++i){
        // store good tracklets in friemannlist
        if (riemannTempSec[i]->getNumHits() > _minHitsPhi) {
          riemannTempCirc.push_back(riemannTempSec[i]);

          // clear clusters
          for(unsigned int iCl=0; iCl < riemannTempSec[i]->getNumHits(); ++iCl){
            fcluster_buffer->erase( remove(fcluster_buffer->begin(), fcluster_buffer->end(),
                                    riemannTempSec[i]->getHit(iCl)->cluster()), fcluster_buffer->end() );
            ++nErasedCl;
          }
          ++nGoodCirlceTrks;
        }
        else{ // delete bad tracklets
          riemannTempSec[i]->deleteHits();
          delete riemannTempSec[i];
        }
      }
      // clear riemannTempSec
      riemannTempSec.clear();

      if (fVerbose) std::cerr << "   found circle tracks: " <<  nGoodCirlceTrks << std::endl;
      // end find circle tracks

      if (fVerbose) std::cerr << "\n   this reduced the number of clusters by " <<  nErasedCl << std::endl;


      // build rest of the tracks
      _trackfinder->SkipCrossingAreas(true);
      _trackfinder->SetSkipAndDelete(false);
      _trackfinder->setSorting(3);
    }

    _trackfinder->buildTracks(*fcluster_buffer,riemannTempSec);
    // end build rest of the tracks


    if(_mergeTracks) {
      if (fVerbose) std::cerr << "    merge " << riemannTempSec.size() << " tracks in sector " << isect;
      _trackfinder->mergeTracks(riemannTempSec);
      if (fVerbose) std::cerr << " ... done - created " << riemannTempSec.size() << " merged tracks" <<std::endl;
    }


    // copy tracklets of this sector to global list
    unsigned int ntrklts=riemannTempSec.size();
    riemannTemp.reserve(riemannTemp.size()+ntrklts);
    for(unsigned int it=0;it<ntrklts;++it){
      riemannTemp.push_back(riemannTempSec[it]);
    }
    riemannTempSec.clear();

  } // end loop over sectors


  // clear small tracklets
  if(true){
    for (unsigned int i=0; i<riemannTemp.size(); ++i){
      if (riemannTemp[i]->getNumHits() < _minpoints){
        riemannTemp[i]->deleteHits();
        delete riemannTemp[i];
        riemannTemp.erase(riemannTemp.begin() + i);
        --i;
      }
    }
  }

  if(_mergeTracks && fnsectors>1) {
    if(_doMultistep){
      if (fVerbose) std::cerr << "merge " << riemannTempCirc.size() << " circular tracks ... ";
      _trackfinder->mergeTracks(riemannTempCirc);
      if (fVerbose) std::cerr << " done - created " << riemannTempCirc.size() << " merged tracks" <<std::endl;
    }

    if (fVerbose) std::cerr << "\nmerge " << riemannTemp.size() << " tracks ... ";
    _trackfinder->mergeTracks(riemannTemp);
    if (fVerbose) std::cerr << " done - created " << riemannTemp.size() << " merged tracks" <<std::endl;
  }


  //append riemannTempCirc to friemannlist
  if(_doMultistep){
    for (unsigned int i=0; i<riemannTempCirc.size(); ++i){
      friemannlist.push_back(riemannTempCirc[i]);
    }
  }

  //append RiemannTemp to friemannlist
  for (unsigned int i=0; i<riemannTemp.size(); ++i){
    friemannlist.push_back(riemannTemp[i]);
  }

  // clear small tracklets
  if(true){
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      if (friemannlist[i]->getNumHits() <= _minpoints+1 ||
          (friemannlist[i]->getFirstHit()->cluster()->pos() -
           friemannlist[i]->getLastHit()->cluster()->pos()).Mag() < 2.){
        friemannlist[i]->deleteHits();
        delete friemannlist[i];
        friemannlist.erase(friemannlist.begin() + i);
        --i;
      }
    }
  }

  if(_mergeTracks && fnsectors>1 && _doMultistep) {
    if (fVerbose) std::cerr << "\nfinal merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
    _trackfinder->mergeTracks(friemannlist);
    if (fVerbose) std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
  }


  if(_MergeCurlers){
    std::vector<PndTpcRiemannTrack*> riemannTempCurl;
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      if (friemannlist[i]->isFitted() &&
          friemannlist[i]->r() < 30. &&
          friemannlist[i]->getMom(Bz) < 0.5 &&
          fabs(friemannlist[i]->m()*1.57) < 140){ // Pi/2
        riemannTempCurl.push_back(friemannlist[i]);
        friemannlist.erase(friemannlist.begin() + i);
        --i;
      }
    }

    if (fVerbose) std::cerr << "\nmerge curlers: merge " << riemannTempCurl.size() << " tracks ... ";
    _trackfinderCurl->mergeTracks(riemannTempCurl);
    if (fVerbose) std::cerr << " done1 - created " << riemannTempCurl.size() << " merged tracks" <<std::endl;
    _trackfinderCurl->mergeTracks(riemannTempCurl);
    if (fVerbose) std::cerr << " done2 - created " << riemannTempCurl.size() << " merged tracks" <<std::endl;


    for (unsigned int i=0; i<riemannTempCurl.size(); ++i){
      friemannlist.push_back(riemannTempCurl[i]);
    }
  }


  unsigned int nUsedCl(0);
  for (unsigned int i=0; i<friemannlist.size(); ++i){
    nUsedCl += friemannlist[i]->getNumHits();
  }

  if (fVerbose) {
    std::cerr << "Pattern Reco finished, found tracks: " << friemannlist.size() << "\n";
    std::cerr << "used " << nUsedCl << " of " << nTotCl << " Clusters \n";
  }
  //----------------------------------------------------------------------------------------------------
  // end PR



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
        if(ntr>0){
          cout << it->first << ":   "
            << it->second /(double)ntr*100. << "%" << endl;
        }
        ++it;
      }// end loop over bins
    } // end if verbose


  // build GFTrackCands
  std::vector<GFTrackCand*> candlist;

  unsigned int nr=friemannlist.size();

  double pbackup = 2.;  // momentum value that is set when other initialisations fail

  // loop over Riemann tracks
  std::cout<< "Looping over "<<nr<<" riemann tracks to write out" << std::endl;

  for(unsigned int itrk=0; itrk<nr; ++itrk){
    PndTpcRiemannTrack* trk=friemannlist[itrk];
    int nhits=trk->getNumHits();
    
    if (fVerbose) std::cout<<"Tracklet "<<itrk<<"   nhits = "<<nhits;

    // check if fitted
    if(!trk->isFitted()){
      if (fVerbose) std::cout<<" - skipping, tracklet not prefitted"<<std::endl;
      continue;
    }
    // check if enough points
    if(nhits<MINHITS){
      if (fVerbose) std::cout<<" - skipping, not enough hits: "<<nhits<<std::endl;
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
    if(p<1.E-2) {  // 10 MeV ~ 3cm helix diameter
      if (fVerbose) std::cout<<" - skipping, momentum too small: "<<p*1E3<<" MeV"<<std::endl;
      continue;
    }


    unsigned int trackId(trk->mcid().DominantID().mctrackID());

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


    // store PndTpcRiemannTracks in output array
    new((*_riemannTrackArray)[_riemannTrackArray->GetEntries()]) PndTpcRiemannTrack(*trk);
    for(unsigned int ih=0;ih<nhits;++ih){
      PndTpcRiemannHit* hit=trk->getHit(ih);
      new ((*_riemannHitArray)[_riemannHitArray->GetEntries()]) PndTpcRiemannHit(*hit);
    }


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
    momerr *= trk->resolution();

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
  
  std::cout<<"PndTpcRiemannTrackingTask::Exec:: "<<candlist.size()<<" track candidates found."<<std::endl;
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
