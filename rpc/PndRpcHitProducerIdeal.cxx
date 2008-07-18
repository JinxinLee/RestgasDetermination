// -------------------------------------------------------------------------
// -----                RpcTofHitProducerIdeal source file             -----
// -----                  Created 07/03/06  by V. Friese               -----
//                      modified 04/12/07 by A. Galoyan
//                   last modifications 08/06/08 by A. Galoyan           
// -------------------------------------------------------------------------


#include "TClonesArray.h"

#include "CbmRootManager.h"
#include "CbmRunAna.h"
#include "CbmRuntimeDb.h"
#include "CbmRun.h"

#include "PndRpcHitProducerIdeal.h"
#include "PndRpcHit.h"
#include "PndRpcPoint.h"
#include "TVector3.h"
#include "TRandom.h"




// -----   Default constructor   -------------------------------------------
PndRpcHitProducerIdeal::PndRpcHitProducerIdeal() :
  CbmTask("Ideal TOF Hit Producer") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndRpcHitProducerIdeal::~PndRpcHitProducerIdeal() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndRpcHitProducerIdeal::Init() {

  // Get RootManager
 std::cout << " INITIALIZATION OF Ideal Tof Hit Producer**"<< std::endl;
  CbmRootManager* ioman = CbmRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndRpcHitProducerIdeal::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array of Points
  fPointArray = (TClonesArray*) ioman->GetObject("PndRpcPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndRpcHitProducerIdeal::Init: "
	 << "No PndRpcPoint array!" << endl;
    return kERROR;
  }


  // Create and register output array
  fHitArray = new TClonesArray("PndRpcHit");
  ioman->Register("PndRpcHit", "PNDRPC", fHitArray, kTRUE);

  cout << "-I- PndRpcHitProducerIdeal: Intialisation successfull" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndRpcHitProducerIdeal::Exec(Option_t* opt) {

  // Reset output array
  if ( ! fHitArray ) Fatal("Exec", "No PndRpcHitArray");

  fHitArray->Clear();

  // Declare some variables
  PndRpcPoint* point = NULL;
  Int_t detID   = 0;          // Detector ID
  Int_t trackID = 0;          // Track index
  Double_t x, y, z;           // Position
  Double_t x_sm, y_sm, z_sm;  // smeared position 
  Double_t dx, dy, dz;        //  Position error

  Double_t tof1 = 0.0;        // Time of flight to 1 end of gap
  Double_t tof2 = 0.0;        // Time of flight to 2 end of gap
     
  TVector3 pos(0.,0.,0.), dpos(0.,0.,0);        // Position and error vectors
  Int_t Gap=0;
  Int_t Cell=0;
  Int_t Module=0; 
  Double_t length_cell=29.8;
  Double_t thickness = 1.0;
  Double_t width = 2.9; 
  Double_t Velosity = 19.047;


  // Loop over PndRpcPoints
  Int_t nPoints = fPointArray->GetEntriesFast();
 
  for (Int_t iPoint=0; iPoint<nPoints; iPoint++) {
    point = (PndRpcPoint*) fPointArray->At(iPoint);
    if ( ! point) continue;

    // Detector ID
    detID = point->GetDetId();
    Gap = point->GetGap();
    Cell = point->GetCell();
    Module = point->GetModule();

  cout<<"gap" <<Gap<<endl;
  cout<<"cell"<<Cell<<endl;
  cout<<"module"<<Module<<endl;

    // MCTrack ID
    trackID = point->GetTrackID();

    // Determine hit position
    x  = point->GetX();
    y  = point->GetY();
    z  = point->GetZ();

    dx = 0.001;
    dy = dz = dx;

    x_sm= gRandom->Gaus(x,dx);
    y_sm= gRandom->Gaus(y,dy);
    z_sm= gRandom->Gaus(z,dz);

//  position error

  dx= x_sm - x;
  dy= y_sm - y;
  dz= z_sm - z;

// Time of flight 
    tof1 = point->GetTime1();
    tof2 = point->GetTime2();

    Double_t sig_t=0.1;    // sigma_tof=75 ps
//  Smear(tof1, and tof2 at  sig_t);
    
     Double_t t1_sm=gRandom->Gaus(tof1,sig_t);
     Double_t t2_sm=gRandom->Gaus(tof2,sig_t);

//   calculate tof_s  from t1_sm and t2_sm 
    Double_t tof_s = (t1_sm + t2_sm)/2.0-length_cell/Velosity/2.0;
     

    pos.SetXYZ(x_sm,y_sm,z_sm);
    dpos.SetXYZ(dx, dy, dz);

    // Create new hit
new((*fHitArray)[iPoint])PndRpcHit(detID,Gap,Cell,Module,pos,dpos,tof_s,iPoint);

  }   // Loop over MCPoints

  // Event summary
  cout << "-I- PndRpcHitProducerIdeal: " << nPoints << " PndRpcPoints, "
       << nPoints << " Hits created." << endl;


}


ClassImp(PndRpcHitProducerIdeal)
