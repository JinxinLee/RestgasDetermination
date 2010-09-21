// -------------------------------------------------------------------------
// -----                   PndDrcHitProducerIdeal source file               -----
// -----               Created 11/10/06  by Annalisa Cecchi            -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <fstream>
#include <iostream>
#include "stdio.h"

#include "PndGeoDrc.h"
#include "PndDrcHitProducerIdeal.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndDrcBarPoint.h"
#include "PndDrcHit.h"
#include "TVector3.h"
#include "TRandom.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairBaseParSet.h"
#include "FairGeoVolume.h"
#include "TString.h"
#include "FairGeoTransform.h"
#include "FairGeoVector.h"
#include "FairGeoMedium.h"
#include "FairGeoNode.h"
#include "PndGeoDrcPar.h"
#include "TFormula.h"
#include "TMath.h"
#include "TParticlePDG.h"
#include "TDatabasePDG.h"
#include "TPDGCode.h"
#include "TGeoManager.h"


using std::endl;
using std::cout;

// -----   Default constructor   -------------------------------------------
PndDrcHitProducerIdeal::PndDrcHitProducerIdeal() 
:FairTask("PndDrcHitProducerIdeal")
{
  fGeo = new PndGeoDrc();
  
}
// -------------------------------------------------------------------------

// -----   Standard constructor with verbosity level  -------------------------------------------

PndDrcHitProducerIdeal::PndDrcHitProducerIdeal(Int_t verbose = 0) 
  :FairTask("PndDrcHitProducerIdeal")
{
  fVerbose = verbose;  
  fGeo = new PndGeoDrc();
}
// -------------------------------------------------------------------------


// -----   Destructor   ----------------------------------------------------
PndDrcHitProducerIdeal::~PndDrcHitProducerIdeal()
{
  if (fGeo) delete fGeo;
  
}
// -------------------------------------------------------------------------


// -----   Initialization   -----------------------------------------------
// -------------------------------------------------------------------------
InitStatus PndDrcHitProducerIdeal::Init()
{
  cout << " ---------- INITIALIZATION ------------" << endl;
  nevents = 0;
  // Get RootManager
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndDrcHitProducerIdeal::Init: "
         << "RootManager not instantiated!" << endl;
    return kFATAL;
  }

  // Get input array
  fBarPointArray = (TClonesArray*) ioman->GetObject("DrcBarPoint");
   if ( ! fBarPointArray ) {
    cout << "-W- PndDrcHitProducerIdeal::Init: "
         << "No DrcBarPoint array!" << endl;
    return kERROR;
  }

   // Create and register output array
   fHitArray = new TClonesArray("PndDrcHit");
   ioman->Register("DrcHit","Drc",fHitArray, kTRUE);
    
   cout << "-I- PndDrcHitProducerIdeal: Intialization successfull" << endl;

   return kSUCCESS;
   
}


// -----   Execution of Task   ---------------------------------------------
// -------------------------------------------------------------------------
void PndDrcHitProducerIdeal::Exec(Option_t* option)
{
  if ( ! fHitArray ) Fatal("Exec", "No HitArray");
  fHitArray->Clear();


  PndDrcBarPoint* pt=NULL;
  nevents++;
   
  if (fVerbose > 0) {
    cout << " ----------------- DRC Hit Producer --------------------" << endl;
    cout <<" Number of input MC points in the bar: "<<fBarPointArray->GetEntries()<<endl;
  }

  // fNHits = 0;

  // Loop over PndDrcPoints
  for(Int_t j=0; j<fBarPointArray->GetEntriesFast(); j++) {
    if (fVerbose > 1) printf("\n\n=====> Event No. %d\n", nevents); 
    
    pt = (PndDrcBarPoint*)fBarPointArray->At(j);
  
    Double_t Px= pt->GetPx();
    Double_t Py= pt->GetPy();
    Double_t Pz= pt->GetPz();
    Double_t P = sqrt(Px*Px + Py*Py +Pz*Pz);
    Double_t mass = pt->GetMass();
    Double_t energy = TMath::Sqrt(P*P + mass*mass); 

    Double_t beta;
    if(energy != 0) {
      beta = P/energy;
    }
    else {
      beta = -1.;
      if (fVerbose >0) cout << "Beta not calculated " << endl; 
    }

    if (pt->GetThetaC() != -1. && beta > 1/1.47){  
    fDetectorID = pt->GetNBar();
  
    // calculate the center of the bars from teh detectorID
//    Int_t s = ((fDetectorID % 1000 - fDetectorID % 10)/10)-1;
    Int_t s = (fDetectorID /10)-1;// correction DD
    Int_t b =  (fDetectorID % 10)-1;

    Double_t rad = TMath::Pi()/180.;
//    Double_t lside = 18.7286709135483108; //side length (cm) 
//    Double_t r = 48.85;
//    Double_t phis = (s*22.5+270 )*rad; // 22.5 degrees are 360/16
//    corrections done 
    Double_t r = fGeo->radius();//radius in the middle of the bar =51.2cm
    Double_t hthick        =  fGeo->barHalfThick();//half thickness of the bars=1.7/2 cm
    Double_t rad_out = (r-hthick)/cos((22.5/2)*rad); // radius at corner - thickness ###
    Double_t lside   = 2*rad_out*sin((22.5/2)*rad);
    Double_t phis = (s*22.5+ 258.75 )*rad; // 22.5 degrees are 360/16
    Double_t Xs =  r*cos(phis);
    Double_t Ys = r*sin(phis);
    Double_t thts = phis- TMath::Pi()/2;
    Double_t Xb =  (5*lside/12 - (lside/6)*(b))*cos(thts);
    Double_t Yb =  (5*lside/12 - (lside/6)*(b))*sin(thts);
   
    Double_t fXHit = Xs+Xb;
    Double_t fYHit = -(Ys+Yb);
    Double_t fZHit = 0.;
 
    //  cout << "hit phi: "<< acos(fXHit/r) << endl;
    
    fPosHit.SetXYZ(fXHit,fYHit,fZHit);

    Double_t fDPosXHit = 0.5; //mm
    Double_t fDPosYHit = 0.5;
    Double_t fDPosZHit = 0.;
    fDPosHit.SetXYZ(fDPosXHit,fDPosYHit,fDPosZHit);

    fThetaC = gRandom->Gaus(pt->GetThetaC(),0.008);
    fErrThetaC = 0.; //rad

    fRefIndex = j;

   // PndMCTrack* tr = NULL;

    AddHit(fDetectorID, 
	   fPosHit, 
	   fDPosHit,
	   fThetaC,
	   fErrThetaC,
	   fRefIndex);
    }
  }
}




// -----   Add Hit to HitCollection   --------------------------------------
PndDrcHit* PndDrcHitProducerIdeal::AddHit(Int_t detID, 
				     TVector3 posHit, 
				     TVector3 dPosHit, 
				     Double_t thetaC,
				     Double_t errThetaC,
				     Int_t index){
  TClonesArray& clref = *fHitArray;
  Int_t size = clref.GetEntriesFast();
  return new(clref[size]) PndDrcHit(detID, 
				    posHit, 
				    dPosHit, 
				    thetaC,
				    errThetaC,
				    index);
}

// -------------------------------------------------------------------------


// -----   Finish Task   ---------------------------------------------------
void PndDrcHitProducerIdeal::Finish()
{

  cout << "-I- PndDrcHitProducerIdeal: Finish" << endl;
 }
// -------------------------------------------------------------------------


ClassImp(PndDrcHitProducerIdeal)
