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
  : CbmTask("TPC RadialDrift"), _persistence(kFALSE), 
    _attach(kTRUE), _diffuseL(kTRUE), _diffuseT(kTRUE), _distort(kFALSE), _phicut(kFALSE), _initialized(kFALSE),
    _qa(NULL),_rGem(39.5),_rInner(5), _LorentzAngle(0.175), _HV(10000)
{
  _primBranchName = "PndTpcPrimaryCluster";
  _devFile = "DevMap_29-06-07_E_and_B_new_fieldclass.dat"; //default
  //_gas= new PndTpcGas("NEON-90_CO2-10_B2_PRES1013.asc",400);
  //std::cout<<*_gas<<std::endl;
}


PndTpcRadialDriftTask::~PndTpcRadialDriftTask()
{
if(_devmap!=NULL)delete _devmap;
}

InitStatus
PndTpcRadialDriftTask::Init()
{ 

  _initialized=false;

  //Get ROOT Manager
  CbmRootManager* ioman= CbmRootManager::Instance();

  if(ioman==0)
    {
      Error("PndTpcRadialDriftTask::Init","RootManager not instantiated!");
      return kERROR;
    }
  
  // Get input collection
  _primArray=(TClonesArray*) ioman->GetObject(_primBranchName);
  
  if(_primArray==0)
    {
      Error("PndTpcRadialDriftTask::Init","PrimaryElectron-array not found!");
      return kERROR;
    }
  
  // create and register output array
  _driftedArray = new TClonesArray("PndTpcDriftedElectron"); 
  ioman->Register("PndTpcDriftedElectron","PndTpc",_driftedArray,_persistence);

  // create histograms with x- and y-shifts
  // these will later be stored in a separate root file
  if(_qa!=NULL){
  _xVariation = _qa->getTH1D("xShifts",
			 "x-coordinate shifts from drifting", 200, -0.7, 0.7);
  _yVariation = _qa->getTH1D("yShifts",
			 "y-coordinate shifts from drifting" , 200, -0.7, 0.7);
  _xVarAndDriftL = _qa->getTH2D("xDrift_vs_DriftLength", 
			    "x-shifts vs. DriftLength",
			    200,-0.7,0.7,200,0,100);
  _yVarAndDriftL = _qa->getTH2D("yDrift_vs_DriftLength", 
			    "y-shift vs. DriftLength",
			    200,-0.7,0.7,200,0,100);
  }
  
 //TODO: get form GEOM!
  //_zGem=_par->getZGem();
  _gas=_par->getGas();
  _diffuseL=_par->getDiffuseL();
  _diffuseT=_par->getDiffuseT();
  _attach=_par->getAttach();
  _mobility=_gas->VDrift()/_gas->E();
  
  _frac=_rGem/_rInner;

//Instantiate deviation map
  if(_distort){
    _devmap = new PndTpcDevmapCyl(_devFile,_gas->VDrift());
    if(!_devmap->loaded()){
      Error("PndTpcRadialDriftTask::Init","Deviation Map not loaded! Switching off distortions.");
      _distort=false;
    }
  }
  else _devmap=NULL;

  _initialized=true;
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
  _par= (PndTpcDigiPar*) db->getContainer("PndTpcDigiPar");
  if (! _par ) Fatal("SetParContainers", "PndTpcDigiPar not found");
}



void
PndTpcRadialDriftTask::Exec(Option_t* opt)
{
  // Reset output Array
  if(_driftedArray==0) Fatal("PndTpcPrimCluster::Exec)","No DriftedElectronArray");
  _driftedArray->Delete();

  //loop over incoming electrons
  Int_t nc=_primArray->GetEntriesFast();
  for(int ic=0;ic<nc;++ic){
    PndTpcPrimaryCluster* pcl=(PndTpcPrimaryCluster*)_primArray->At(ic);

    if(_phicut){
      double phi=pcl->pos().Phi();
      if(phi<_phimin || phi>_phimax)continue;
    }

    //create single electrons
    Int_t q=pcl->q();
    for(Int_t ie=0;ie<q;++ie){
      
      //diffusion
      // Diffusion formula from STAR Note #45 March 1992
      double r=pcl->pos().Perp();
      TVector3 radial=pcl->pos();
      radial.SetZ(0);
      double sigmaPhi=_gas->Dt()*TMath::Sqrt((_rGem-r)*_rGem/r);
      double d=gRandom->Gaus(0,sigmaPhi);
      TVector3 displace=radial.Orthogonal();
      displace.SetMag(d);
      displace.Rotate(gRandom->Uniform(0,TMath::TwoPi()),radial);
      
      // Lorentzdrift:
      double phishift=_LorentzAngle*TMath::Log(_rGem/r);
      radial.RotateZ(phishift);
      radial.SetMag(_rGem);
      radial+=displace;
     
      //calculate drift time
      double driftl=_rGem-r;
      if(driftl<0)continue;
      //attachment
      if(_attach){
	if ( exp( -driftl * _gas->k() ) < gRandom->Uniform())
	  continue;
      }

      double dt=_frac/(_HV*_mobility)*(_rGem*_rGem-r*r);


      Int_t size = _driftedArray->GetEntriesFast();
      // for radial drift x=z; y=phi; 
      new((*_driftedArray)[size]) PndTpcDriftedElectron(pcl->pos().Z()+radial.Z(),
						       radial.Phi(),
						       pcl->t()+dt,
						       pcl);
      //feeding the tracking Histograms with this electrons' data
      //FillHistograms(dx, dy, driftl);

    } // end loop over electrons

  } // end loop over clusters 
  std::cout<<_driftedArray->GetEntriesFast()<<" electrons arriving at readout"
	   <<std::endl;
  return;
}

void PndTpcRadialDriftTask::FillHistograms(double x, double y, double dl) {
	if(_qa!=NULL){
		_xVariation->Fill(x);
		_yVariation->Fill(y);
		_xVarAndDriftL->Fill(x, dl);
		_yVarAndDriftL->Fill(y, dl);
	}
}

//WriteHistograms() has to be called once in the runDigi.C macro!
void PndTpcRadialDriftTask::WriteHistograms() {
  if(!_initialized)return;
  TFile* file=CbmRootManager::Instance()->GetOutFile();

  file->mkdir("PndTpcRadialDriftTask");
  file->cd("PndTpcRadialDriftTask");

  _xVariation->Write();
  delete _xVariation;
  _yVariation->Write();
  delete _yVariation;
  _xVarAndDriftL->Write();
  delete _xVarAndDriftL;
  _yVarAndDriftL->Write();
  delete _yVarAndDriftL;

 }

  
ClassImp(PndTpcRadialDriftTask)
