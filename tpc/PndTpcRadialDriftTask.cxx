//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Implementation of class PndTpcRadialDriftTask
//      see PndTpcRadialDriftTask.hh for details
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
#include "PndTpcRadialDriftTask.h"

// C/C++ Headers ----------------------
#include <iostream>
#include <exception>

// Collaborating Class Headers --------
#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "PndTpcDigiPar.h"
#include "TClonesArray.h"
#include "PndTpcGas.h"
#include "TRandom.h"
#include "PndTpcPrimaryCluster.h"
#include "PndTpcDriftedElectron.h"
#include "PndTpcDevmapCyl.h"
#include "TVector3.h"
#include "QAPlotCollection.h"
#include "TMath.h"

#include <iostream>
#include <cmath>

using std::cout;
using std::exp;
using std::sqrt;

// Class Member definitions -----------


PndTpcRadialDriftTask::PndTpcRadialDriftTask()
  : CbmTask("TPC RadialDrift"), fpersistence(kFALSE), 
    fattach(kTRUE), fdiffuseL(kTRUE), fdiffuseT(kTRUE), fdistort(kFALSE), fphicut(kFALSE), finitialized(kFALSE),
    fqa(NULL),frGem(39.5),frInner(5), fLorentzAngle(0.175), fHV(10000)
{
  fprimBranchName = "PndTpcPrimaryCluster";
  fdevFile = "DevMap_29-06-07_E_and_B_new_fieldclass.dat"; //default
  //fgas= new PndTpcGas("NEON-90_CO2-10_B2_PRES1013.asc",400);
  //std::cout<<*fgas<<std::endl;
}


PndTpcRadialDriftTask::~PndTpcRadialDriftTask()
{
if(fdevmap!=NULL)delete fdevmap;
}

InitStatus
PndTpcRadialDriftTask::Init()
{ 

  finitialized=false;

  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcRadialDriftTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  fprimArray=(TClonesArray*) ioman->GetObject(fprimBranchName);
  
  if(fprimArray==0)
    {
      Error("PndTpcRadialDriftTask::Init","PrimaryElectron-array not found!");
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
  
 //TODO: get form GEOM!
  //fzGem=fpar->getZGem();
  fgas=fpar->getGas();
  fdiffuseL=fpar->getDiffuseL();
  fdiffuseT=fpar->getDiffuseT();
  fattach=fpar->getAttach();
  fmobility=fgas->VDrift()/fgas->E();
  
  ffrac=frGem/frInner;

//Instantiate deviation map
  if(fdistort){
    fdevmap = new PndTpcDevmapCyl(fdevFile,fgas->VDrift());
    if(!fdevmap->loaded()){
      Error("PndTpcRadialDriftTask::Init","Deviation Map not loaded! Switching off distortions.");
      fdistort=false;
    }
  }
  else fdevmap=NULL;

  finitialized=true;
  return kSUCCESS;
}

void 
PndTpcRadialDriftTask::SetParContainers() {

  std::cout<<"PndTpcRadialDriftTask::SetParContainers"<<std::endl;
  std::cout.flush();

  // Get run and runtime database
  CbmRunAna* run = CbmRunAna::Instance();
  if ( ! run ) Fatal("SetParContainers", "No analysis run");

  CbmRuntimeDb* db = run->GetRuntimeDb();
  if ( ! db ) Fatal("SetParContainers", "No runtime database");

  // Get PndTpc digitisation parameter container
  fpar= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! fpar ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



void
PndTpcRadialDriftTask::Exec(Option_t* opt)
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
      
      //diffusion
      // Diffusion formula from STAR Note #45 March 1992
      double r=pcl->pos().Perp();
      TVector3 radial=pcl->pos();
      radial.SetZ(0);
      double sigmaPhi=fgas->Dt()*TMath::Sqrt((frGem-r)*frGem/r);
      double d=gRandom->Gaus(0,sigmaPhi);
      TVector3 displace=radial.Orthogonal();
      displace.SetMag(d);
      displace.Rotate(gRandom->Uniform(0,TMath::TwoPi()),radial);
      
      // Lorentzdrift:
      double phishift=fLorentzAngle*TMath::Log(frGem/r);
      radial.RotateZ(phishift);
      radial.SetMag(frGem);
      radial+=displace;
     
      //calculate drift time
      double driftl=frGem-r;
      if(driftl<0)continue;
      //attachment
      if(fattach){
	if ( exp( -driftl * fgas->k() ) < gRandom->Uniform())
	  continue;
      }

      double dt=ffrac/(fHV*fmobility)*(frGem*frGem-r*r);


      Int_t size = fdriftedArray->GetEntriesFast();
      // for radial drift x=z; y=phi; 
      new((*fdriftedArray)[size]) PndTpcDriftedElectron(pcl->pos().Z()+radial.Z(),
						       radial.Phi(),
						       pcl->t()+dt,
						       pcl);
      //feeding the tracking Histograms with this electrons' data
      //FillHistograms(dx, dy, driftl);

    } // end loop over electrons

  } // end loop over clusters 
  std::cout<<fdriftedArray->GetEntriesFast()<<" electrons arriving at readout"
	   <<std::endl;
  return;
}

void PndTpcRadialDriftTask::FillHistograms(double x, double y, double dl) {
	if(fqa!=NULL){
		fxVariation->Fill(x);
		fyVariation->Fill(y);
		fxVarAndDriftL->Fill(x, dl);
		fyVarAndDriftL->Fill(y, dl);
	}
}

//WriteHistograms() has to be called once in the runDigi.C macro!
void PndTpcRadialDriftTask::WriteHistograms() {
  if(!finitialized)return;
  TFile* file=CbmRootManager::Instance()->GetOutFile();

  file->mkdir("PndTpcRadialDriftTask");
  file->cd("PndTpcRadialDriftTask");

  fxVariation->Write();
  delete fxVariation;
  fyVariation->Write();
  delete fyVariation;
  fxVarAndDriftL->Write();
  delete fxVarAndDriftL;
  fyVarAndDriftL->Write();
  delete fyVarAndDriftL;

 }

  
ClassImp(PndTpcRadialDriftTask)
