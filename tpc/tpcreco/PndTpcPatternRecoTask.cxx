//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcPatternRecoTask
//      see PndTpcPatternRecoTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcPatternRecoTask.h"

// C/C++ Headers ----------------------
#include <map>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "TClonesArray.h"
#include "PndTpcRiemannTrackFinder.h"
#include "PndTpcRiemannTrack.h"
#include "PndTpcProximityHTCorrelator.h"
#include "PndTpcRiProxHTCorrelator.h"
#include "PndTpcSzHTCorrelator.h"
#include "PndTpcProximityTTCorrelator.h"
#include "PndTpcRiemannHTCorrelator.h"
#include "PndTpcSzTTCorrelator.h"
#include "PndTpcRiemannTTCorrelator.h"
#include "PndTpcDigiPar.h"
#include "FairRuntimeDb.h"
#include "TH1I.h"
#include "TH1D.h"
#include "TGraph.h"
#include "McIdCollection.h"
//#include "AbsBFieldIfc.h"
//#include "FairFieldAdaptor.h"

#include <cmath>

using std::fabs;
using namespace std;

// Class Member definitions -----------

ClassImp(PndTpcPatternRecoTask)

PndTpcPatternRecoTask::PndTpcPatternRecoTask()
  : FairTask("PndTpc Pattern Reco"), _persistence(kFALSE)
{
   _clusterBranchName = "PndTpcCluster";
}

PndTpcPatternRecoTask::~PndTpcPatternRecoTask()
{
  if(_multiplicityHisto!=NULL)delete _multiplicityHisto;
  if(_trackPurityH!=NULL)delete _trackPurityH;
  if(_trackSizeH!=NULL)delete _trackSizeH;
}

void
PndTpcPatternRecoTask::SetParContainers() {

  std::cout<<"PndTpcPadResponseTask::SetParContainers"<<std::endl;
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
PndTpcPatternRecoTask::SetTrkFinderParameters(double RiemannScale,
			      double proxcut, double riproxcut, double szcut,
					      double planecut,
					      double TTproxcut, 
					      double TTplanecut, 
					      double TTszcut,
					      unsigned int minpointsforfit, 
					      unsigned int maxpointsforPR)
{
  fRiemannScale=RiemannScale;
  _proxcut=proxcut; _riproxcut=riproxcut; _szcut=szcut;
  _planecut=planecut; 
  _TTproxcut=TTproxcut; _TTplanecut=TTplanecut; _TTszcut=szcut;
  _minpoints=minpointsforfit;
  _maxpoints=maxpointsforPR;
}


InitStatus
PndTpcPatternRecoTask::Init()
{
//Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcPatternRecoTask::Init","RootManager not instantiated!");
      return kERROR;
    }

  // Get input collection
  _clusterArray=(TClonesArray*) ioman->GetObject(_clusterBranchName);

  if(_clusterArray==0)
    {
      Error("PndTpcPatternRecoTask::Init","Cluster-array not found!");
      return kERROR;
    }

  // create and register output array
  //_trackArray = new TClonesArray("GFTrack");
  //ioman->Register("TrackPreFit","GenFit",_trackArray,_persistence);

  // init TrackFinder
  _trackfinder= new PndTpcRiemannTrackFinder();
  _trackfinder->setSorting(_sorting);
  _trackfinder->setInteractionZ(0);
  _trackfinder->setSortingMode(_sortingmode);
  _trackfinder->setMinHitsForFit(_minpoints);
  _trackfinder->setScale(fRiemannScale);
  _trackfinder->setMaxNumHitsForPR(_maxpoints);
  
  // Hit-Track Correlators
  _trackfinder->addCorrelator(new PndTpcProximityHTCorrelator(_proxcut));
  _trackfinder->addCorrelator(new PndTpcRiProxHTCorrelator(_riproxcut));
  _trackfinder->addCorrelator(new PndTpcSzHTCorrelator(_szcut));
  _trackfinder->addCorrelator(new PndTpcRiemannHTCorrelator(_planecut));
  
  // Track-Track Correlators
  _trackfinder->addTTCorrelator(new PndTpcProximityTTCorrelator(_TTproxcut));
  _trackfinder->addTTCorrelator(new PndTpcRiemannTTCorrelator(_TTplanecut, _minpoints));
  _trackfinder->addTTCorrelator(new PndTpcSzTTCorrelator(_TTszcut));
  
   // init histos
  _multiplicityHisto=new TH1I("multipl","# track candidates",20,0,20);
  _trackSizeH=new TH1I("trksize","# hits in track",100,0,100);
  _trackPurityH=new TH1D("trkpurity","trackPurity",25,0,1);

   _nbins=100;
   _gpurity=new TGraph(_nbins);

  fnsectors= fpar->getPadPlane()->GetNSectors();
  for(unsigned int  isect=0;isect<fnsectors;++isect){
    fbuffermap[isect]=new std::vector<PndTpcCluster*>;
  }


  return kSUCCESS;
}

void
PndTpcPatternRecoTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcPatternRecoTask::Exec"<<std::endl;
  // Reset output Array
  // if(_trackArray==0) Fatal("PndTpcPatternReco::Exec)","No TrackArray");
  // _trackArray->Delete();
  
   std::cerr<<"Fetching clusters from cluster branch..."<<std::endl;
   unsigned int ncl=_clusterArray->GetEntries();
   for(unsigned int isect=0;isect<fnsectors;++isect)
     fbuffermap[isect]->reserve(ncl/fnsectors+10);
   for(unsigned int i=0; i<ncl; ++i){
     PndTpcCluster *cluster = (PndTpcCluster*)_clusterArray->At(i);
     unsigned int sectorId=cluster->sector();
     fbuffermap[sectorId]->push_back(cluster);
   }

   std::cerr << "Starting Pattern Reco..." << std::endl;
     
   // clean up riemannlist!
    for(int i=0; i<friemannlist.size(); ++i){
      if(friemannlist[i]!=NULL) delete friemannlist[i];
    }
    friemannlist.clear();

    /// PLAN: 
    /// 1) build several cluster buffer, sectorwise
    /// 2) run trackfinder over each clusterbuffer independently
    /// 3) put all found tracklets into one list
    /// 4) then do start merging

    std::vector<PndTpcRiemannTrack*> riemannTemp;

    // loop over sectors
    for(unsigned int isect=0;isect<fnsectors;++isect){
      std::cerr << "... building tracks in sector " << isect << std::endl;
      fcluster_buffer=fbuffermap[isect];
      _trackfinder->buildTracks(*fcluster_buffer,riemannTemp);
      if(_doClean) _trackfinder->cleanTracks(friemannlist, _szcut, _planecut);
      if(_doMerge) _trackfinder->mergeTracks(riemannTemp);

      // copy tracklets of this sector to global list
      unsigned int ntrklts=riemannTemp.size();
      friemannlist.reserve(friemannlist.size()+ntrklts);
      for(unsigned int it=0;it<ntrklts;++it){
	      friemannlist.push_back(riemannTemp[it]);
      }
      riemannTemp.clear();
    } // end loop over sectors

    if(_doClean) _trackfinder->cleanTracks(friemannlist, _szcut, _planecut);

    if(_doMerge && fnsectors>1) _trackfinder->mergeTracks(friemannlist);

    if(_doClean && fnsectors>1) _trackfinder->cleanTracks(friemannlist, _szcut, _planecut);
    
    std::cerr << "Pattern Reco finished. " 
	      << friemannlist.size() << " tracklets found." << std::endl;

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
    cout << "Found " << globalCol.nIDs() << " mcids in tracklets" << endl;
    cout << "Purity: "<< endl;
    map<double,double>::iterator it=goodCl.begin();
    unsigned int counter=0;
    while(it!=goodCl.end()){
      _gpurity->SetPoint(counter++, it->first, it->second /(double)ntr);
      cout << it->first << ":   " 
	   << it->second /(double)ntr*100. << "%" << endl;
      ++it;
    }// end loop over bins
  

  return;
}


void
PndTpcPatternRecoTask::WriteHistograms(const TString& filename) {
  TFile* file = new TFile(filename,"UPDATE");
  file->mkdir("PatternReco");
  file->cd("PatternReco");
  
  _gpurity->Write("gpurity");

  // _multiplicityHisto->Write();
  // delete _multiplicityHisto;
  // _multiplicityHisto=NULL;

  // _trackSizeH->Write();
  // delete _trackSizeH;
  // _trackSizeH=NULL;

  // _trackPurityH->Write();
  // delete _trackPurityH;
  // _trackPurityH=NULL;

  file->Close();
  delete file;
}
