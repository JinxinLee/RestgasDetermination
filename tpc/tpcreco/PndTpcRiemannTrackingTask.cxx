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

     counter(0)
  {
    fVerbose = 0;
  }

PndTpcRiemannTrackingTask::~PndTpcRiemannTrackingTask(){
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

  _riemannTrackArray = new TClonesArray("PndTpcRiemannTrack");
  ioman->Register("RiemannTrack","Tpc",_riemannTrackArray,_persistence);

  _riemannHitArray = new TClonesArray("PndTpcRiemannHit");
  ioman->Register("RiemannHit","Tpc",_riemannHitArray,_persistence);
    
  

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
 


  // get the maximum radius
  _maxRadius = fpar->getRMax();
  
  fnsectors= fpar->getPadPlane()->GetNSectors();
  std::cerr << "Found " << fnsectors << " sectors in padplane; outer radius = " << _maxRadius << std::endl;
  for(unsigned int  isect=0;isect<fnsectors;++isect){
    fbuffermap[isect]=new std::vector<PndTpcCluster*>;
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
  if(_riemannTrackArray==0) Fatal("PndTpcRiemannTrackingTask::Exec","No RiemannTrackArray");
     _riemannTrackArray->Delete();

  if(_riemannHitArray==0) Fatal("PndTpcRiemannTrackingTask::Exec","No RiemannHitArray");
     _riemannHitArray->Delete();


  // clean up friemannlist!
  for(int i=0; i<friemannlist.size(); ++i){
    if(friemannlist[i]!=NULL) {
      friemannlist[i]->deleteHits();
      delete friemannlist[i];
    }
  }
  friemannlist.clear();

  for(unsigned int isect=0;isect<fnsectors;++isect) fbuffermap[isect]->clear();


  if (fVerbose) std::cout<<"Fetching clusters from cluster branch..."<<std::endl;
  unsigned int ncl=_clusterArray->GetEntriesFast();
  for(unsigned int isect=0;isect<fnsectors;++isect)
    fbuffermap[isect]->reserve(ncl/fnsectors + 1000);
  for(unsigned int i=0; i<ncl; ++i){
    PndTpcCluster *cluster = (PndTpcCluster*)_clusterArray->At(i);
    unsigned int sectorId=cluster->sector();
    fbuffermap[sectorId]->push_back(cluster);
  }

  if (fVerbose) std::cout << "Starting Pattern Reco..." << std::endl;


  std::vector<PndTpcRiemannTrack*> riemannTempSec; // temporary storage, reused for every sector
  std::vector<PndTpcRiemannTrack*> riemannTempCirc; // temporary global storage for circle tracks
  std::vector<PndTpcRiemannTrack*> riemannTemp; // temporary global storage for arbitrary tracks

  unsigned int nTotCl(0), nCl;


  // loop over sectors
  for(unsigned int isect=0;isect<fnsectors;++isect){

    nCl = fbuffermap[isect]->size();
    if (nCl==0) continue;

    if (fVerbose) std::cout << "\n... building tracks in sector " << isect << " from " << nCl << " clusters" << std::endl;

    fcluster_buffer=fbuffermap[isect];
    nTotCl += nCl;

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

      if (fVerbose) std::cout << "   found steep tracks: " <<  nGoodSteepTrks << std::endl;
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

      if (fVerbose) std::cout << "   found circle tracks: " <<  nGoodCirlceTrks << std::endl;
      // end find circle tracks

      if (fVerbose) std::cout << "\n   this reduced the number of clusters by " <<  nErasedCl << std::endl;


      // build rest of the tracks
      _trackfinder->SkipCrossingAreas(true);
      _trackfinder->SetSkipAndDelete(false);
      _trackfinder->setSorting(3);
    }

    _trackfinder->buildTracks(*fcluster_buffer,riemannTempSec);
    // end build rest of the tracks


    if(_mergeTracks) {
      if (fVerbose) std::cout << "    merge " << riemannTempSec.size() << " tracks in sector " << isect;
      _trackfinder->mergeTracks(riemannTempSec);
      if (fVerbose) std::cout << " ... done - created " << riemannTempSec.size() << " merged tracks" <<std::endl;
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
      if (fVerbose) std::cout << "merge " << riemannTempCirc.size() << " circular tracks ... ";
      _trackfinder->mergeTracks(riemannTempCirc);
      if (fVerbose) std::cout << " done - created " << riemannTempCirc.size() << " merged tracks" <<std::endl;
    }

    if (fVerbose) std::cout << "\nmerge " << riemannTemp.size() << " tracks ... ";
    _trackfinder->mergeTracks(riemannTemp);
    if (fVerbose) std::cout << " done - created " << riemannTemp.size() << " merged tracks" <<std::endl;
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
    if (fVerbose) std::cout << "\nfinal merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
    _trackfinder->mergeTracks(friemannlist);
    if (fVerbose) std::cout << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
  }


  if(_MergeCurlers){
    std::vector<PndTpcRiemannTrack*> riemannTempCurl;
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      if (friemannlist[i]->isFitted() &&
          friemannlist[i]->r() < 30. &&
          fabs(friemannlist[i]->m()*1.57) < 140){ // Pi/2
        riemannTempCurl.push_back(friemannlist[i]);
        friemannlist.erase(friemannlist.begin() + i);
        --i;
      }
    }

    if (fVerbose) std::cout << "\nmerge curlers: merge " << riemannTempCurl.size() << " tracks ... ";
    _trackfinderCurl->mergeTracks(riemannTempCurl);
    if (fVerbose) std::cout << " done1 - created " << riemannTempCurl.size() << " merged tracks" <<std::endl;
    _trackfinderCurl->mergeTracks(riemannTempCurl);
    if (fVerbose) std::cout << " done2 - created " << riemannTempCurl.size() << " merged tracks" <<std::endl;


    for (unsigned int i=0; i<riemannTempCurl.size(); ++i){
      friemannlist.push_back(riemannTempCurl[i]);
    }
  }// end merge curlers

  unsigned int foundTrks = friemannlist.size();

  // store PndTpcRiemannTracks and Hits in output array
  PndTpcRiemannTrack* trk;
  unsigned int nUsedCl(0), nHits;
  for (unsigned int i=0; i<foundTrks; ++i){
    trk = friemannlist[i];
    nHits = trk->getNumHits();
    nUsedCl += nHits;

    new((*_riemannTrackArray)[_riemannTrackArray->GetEntriesFast()]) PndTpcRiemannTrack(*trk);
    for(unsigned int ih=0; ih<nHits; ++ih){
      PndTpcRiemannHit* hit=trk->getHit(ih);
      new ((*_riemannHitArray)[_riemannHitArray->GetEntriesFast()]) PndTpcRiemannHit(*hit);
    }
  }


  std::cout << "Pattern Reco finished, found tracks: " << foundTrks << "\n";
  if (fVerbose) {
    std::cout << "used " << nUsedCl << " of " << nTotCl << " Clusters \n";
  }

}

