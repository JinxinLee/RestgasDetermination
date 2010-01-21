//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcGenfitTestTask
//      see PndTpcGenfitTestTask.hh for details
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Christian Hoeppner     TUM         
//
//
//-----------------------------------------------------------

// Panda Headers ----------------------

// This Class' Header ------------------
#include "PndTpcGenfitTestTask.h"
#include <cmath>
// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h> 

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "GFTrack.h"
#include "PndTpcCluster.h"
#include "PndTpcDigiMapper.h"
#include "PndTpcFrontend.h"
#include "PndTpcSPHit.h"
#include "PndTpcTestPlanarHit.h"
#include "PndMCTrack.h"
#include "PndTpcPoint.h"


#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"

#include "GeaneTrackRep.h"
#include "GFAbsTrackRep.h"
#include "GFRecoHitFactory.h"
#include "GFKalman.h"
#include "GFException.h"
#include "GFTrack.h"
#include "TFile.h"
#include "GFDetPlane.h"
#include "GFAbsRecoHit.h"

#include "TVector3.h"

#include <signal.h>
#include <stdlib.h>



// Class Member definitions -----------


PndTpcGenfitTestTask::PndTpcGenfitTestTask()
  : FairTask("GENFIT test Task")
{

}


PndTpcGenfitTestTask::~PndTpcGenfitTestTask()
{
}

InitStatus
PndTpcGenfitTestTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();
  
  if(ioman==0)
    {
      Error("PndTpcGenfitTestTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _pointArray=(TClonesArray*) ioman->GetObject("PndTpcPoint");
  
  if(_pointArray==0)
    {
      Error("PndTpcGenfitTestTask::Init","Point-array not found!");
      return kERROR;
    }

  stMCT  = new TMatrixT<double>(5,1);
  covMCT = new TMatrixT<double>(5,5);
  stREC  = new TMatrixT<double>(5,1);
  covREC = new TMatrixT<double>(5,5);

  outfile = TFile::Open("out.root","RECREATE");
  outtree = new TTree("t","example output");
  
  outtree->Branch("stMCT","TMatrixT<double>",&stMCT);
  outtree->Branch("covMCT","TMatrixT<double>",&covMCT);
  outtree->Branch("stREC","TMatrixT<double>",&stREC);
  outtree->Branch("covREC","TMatrixT<double>",&covREC);
  outtree->Branch("chi2",&chi2,"chi2/D");
  outtree->Branch("nfail",&nfail,"nfail/I");
 

  _geanePro=new FairGeanePro();

  return kSUCCESS;
}


void
PndTpcGenfitTestTask::SetParContainers() {


  // Get run and runtime database
  FairRun* run = FairRun::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  FairRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

}



void
PndTpcGenfitTestTask::Exec(Option_t* opt)
{
  static int counter(0);
  std::cout << "PndTpcGenfitTestTask::Exec " << counter++ << std::endl;

  std::vector<PndTpcPoint*> pointlist;
  
  //loop over MC points and collect 
  for(int p=0; p<_pointArray->GetEntriesFast(); ++p) {
    int id = ((PndTpcPoint*)_pointArray->At(p))->GetTrackID();
    if(id==0) //only primary tracks
      pointlist.push_back((PndTpcPoint*)_pointArray->At(p));
  }
  
  std::vector<TVector3> vecList;
  double rho(0.);

  TVector3 mom;
  for(unsigned int i=0;i<pointlist.size();++i){
    //    pointlist.at(i)->Print();
    TVector3 vec;
    pointlist.at(i)->Position(vec);
    if(i==0) pointlist.at(i)->Momentum(mom);
    if(vec.Mag()<rho) return;//abort in this case
    rho = vec.Mag();
    vecList.push_back(vec);
  }

  std::vector<TVector3> vecList2;
  TVector3 before(0.,0.,0.);
  static const double minDist(2.);
  for(unsigned int i=0;i<vecList.size();++i){
    double dist = (vecList.at(i)-before).Mag();
    if(dist>minDist){
      before=vecList.at(i);
      vecList2.push_back(vecList.at(i));
    }
  }

  if(vecList2.size()<4.) return;
  
  
  GFDetPlane pl(vecList2.at(0)-1./mom.Mag()*mom,mom);

  GFDetPlane plRef(vecList2.at(0),vecList2.at(1)-vecList2.at(0));

  GFAbsTrackRep *rep = new GeaneTrackRep(_geanePro,
					 pl,
					 mom,
					 TVector3(1.,1.,1.),
					 TVector3(.2,.2,.2),
					 -1.,
					 211
					 );

  GFAbsTrackRep *repRef = new GeaneTrackRep(_geanePro,
					 plRef,
					 mom,
					 TVector3(1.,1.,1.),
					 TVector3(.2,.2,.2),
					 -1.,
					 211
					 );

  
  GFTrack trk(rep);
  for(unsigned int i=0;i<vecList2.size();++i){
    if(i==0) {
      trk.addHit(new PndTpcTestPlanarHit(plRef,0.01),2,i);
    }
    else trk.addHit(new PndTpcSPHit(vecList2.at(i),TVector3(0.1,0.1,0.2),true),3,i);
  }

  GFKalman k;
  try{
    k.processTrack(&trk);
  }
  catch(GFException e){
    std::cout << e.what() << std::endl;
    throw e;
  }
  
  *stMCT = repRef->getState();
  *covMCT = repRef->getCov();

  if (rep->getReferencePlane()!=plRef) return;//abort if extrap to last failed
  
  *stREC = rep->getState();
  *covREC = rep->getCov();


  chi2 = rep->getRedChiSqu();
  nfail = trk.getFailedHits();

  static int fillCounter(0);
  std::cout << "filling # " << fillCounter++ << std::endl;
  outtree->Fill();
}


void 
PndTpcGenfitTestTask::WriteTree(){
  std::cout << "$$$$$$$$$$$$$$$$$$$$ WriteTree" << std::endl;
  outfile->cd();
  outtree->Write();
  outfile->Close();
}



ClassImp(PndTpcGenfitTestTask)
