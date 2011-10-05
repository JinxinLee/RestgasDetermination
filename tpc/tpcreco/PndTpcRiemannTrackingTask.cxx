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

#define MINHITS 4
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

    /*_minpoints(3), // panda settings
    _proxcut(1.9),
    _proxZstretch(1.6),
    _helixcut(0.2),
    _maxRMS(0.15),

    _mergeTracks(true),
    _TTproxcut(15.0),
    _TTdipcut(0.2),
    _TThelixcut(0.5),
    _TTplanecut(0.3),*/

    // FOPI settings
    _minpoints(3),
    _proxcut(2.0),
    _proxZstretch(1.6),
    _helixcut(0.4),
    _maxRMS(0.3),

    _mergeTracks(true),
    _TTproxcut(15.0),
    _TTdipcut(0.2),
    _TThelixcut(0.5),
    _TTplanecut(0.3),

    _MergeCurlers(true),
    _blowUp(1.),

    _skipCrossingAreas(true),

    _doMultistep(true),
    _minHitsZ(10),
    _minHitsR(10),
    _minHitsPhi(10),


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
                 unsigned int minHitsR,
                 unsigned int minHitsPhi){
  _doMultistep = doMultistep;
  _minHitsZ = minHitsZ;
  _minHitsR = minHitsR;
  _minHitsPhi = minHitsPhi;
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
  _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut, _proxZstretch, _helixcut));
  _trackfinder->addCorrelator(new PndTpcHelixHTCorrelator(_helixcut));

  // Track-Track Correlators
  _trackfinder->addTTCorrelator(new PndTpcDipTTCorrelator(_TTproxcut, _TTdipcut, _TThelixcut));
  _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));



  // for merging curling tracks with increased TT helixcut
  _trackfinderCurl= new PndTpcRiemannTrackFinder();
  _trackfinderCurl->setSorting(_sorting);
  _trackfinderCurl->setSortingMode(_sortingMode);
  _trackfinderCurl->setMinHitsForFit(_minpoints);
  _trackfinderCurl->setScale(_riemannscale);
  _trackfinderCurl->setMaxNumHitsForPR(_minpoints);

  _trackfinderCurl->setProxcut(_proxcut);
  _trackfinderCurl->setTTProxcut(30.);

  // Track-Track Correlators
  _trackfinderCurl->addTTCorrelator(new PndTpcDipTTCorrelator(30., _blowUp*_TTdipcut, _blowUp*_TThelixcut));
  _trackfinderCurl->addTTCorrelator(new PndTpcRiemannTTCorrelator(1.5*_TTplanecut, 20));
 


  // get the maximum radius
  _maxRadius = fpar->getRMax();
  
  fnsectors= fpar->getPadPlane()->GetNSectors();
  std::cerr << "Found " << fnsectors << " sectors in padplane; outer radius = " << _maxRadius << std::endl;
  for(unsigned int  isect=0;isect<fnsectors;++isect){
    fbuffermap[isect]=new std::vector<PndTpcCluster*>;
  }

  tpcOffset = fpar->getZGem();
  tpcLength = fpar->getZMax() - tpcOffset;

  std::cout << "TPC Offset: " << tpcOffset << "  \tTPC Length: " << tpcLength << std::endl;

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
    fbuffermap[isect]->reserve(ncl/fnsectors + 2000);
  for(unsigned int i=0; i<ncl; ++i){
    PndTpcCluster *cluster = (PndTpcCluster*)_clusterArray->At(i);
    unsigned int sectorId=cluster->sector();
    fbuffermap[sectorId]->push_back(cluster);
  }

  if (fVerbose) std::cout << "Starting Pattern Reco..." << std::endl;

  unsigned int nTotCl(0); // number of total clusters
  for(unsigned int isect=0;isect<fnsectors;++isect){
    nTotCl += fbuffermap[isect]->size();
  }

  std::vector<PndTpcRiemannTrack*> riemannTempSec; // temporary storage, reused for every sector

  // first loop over sectors
  for(unsigned int isect=0;isect<fnsectors;++isect){
    if (fbuffermap[isect]->size() == 0) continue;
    if (fVerbose) std::cout << "\n... building tracks in sector " << isect << " from " << fbuffermap[isect]->size() << " clusters" << std::endl;

    fcluster_buffer=fbuffermap[isect];

      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 2, _minHitsZ, 0.7*_maxRMS);
    buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 3, _minHitsR, _maxRMS);

    riemannTempSec.clear();
  } // end loop over sectors


  if(_mergeTracks) {
    if (fVerbose) std::cerr << "\n merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
    _trackfinder->mergeTracks(friemannlist);
    if (fVerbose) std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
  }


  // resectorize: 10 sectors, left and right half of chamber, each 5 sectors in z
  int factor, zSlice, nSlices(5);
  if (fnsectors > 2*nSlices){
    std::vector<PndTpcCluster*> remainingClusters;
    remainingClusters.reserve(200000);

    for(unsigned int isect=0; isect<fnsectors; ++isect){
      for(unsigned int iCl=0; iCl<fbuffermap[isect]->size(); ++iCl){
        remainingClusters.push_back((*fbuffermap[isect])[iCl]);
      }
    }
    for(unsigned int isect=0;isect<fnsectors;++isect){
      fbuffermap[isect]->clear();
    }
    PndTpcCluster* Cl;
    for(unsigned int iCl=0; iCl<remainingClusters.size(); ++iCl){
      Cl = remainingClusters[iCl];
      if (Cl->pos().X()<0) factor=0;
      else factor=1;

      for (zSlice=0; zSlice<nSlices; ++zSlice){
        if (Cl->pos().Z() < tpcOffset + (zSlice+1)*tpcLength/nSlices) break;
      }

      fbuffermap[factor + 2*zSlice]->push_back(Cl);
    }
  }

  // second loop over sectors
  double lowLim(tpcOffset), upLim;
  for(unsigned int isect=0;isect<fnsectors;++isect){
    if (fbuffermap[isect]->size() == 0) continue;
    if (fVerbose) std::cerr << "\n... building tracks in sector " << isect << " from " << fbuffermap[isect]->size() << " clusters" << std::endl;

    fcluster_buffer=fbuffermap[isect];

    // fill riemannTempSec with tracks lying in the sector
    upLim = lowLim + tpcLength/nSlices;
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      double z(friemannlist[i]->getFirstHit()->z());
      if (z>lowLim-2 && z<upLim+2){
        riemannTempSec.push_back(friemannlist[i]);
        continue;
      }

      z = friemannlist[i]->getLastHit()->z();
      if (z>lowLim-2 && z<upLim+2){
        riemannTempSec.push_back(friemannlist[i]);
        continue;
      }
    }
    lowLim=upLim;

    buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 5, _minHitsPhi, _maxRMS);
    buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, -5, _minHitsPhi, _maxRMS);

    riemannTempSec.clear();

  } // end loop over sectors


  if(_mergeTracks) {
    if (fVerbose) std::cerr << "\n merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
    _trackfinder->mergeTracks(friemannlist);
    if (fVerbose) std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
  }



  // third loop over sectors
  lowLim = tpcOffset;
  for(unsigned int isect=0;isect<fnsectors;++isect){
    if (fbuffermap[isect]->size() == 0) continue;
    if (fVerbose) std::cerr << "\n... building tracks in sector " << isect << " from " << fbuffermap[isect]->size() << " clusters" << std::endl;

    fcluster_buffer=fbuffermap[isect];


    // fill riemannTempSec with tracks lying in the sector
    upLim = lowLim + tpcLength/nSlices;
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      double z(friemannlist[i]->getFirstHit()->cluster()->pos().Z());
      if (z>lowLim-1 && z<upLim+1){
        riemannTempSec.push_back(friemannlist[i]);
        continue;
      }

      z = friemannlist[i]->getLastHit()->cluster()->pos().Z();
      if (z>lowLim-1 && z<upLim+1){
        riemannTempSec.push_back(friemannlist[i]);
        continue;
      }
    }
    lowLim=upLim;

    unsigned int nTrks = riemannTempSec.size();


    buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 2, _minpoints+1, _maxRMS*1.5);
      buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 3, _minpoints+3, _maxRMS);
    buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, 5, _minpoints+1, _maxRMS*1.5);
    buildTracks(_trackfinder, fcluster_buffer, &riemannTempSec, -5, _minpoints+1, _maxRMS*1.5);

    riemannTempSec.clear();
  } // end loop over sectors


  if(_mergeTracks) {
    if (fVerbose) std::cerr << "\nfinal merge of friemannlist: merge " << friemannlist.size() << " tracks ... ";
    _trackfinder->mergeTracks(friemannlist);
    if (fVerbose) std::cerr << " done - created " << friemannlist.size() << " merged tracks" <<std::endl;
  }




  if(_MergeCurlers){
    std::vector<PndTpcRiemannTrack*> riemannTempCurl;
    for (unsigned int i=0; i<friemannlist.size(); ++i){
      if (friemannlist[i]->isFitted() &&
            friemannlist[i]->getNumHits() > 5 &&
            friemannlist[i]->r() < 30. &&
            fabs(friemannlist[i]->m()*1.57) < 120){ // Pi/2
        riemannTempCurl.push_back(friemannlist[i]);
        friemannlist.erase(friemannlist.begin() + i);
        --i;
      }
    }

    if (fVerbose) std::cerr << "\nmerge curlers: merge " << riemannTempCurl.size() << " tracks ... ";
    _trackfinderCurl->mergeTracks(riemannTempCurl);
    if (fVerbose) std::cerr << " done - created " << riemannTempCurl.size() << " merged tracks" <<std::endl;

    for (unsigned int i=0; i<riemannTempCurl.size(); ++i){
      friemannlist.push_back(riemannTempCurl[i]);
    }
  }// end merge curlers



  // clear small tracklets with < MINHITS hits
  for (unsigned int i=0; i<friemannlist.size(); ++i){
    if (friemannlist[i]->getNumHits() < MINHITS){
      friemannlist.erase(friemannlist.begin() + i);
      --i;
    }
  }


  unsigned int foundTrks(friemannlist.size());

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

  if (fVerbose) {
    std::cerr << "Pattern Reco finished, found tracks: " << foundTrks << "\n";
    std::cerr << "used  " << nUsedCl << " of " << nTotCl << " Clusters \n";
  }

}


void PndTpcRiemannTrackingTask::buildTracks(PndTpcRiemannTrackFinder* trackfinder,
                   std::vector<PndTpcCluster*>* clusterBuffer,
                   std::vector<PndTpcRiemannTrack*>* TrackletList,
                   int sorting,
                   unsigned int minHits,
                   double maxRMS,
                   bool skipCrossingAreas,
                   bool skipAndDelete){

  trackfinder->setSorting(sorting);
  trackfinder->setMinHits(minHits);
  trackfinder->SkipCrossingAreas(skipCrossingAreas);
  trackfinder->SetSkipAndDelete(skipAndDelete);

  std::vector<PndTpcRiemannTrack*> TrackletListCopy = *TrackletList;

  int nTracksIn(TrackletList->size());
  int nClIn(clusterBuffer->size());


  PndTpcRiemannTrack* LastTrackIn;
  if(nTracksIn > 0) LastTrackIn = TrackletList->back();

  trackfinder->buildTracks(*clusterBuffer, *TrackletList);

  unsigned int nGoodTrks(0), nErasedCl(0), nHits;
  PndTpcRiemannTrack* trk;

  for(unsigned int i=0; i<TrackletList->size(); ++i){
    trk = (*TrackletList)[i];

    nHits = trk->getNumHits();

    if (trk->distRMS() < maxRMS &&
        (nHits >= minHits || trk->isGood())){
      trk->setFinished(false);
      trk->setGood();
      // clear clusters from good tracklets
      for(unsigned int iCl=0; iCl < nHits; ++iCl){
        clusterBuffer->erase( remove(clusterBuffer->begin(), clusterBuffer->end(),
                                     trk->getHit(iCl)->cluster()),
                              clusterBuffer->end() );
      }
      ++nGoodTrks;

      //push back unique track to riemannlist
      if (std::find(TrackletListCopy.begin(), TrackletListCopy.end(), trk) == TrackletListCopy.end()){
        friemannlist.push_back(trk);
      }
    }
    else{ // delete bad tracklets
      if (trk->isGood()){ // track has been found before (->clusters were taken out) but does not pass quality criteria anymore -> fill clusters back into buffer
        for(unsigned int iCl=0; iCl < nHits; ++iCl){
          clusterBuffer->push_back(trk->getHit(iCl)->cluster());
        }
      }
      // delete hits and track
      trk->deleteHits();
      delete trk;
      TrackletList->erase(TrackletList->begin()+i);

      // also has to be removed from friemannlist
      friemannlist.erase( remove(friemannlist.begin(), friemannlist.end(), trk),
                          friemannlist.end() );

      --i;
    }
  }

  if (fVerbose) std::cout << "   found good tracks: " <<  nGoodTrks-nTracksIn << ", reduced nCl by " << nClIn-clusterBuffer->size() << std::endl;
}

