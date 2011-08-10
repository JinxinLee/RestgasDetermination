//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndEvtDeconvTask
//      see PndEvtDeconvTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcEvtDeconvTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "TMath.h"
#include "TH1D.h"
#include "TH1I.h"
#include "TGraph.h"
#include "PndTpcCluster.h"
#include "PndTpcRiemannHit.h"

// Class Member definitions -----------

using namespace std;

#define DEBUG 0



PndTpcEvtDeconvTask::PndTpcEvtDeconvTask()
  : FairTask("PndTpc Event Deconvolution"), fPersistence(kFALSE), fRCut(0), fZCut(0) 
{
  fOutTrackBranchName = "RiemannTrackTagged";
  fTrackBranchName = "RiemannTrack";
}


PndTpcEvtDeconvTask::~PndTpcEvtDeconvTask()
{
  unsigned int n=fsurvivormap.size();
  for(unsigned int i=0;i<n;++i){
       fsurvivormap[i]->clear();
       delete fsurvivormap[i];
     } // end loop over survivor map
}

InitStatus
PndTpcEvtDeconvTask::Init()
{
  
  
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcEvtDeconvTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fTrackArray=(TClonesArray*) ioman->GetObject(fTrackBranchName);

  if(fTrackArray==0)
    {
      Error("PndTpcEvtDeconvTask::Init","riemmann-track-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fOutTrackArray = new TClonesArray("PndTpcRiemannTrack"); 
  ioman->Register(fOutTrackBranchName,"PndTpc",fOutTrackArray,fPersistence);
  
  // histograms
  hRetained= new TH1I("hRetained","Number of tracklets after target cut",
		      200,0,200);
  hFoundPhysics= new TH1I("hFoundPhysics","Found Physics tracklets",
			  10,0,10);
  hFoundIDs= new TH1I("hFoundID","Number of physics tracks with at least one found tracklet", 10,0,10);
  hPocaZ = new TH1D("hPocaZ","Poca to IP z",100,-10,10);
  hPocaR = new TH1D("hPocaR","Poca to IP r",100,0,15);
  
  // prepare efficiency / purity statistics
  fNSingleTrackPhys.resize(fnz);
  fNSingleTrackBkg.resize(fnz);
  fNFullEvent.resize(fnz);
  for(unsigned int i=0; i<fnz;++i){
    double zcut=((double)i+1.)*fdz;
    //cout << "Cut: " << zcut << endl;
    fZCuts.push_back(zcut);
    fsurvivormap.push_back(new vector<unsigned int>());
  }

  fevtcounter=0;

  return kSUCCESS;
}


void
PndTpcEvtDeconvTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcEvtDeconvTask::Exec"<<std::endl;
  fOutTrackArray->Delete();
  unsigned int foundPhysics=0;
  unsigned int presentPhysics=0;
  unsigned int retainedPileup=0;
  McIdCollection physicsID;
  unsigned int ntracks = fTrackArray->GetEntries(); 


  std::cout<< ntracks << " track candidates in mixed event" << std::endl;
  //loop over tracks
  for(unsigned int itr=0;  itr<ntracks; itr++) {
    if(DEBUG) std::cout<<"  ... processing Riemann track no. "<<itr<<std::endl;

    bool targetR=false;
    bool targetZ=false;
    bool endcapPene=false;
    PndTpcRiemannTrack* track = (PndTpcRiemannTrack*) (*fTrackArray)[itr];
    
    //check if we have an endcap penetration
    PndTpcCluster* lasthit= track->getLastHit()->cluster();
    PndTpcCluster* firsthit=track->getFirstHit()->cluster();
    // check if hit is neither at inner nor outer boundary
    if(lasthit->pos().Perp()<40.5 && lasthit->pos().Perp()>16.5){
      if(fabs(lasthit->pos().Z()-110)<fendcapcut)endcapPene=true;
    }
    if(firsthit->pos().Perp()<40.5 && firsthit->pos().Perp()>16.5){
      if(fabs(firsthit->pos().Z()-110)<fendcapcut)endcapPene=true;
    }
    
    
    // extrapolate track to z-axis and cut 
    // for the moment: presume physics ecent has been generated at t=0;
    
    TVector3 poca=track->pocaToIP();
    
    if(track->mcid().DominantID().mceventID()==0){
      hPocaZ->Fill(poca.Z());
      hPocaR->Fill(poca.Perp());
      ++presentPhysics;
      std::cout << "Physics tracklet: " << track->mcid().DominantID() << "  z="<< poca.Z() << "   r=" << poca.Perp() << std::endl;
    }
    
    if(poca.Perp()<fRCut)targetR=true;;
    
    if(targetR){
      unsigned int ns=fsurvivormap.size();
      for(unsigned int is=0;is<ns;++is){
	double cut=fZCuts[is];
	if(TMath::Abs(poca.Z())<cut)fsurvivormap[is]->push_back(itr);
      } // end loop over survivor map
    }
    // do actual cut:
    if(TMath::Abs(poca.Z())<fZCut)targetZ=true;
    
    // check if it survived the cuts -> store track
    if( (targetZ && targetR) || endcapPene ){
      if(endcapPene){
	cout << "found endcap penetrator" << endl;
      }
      if(targetZ && targetR){
	cout << "found target pointer" << endl;
      }
      // keep track of mcids
      if(track->mcid().DominantID().mceventID()==0){
	++foundPhysics;
	physicsID.AddID(track->mcid().DominantID());
      }
      else ++retainedPileup;
      
      new((*fOutTrackArray)[fOutTrackArray->GetEntries()]) PndTpcRiemannTrack(*track);
    }
  } //end loop over tracks
  
  // loop over survivors to build efficiencies and 
  // purities of the target pointing
  
  unsigned int nz=fsurvivormap.size();
  for(unsigned int iz=0;iz<nz;++iz){
    McIdCollection MyphysicsID;
    unsigned int nsurv=fsurvivormap[iz]->size();
    unsigned int nphys=0;
    unsigned int nbkg=0;
    // check how many physics tracklets
    for(unsigned int is=0;is<nsurv;++is){
      unsigned int trckid=fsurvivormap[iz]->at(is);
      PndTpcRiemannTrack* track = (PndTpcRiemannTrack*) (*fTrackArray)[trckid];
      if(track->mcid().DominantID().mceventID()!=0)++nbkg;
      // let's disregard secondaries from physics event
      else if(track->mcid().DominantID().mcsecID()==0){
	++nphys;
	MyphysicsID.AddID(track->mcid().DominantID());
      }
    }
    if(nphys>4){cerr << "EvtDeconvTask:: More than 4 primary tracks!!!" << endl;}

    // store numbers for this cut setting
    fNSingleTrackPhys[iz]+=nphys;
    fNSingleTrackBkg[iz]+=nbkg;
    if(MyphysicsID.nIDs()==fNExpectedTracks)fNFullEvent[iz]+=1;

    // clear survivor map
    fsurvivormap[iz]->clear();
  } // end loop over survivormap


  std::cout << "Retained "<< fOutTrackArray->GetEntries() << " tracklets in physics event (PR found "<<ntracks<<")" << std::endl;
  std::cout << foundPhysics << " true physics tracklets (PR found "<< presentPhysics <<")"<< std::endl;
  std::cout << "... containing "<< physicsID.nIDs() << " different MCtrackIDs" << std::endl; 

  hRetained->Fill( fOutTrackArray->GetEntries());
  hFoundPhysics->Fill(foundPhysics);
  hFoundIDs->Fill(physicsID.nIDs());

  std::cout << retainedPileup << " pileup tracklets" << std::endl<< std::endl;
  std::cout<<"PndTpcEvtDeconvTask::Exec ... finished"<<std::endl;

  ++fevtcounter;

  return;
}

void 
PndTpcEvtDeconvTask::FinishTask(){
TFile* file=FairRootManager::Instance()->GetOutFile();
 file->mkdir("PndTpcEvtDeconvTask");
 file->cd("PndTpcEvtDeconvTask");
 hRetained->Write();delete  hRetained;
 hFoundPhysics->Write();delete hFoundPhysics;
 hFoundIDs->Write();delete hFoundIDs;
 hPocaZ->Write();delete hPocaZ;
 hPocaR->Write();delete hPocaR;


 // evaluate statistics
 TGraph* geffpur=new TGraph(fNSingleTrackPhys.size());
 geffpur->SetTitle("Single track efficiency / purity");
 TGraph* gEvteff=new TGraph(fNSingleTrackPhys.size());
 gEvteff->SetTitle("Full event deconvolution efficiency");
 
 unsigned int ns=fsurvivormap.size();
 for(unsigned int is=0;is<ns;++is){
   double zcut=fZCuts[is];
   unsigned int nbkg=fNSingleTrackBkg[is];
   unsigned int nphys=fNSingleTrackPhys[is];

   TString name="hZCut";name+=is;
   TString title="nbkg nphys fullevents nevent - zcut=";title+=zcut;
   TH1D* h=new TH1D(name,title,4,0,4);
   h->Fill(0.5,nbkg);
   h->Fill(1.5,nphys);
   h->Fill(2.5,fNFullEvent[is]);
   h->Fill(3.5,fevtcounter);
   h->Write();

   double eff=(double)nphys/(double)(fNExpectedTracks*fevtcounter); // 
   double pur=1. -  (double)nbkg/(double)(nbkg+nphys);
   geffpur->SetPoint(is,eff,pur);  
   gEvteff->SetPoint(is,zcut,fNFullEvent[is]/(double)fevtcounter);
 }
 geffpur->Write("geffpur");
 gEvteff->Write("gEvteff");

}


  
  



ClassImp(PndTpcEvtDeconvTask);
