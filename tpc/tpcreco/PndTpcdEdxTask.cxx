//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcdEdxTask
//      see PndTpcdEdxTask.hh for details
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
#include "PndTpcdEdxTask.h"
#include <cmath>
// C/C++ Headers ----------------------
#include <iostream>
#include <assert.h>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "TClonesArray.h"
#include "Track.h"
#include "PndTpcCluster.h"
#include "PndTpcSPHit.h"


#include "GeaneTrackRep.h"
#include "AbsTrackRep.h"
#include "RecoHitFactory.h"
#include "Kalman.h"
#include "FitterExceptions.h"
#include "TH1D.h"
#include "TFile.h"
#include "TGeoTrack.h"
#include "TGeoManager.h"
#include "DetPlane.h"
#include "PndTpcdEdx.h"

#include "AbsRecoHit.h"
#include "TVector3.h"

#include <signal.h>
#include <stdlib.h>



// Class Member definitions -----------


PndTpcdEdxTask::PndTpcdEdxTask()
  : FairTask("dE/dx Task"), _persistence(kFALSE)
{
  _trackBranchName = "Track_out";
}


PndTpcdEdxTask::~PndTpcdEdxTask()
{
}

InitStatus
PndTpcdEdxTask::Init()
{
  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcdEdxTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _trackArray=(TClonesArray*) ioman->GetObject(_trackBranchName);
   
  if(_trackArray==0)
    {
      Error("PndTpcdEdxTask::Init","track-array not found!");
      return kERROR;
    }
   
  _dEdxOutArray = new TClonesArray("PndTpcdEdx");
  ioman->Register("dEdx","PndTpc",_dEdxOutArray,kTRUE);

  //TClonesArray* ar=(TClonesArray*) ioman->GetObject("PndTpcCluster");
  // if(ar==0){
  //   Error("PndTpcdEdxTask::Init","PndTpcCluster array not found");
  // }
   

  // setup histograms
  _distHist=new TH1D("distHist","Stepping lentghs",500,-3.,3.);
  _dirHist=new TH1D("dirHist","direction abundances",10,-2.,2.);
  
  return kSUCCESS;
}


void
PndTpcdEdxTask::Exec(Option_t* opt)
{
  std::cout<<"PndTpcdEdxTask::Exec"<<std::endl;
  _dEdxOutArray->Delete();

  Int_t ntracks=_trackArray->GetEntriesFast();
  
  if(ntracks>2000){
    std::cout<<"ntracks="<<ntracks<<" Evil Event! skipping"<<std::endl;
    return;
  }

  for(Int_t itr=0;itr<ntracks;++itr){
    std::cout<<"starting track"<<itr<<std::endl;
    Track* trk=(Track*)_trackArray->At(itr);
    std::cout<<"*** Number of clusters in track: "<<trk->getNumHits()<<" ***"<<std::endl;

    AbsTrackRep* absrep = trk->getCardinalRep();
    

    if(dynamic_cast<GeaneTrackRep*>(absrep) == NULL) {
      std::cerr<<"WRONG trackrep! Need GEANE to process ... skipping track"<<std::endl;
      return;
    }

    AbsTrackRep* theRep = absrep->clone();
	((GeaneTrackRep*)theRep)->setPropDir(0);
	
    std::vector<AbsRecoHit*> hits = trk->getHits();
    std::cout<<"\nstd::vector<AbsRecoHit*> hits has "<< hits.size()<<" entries"<<std::endl;
    
    std::vector<AbsRecoHit*>::iterator it;
    
    for(it = hits.begin(); it!=hits.end(); it++) {
      PndTpcSPHit* the_sphit = dynamic_cast<PndTpcSPHit*>(*it);
      //erase non-TPC hits
      if(the_sphit==NULL)
		hits.erase(it);
    }
	   
	PndTpcdEdx dedx;

	bool unsorted =false;
	
	for(int i=1;i<hits.size()-1;++i){
	  	  
	  TVector3 pos,mom;
	  
	  try{
		pos = theRep->getPos();
		mom = theRep->getMom();
	  }
	  catch(FitterException& e){
		e.what();
		return;
		//

	  }


	  TMatrixT<double> statePred(5,1);
	  TMatrixT<double> covPred(5,5);

	  bool backwards;
	  double dist;
	  DetPlane pl;
	  try{
		std::cout << "########## " << i << " of " << hits.size() << std::endl;
		pos.Print();
		hits.at(i+1)->getRawHitCoord().Print();
		pl = hits.at(i+1)->getDetPlane(theRep);
		pl.Print();
		TVector3 dir=pl.dist(pos);
		backwards = (dir*mom)<0;
		std::cout << "########## " << backwards << std::endl;
		dist = theRep->extrapolate(pl,statePred,covPred);
	  }
	  catch(FitterException& e){
		std::cerr << e.what() << std::endl;
		return;

		//
	  }
	  dedx.add(((PndTpcSPHit*)hits.at(i+1))->amp(),dist);
	  if(backwards) {
		dist*=-1;
		unsorted=true;
	  }
	  
	  _distHist->Fill(dist);

	  theRep->setState(statePred);
	  theRep->setCov(covPred);
	  theRep->setReferencePlane(pl);

	}
	_dirHist->Fill(unsorted);
	
	int size = _dEdxOutArray->GetEntriesFast();
	new((*_dEdxOutArray)[size]) PndTpcdEdx(dedx);
    return;
  }
}


void 
PndTpcdEdxTask::WriteHistograms(const TString& filename){
  TFile* file = new TFile(filename,"RECREATE");
  file->mkdir("DEDX");
  file->cd("DEDX");

  _distHist->Write();
  delete _distHist;

  _dirHist->Write();
  delete _dirHist;
   
  file->Close();
  delete file;
}



ClassImp(PndTpcdEdxTask)
