// -------------------------------------------------------------------------
// -----             PndDchCylinderHitProducer source file             -----
// -----                Created 26/03/08  by A. Wronska                -----
// -------------------------------------------------------------------------


// Pnd includes
#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"
#include "FairDetector.h"
#include "PndDchStructure.h"
#include "PndDchMapper.h"
#include "PndDchDrifter.h"
#include "PndDchCylinderHitProducer.h"
#include "PndDchCylinderHit.h"
#include "PndDchDigi.h"

// ROOT includes
#include "TClonesArray.h"
#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoNode.h"
#include "TGeoMatrix.h"
#include "TMath.h"
#include "TRandom.h"
#include "TVector2.h"
#include "TVector3.h"

// C++ includes
#include <iostream>
#include "math.h" 
using std::cout;
using std::endl;


// -----   Default constructor   -------------------------------------------
PndDchCylinderHitProducer::PndDchCylinderHitProducer() :
  FairTask("CylinderHit Producer for DCH") {
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDchCylinderHitProducer::~PndDchCylinderHitProducer() {
  if(fCylHitArray){
    fCylHitArray->Delete();
    delete fCylHitArray;
  }
}
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndDchCylinderHitProducer::Init() {

  std::cout << " INITIALIZATION OF PndDchCylinderHitProducer***************" << std::endl;

  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if( !ioman ) {
    std::cout << "-E- PndDchCylinderHitProducer::Init: "
	      << "RootManager not instantiated!" << std::endl;
    return kFATAL;
  }
  fDchMapper     = PndDchMapper::Instance();
  fDchDrifter    = PndDchDrifter::Instance();//"d2t.dat",1.);

  // Get input array
  fDigiArray = (TClonesArray*) ioman->GetObject("PndDchDigi");
  if( !fDigiArray ) {
    std::cout << "-W- PndDchCylinderHitProducer::Init: "
	      << "Array of PndDchDigis not found!" << std::endl;
    return kERROR;
  }
  // Get input array
  fPointArray = (TClonesArray*) ioman->GetObject("PndDchPoint");
  if( !fPointArray ) {
    std::cout << "-W- PndDchCylinderHitProducer::Init: "
	      << "Array of PndDchPoint not found!" << std::endl;
    return kERROR;
  }

  // Create and register output array
  fCylHitArray = new TClonesArray("PndDchCylinderHit");

  ioman->Register("PndDchCylinderHit","Dch",fCylHitArray,kTRUE);

  // Geometry loading
  TFile *infile = ioman->GetInFile();
  TGeoManager *geoMan = (TGeoManager*) infile->Get("FAIRGeom");
  fDchStructure = PndDchStructure::Instance(geoMan);

  std::cout << "-I- PndDchCylinderHitProducer: Intialization successfull" << std::endl;
  return kSUCCESS;
}
// -------------------------------------------------------------------------


// -----   Public method Exec   --------------------------------------------
void PndDchCylinderHitProducer::Exec(Option_t* opt) {

	// Reset output array
	if( !fCylHitArray ) Fatal("Exec", "No PndDchCylinderHit array");
	fCylHitArray->Clear();

	// Loop over Digis
	PndDchDigi* digi  = 0;
	Double_t xLoc, zGlo, dist, distErr, alpha, drTime, cellSize;
	Double_t cosalpha, tanalpha, sinalpha; 
	Int_t nDigis = fDigiArray->GetEntriesFast();
	Int_t wi, ch, pl, detID;
	TVector2 end1, end2, tmp;
	TVector3 planeHalfSizes;

	for(Int_t iDigi = 0; iDigi < nDigis; iDigi++){
	  digi  = (PndDchDigi*) fDigiArray->At(iDigi);
	  PndDchPoint* point = (PndDchPoint*)fPointArray->At(digi->GetRefIndex());
	  xLoc = zGlo = dist = distErr = alpha = drTime = cellSize = 0.0;
	  wi = ch = pl = detID = 0;
	  end1.Set(0.,0.);
	  end2.Set(0.,0.);
	  tmp.Set(-1e6,-1e6);
	  wi = digi->GetWire();
	  ch = digi->GetChamber();
	  pl = digi->GetPlane();
	  drTime = digi->GetDriftTime();
	  detID = fDchMapper->CalculateDetectorID(ch,pl);
	  cellSize = fDchStructure->GetCellSize(detID);
	  fDchDrifter->CalculateDistance(drTime,dist); // 0<=dist<=0.5
	  Double_t smearedDist = SmearDistance(dist*cellSize, cellSize/2.);
	  if(0==Accept(2.*smearedDist/cellSize))
	    continue;
	  distErr = Resolution(2.*dist);
	  dist = smearedDist;
	  alpha = fDchStructure->GetTransMatrix(detID)->GetRotation()->GetPhiRotation();
	  const Double_t* translation = fDchStructure->GetTransMatrix(detID)->GetTranslation();
	  zGlo = translation[2];
	  xLoc = fDchMapper->WirePosXlocal(wi,detID);
	  
	  Double_t globalPos[3];
	  Double_t globalWireDir[3];
	  
	  fDchMapper->TransformToGlobal(globalPos,globalWireDir,xLoc,detID);
	  alpha *= TMath::DegToRad();
	  cosalpha= TMath::Cos(alpha);
	  tanalpha= TMath::Tan(TMath::Pi()/2.-alpha);
	  sinalpha= TMath::Sin(alpha);
	  planeHalfSizes = fDchStructure->GetPlaneHalfSizes(detID) ;
	  //************* finding intersections with chamber's edges **********************
	  if (TMath::Abs(alpha) < 1e-9) { // x-wires
	    end1.Set(xLoc, planeHalfSizes.Y());
	      end2.Set(xLoc, -planeHalfSizes.Y());
	  } else if (TMath::Abs(alpha*TMath::RadToDeg()-90) < 1e-9) { // y-wires
	    end1.Set(-planeHalfSizes.X(),xLoc);
	    end2.Set(planeHalfSizes.X(),xLoc);
	    
	  } else {  //inclined wires
	    Double_t a = globalWireDir[1]/globalWireDir[0];
	    tmp.Set((-planeHalfSizes.Y()-globalPos[1])/a+globalPos[0], -planeHalfSizes.Y());
	    if (TMath::Abs(tmp.X())<planeHalfSizes.X()){
	      end1 = tmp;
	    }
	    tmp.Set((planeHalfSizes.Y()-globalPos[1])/a+globalPos[0], planeHalfSizes.Y());
	    if (TMath::Abs(tmp.X())<planeHalfSizes.X()){
	      if(end1.Mod()<1)
		end1 = tmp;
	      else 
		end2=tmp;
	    }
	    if (end2.Mod() < 1) {
	      tmp.Set(planeHalfSizes.X(),a*(planeHalfSizes.X()-globalPos[0])+globalPos[1]);
	      if (TMath::Abs(tmp.Y())<planeHalfSizes.Y()){
		if(end1.Mod()<1)
		  end1 = tmp;
		else 
		  end2=tmp;
	      }
	    }
	    if (end2.Mod() < 1) {
	      tmp.Set(-planeHalfSizes.X(),a*(-planeHalfSizes.X()-globalPos[0])+globalPos[1]);
	      if (TMath::Abs(tmp.Y())<planeHalfSizes.Y()){
		if(end1.Mod()<1) {
		  end1 = tmp;
		  cout<<"NO WAY TO BE HERE !!!!!!!..."<<endl;
		  exit(2);
		} else {
		  end2=tmp;
		}
	      }
	    }
	  } 
	  if(fVerbose>1){
	    digi->Print("");
	    cout<<"alpha = "<<alpha*TMath::RadToDeg()<<
	      " deg, zGlo = "<<zGlo<<
	      " cm, xLoc = "<<xLoc<<
	      " cm, dist = "<<dist<<
	      " plus/minus "<<distErr<<
	      " cm"<<endl;
	  }
	  
	  AddCylinderHit(iDigi, xLoc, zGlo, dist, distErr, alpha, end1, end2);
	}  // end of loop over Digis
}
// -------------------------------------------------------------------------


// -----   Private method AddHit   --------------------------------------------
PndDchCylinderHit* PndDchCylinderHitProducer::AddCylinderHit(Int_t digiidx, Double_t xLoc, Double_t zGlo,
							     Double_t dist, Double_t distErr,
							     Double_t alpha, TVector2 end1, TVector2 end2) const {
  TClonesArray& clref = *fCylHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndDchCylinderHit(digiidx, xLoc, zGlo,
					    dist, distErr, alpha, end1, end2);
}
// -------------------------------------------------------------------------


Double_t PndDchCylinderHitProducer::Efficiency(Double_t fracDist){
  if (fracDist >= 0   && fracDist <= 0.91) return 1.;
  if (fracDist > 0.91 && fracDist <= 0.94) return 0.9;
  if (fracDist > 0.94 && fracDist <= 0.97) return 0.89;
  if (fracDist > 0.97 && fracDist <= 0.99) return 0.76;
  return 0;
}

Bool_t PndDchCylinderHitProducer::Accept(Double_t fracDist){
  if (Efficiency(fracDist)>=0 && Efficiency(fracDist) >= gRandom->Uniform(1)) 
    return 1;
  else
    return 0;
}

Double_t PndDchCylinderHitProducer::Resolution(Double_t fracDist){
  Double_t res;
  res = (0.1568-0.227*fracDist+0.121*fracDist*fracDist);
  return res;
}

Double_t PndDchCylinderHitProducer::SmearDistance(Double_t dist, Double_t cellsize){
  Double_t newd;
  newd = gRandom->Gaus(dist,Resolution(dist/cellsize));
  return fabs(newd);
}




ClassImp(PndDchCylinderHitProducer)
