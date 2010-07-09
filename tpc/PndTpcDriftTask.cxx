//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcDriftTask
//      see PndTpcDriftTask.hh for details
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
#include "PndTpcDriftTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <exception>

// Collaborating Class Headers --------
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "PndTpcGas.h"
#include "TRandom.h"
#include "PndTpcPrimaryCluster.h"
#include "PndTpcDriftedElectron.h"
#include "PndTpcDevmapCyl.h"
#include "PndDetectorList.h"
#include "TVector3.h"
#include "QAPlotCollection.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::exp;
using std::sqrt;

// Class Member definitions -----------


PndTpcDriftTask::PndTpcDriftTask()
  : FairTask("TPC Drift"), fpersistence(kFALSE), 
    fattach(kTRUE), fdiffuseL(kTRUE), fdiffuseT(kTRUE), fdistort(kFALSE), 
    fphicut(kFALSE), finitialized(kFALSE),
    fqa(NULL)
{
  fprimBranchName = "PndTpcPrimaryCluster";
  //TODO: parameter management!!!!
  fdevFile = "DevMap_29-06-07_E_and_B_new_fieldclass.dat"; //default
}


PndTpcDriftTask::~PndTpcDriftTask()
{
if(fdevmap!=NULL)delete fdevmap;
}

InitStatus
PndTpcDriftTask::Init()
{ 

  finitialized=false;

  //Get ROOT Manager
  FairRootManager* ioman= FairRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcDriftTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fprimArray=(TClonesArray*) ioman->GetObject(fprimBranchName);
  
  if(fprimArray==0)
    {
      Error("PndTpcDriftTask::Init","PrimaryElectron-array not found!");
      return kERROR;
    }
  
  // create and register output array
  fdriftedArray = new TClonesArray("PndTpcDriftedElectron"); 
  ioman->Register("PndTpcDriftedElectron","PndTpc",fdriftedArray,fpersistence);

  // create histograms with x- and y-shifts
  // these will later be stored in a separate root file
  if(fqa!=NULL){
  fxVariation = fqa->getTH1D("xShifts",
			 "x-coordinate shifts from drifting", 200, -0.7, 0.7);
  fyVariation = fqa->getTH1D("yShifts",
			 "y-coordinate shifts from drifting" , 200, -0.7, 0.7);
  fxVarAndDriftL = fqa->getTH2D("xDrift_vs_DriftLength", 
			    "x-shifts vs. DriftLength",
			    200,-0.7,0.7,200,0,100);
  fyVarAndDriftL = fqa->getTH2D("yDrift_vs_DriftLength", 
			    "y-shift vs. DriftLength",
			    200,-0.7,0.7,200,0,100);
  }
  
  fzGem=fpar->getZGem();
  fgas=fpar->getGas();
  fdiffuseL=fpar->getDiffuseL();
  fdiffuseT=fpar->getDiffuseT();
  fattach=fpar->getAttach();
  
//Instantiate deviation map
  if(fdistort){
    fdevmap = new PndTpcDevmapCyl(fdevFile,fgas->VDrift());
    if(!fdevmap->loaded()) {
      Fatal("PndTpcDriftTask::Init","Deviation Map not loaded! Aborting...");
      return kERROR;
    }
  }
  
  finitialized=true;
  return kSUCCESS;
}

void 
PndTpcDriftTask::SetParContainers() {

  std::cout<<"PndTpcDriftTask::SetParContainers"<<std::endl;
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
PndTpcDriftTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(fdriftedArray==0) Fatal("PndTpcPrimCluster::Exec)","No DriftedElectronArray");
  fdriftedArray->Delete();

  //loop over incoming electrons
  Int_t nc=fprimArray->GetEntriesFast();
  for(int ic=0;ic<nc;++ic){
    PndTpcPrimaryCluster* pcl=(PndTpcPrimaryCluster*)fprimArray->At(ic);

    if(fphicut){
      double phi=pcl->pos().Phi();
      if(phi<fphimin || phi>fphimax)continue;
    }
    //create single electrons
    Int_t q=pcl->q();
    for(Int_t ie=0;ie<q;++ie){
      
      //calculate drift time
      double driftl=pcl->z()-fzGem;
      if(driftl<0)continue;
      //attachment
      if(fattach){
	if ( exp( -driftl * fgas->k() ) < gRandom->Uniform())
	  continue;
      }
      //diffusion
      double dx=0;double dy=0; double dt=0;
      dt=driftl/fgas->VDrift();
      if(fdiffuseL){
	double sigmal = fgas->Dl() * sqrt(driftl);
	dt+=gRandom->Gaus(0,sigmal)/fgas->VDrift();
      }
      if(fdiffuseT){
	double sigmat = fgas->Dt() * sqrt(driftl);
	dx+=gRandom->Gaus(0,sigmat);
	dy+=gRandom->Gaus(0,sigmat);
      }
      //drift distortions
      if(fdistort){
	double posX = pcl->x();
	double posY = pcl->y();
	double posZ = pcl->z();
	TVector3 value = fdevmap->value(TVector3(posX, posY, posZ));
	dx+=value.X();
	dy+=value.Y();
	dt+=value.Z() / fgas->VDrift();      
      }
      Int_t size = fdriftedArray->GetEntriesFast();
      PndTpcDriftedElectron* myElectron = new((*fdriftedArray)[size]) PndTpcDriftedElectron(pcl->x()+dx,
											   pcl->y()+dy,
											pcl->t()+dt,
											pcl);
      myElectron->SetLink(FairLink(fprimBranchName, ic));
      //feeding the tracking Histograms with this electrons' data
      FillHistograms(dx, dy, driftl);

    } // end loop over electrons

  } // end loop over clusters 
  std::cout<<fdriftedArray->GetEntriesFast()<<" electrons arriving at readout"
	   <<std::endl;
  return;
}

void PndTpcDriftTask::FillHistograms(double x, double y, double dl) {
	if(fqa!=NULL){
		fxVariation->Fill(x);
		fyVariation->Fill(y);
		fxVarAndDriftL->Fill(x, dl);
		fyVarAndDriftL->Fill(y, dl);
	}
}

//WriteHistograms() has to be called once in the runDigi.C macro!
void PndTpcDriftTask::WriteHistograms() {
  if(!finitialized)return;
  TFile* file=FairRootManager::Instance()->GetOutFile();

  file->mkdir("PndTpcDriftTask");
  file->cd("PndTpcDriftTask");

  fxVariation->Write();
  delete fxVariation;
  fyVariation->Write();
  delete fyVariation;
  fxVarAndDriftL->Write();
  delete fxVarAndDriftL;
  fyVarAndDriftL->Write();
  delete fyVarAndDriftL;

 }

  
ClassImp(PndTpcDriftTask)
