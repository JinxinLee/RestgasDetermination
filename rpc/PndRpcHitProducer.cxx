// -------------------------------------------------------------------------
// -----                RpcTofHitProducer source file             -----
// -----                  Created 07/03/06  by V. Friese               -----
//                      modified 04/12/07 by A. Galoyan
//                   last modifications 08/06/08 by A. Galoyan           
// -------------------------------------------------------------------------


#include "TClonesArray.h"

#include "FairRootManager.h"
#include "FairRunAna.h"
#include "FairRuntimeDb.h"
#include "FairRun.h"

#include "PndRpcHitProducer.h"
#include "PndRpcHit.h"
#include "PndRpcPoint.h"
#include "TVector3.h"
#include "TRandom.h"




// -----   Default constructor   -------------------------------------------
PndRpcHitProducer::PndRpcHitProducer() :
  FairTask("Ideal TOF Hit Producer") { }
// -------------------------------------------------------------------------



// -----   Destructor   ----------------------------------------------------
PndRpcHitProducer::~PndRpcHitProducer() { }
// -------------------------------------------------------------------------



// -----   Public method Init   --------------------------------------------
InitStatus PndRpcHitProducer::Init() {

  // Get RootManager
 std::cout << " INITIALIZATION OF  Tof Hit Producer**"<< std::endl;
  FairRootManager* ioman = FairRootManager::Instance();
  if ( ! ioman ) {
    cout << "-E- PndRpcHitProducer::Init: "
	 << "RootManager not instantised!" << endl;
    return kFATAL;
  }

  // Get input array of Points
  fPointArray = (TClonesArray*) ioman->GetObject("PndRpcPoint");
  if ( ! fPointArray ) {
    cout << "-W- PndRpcHitProducer::Init "
	 << "No PndRpcPoint array!" << endl;
    return kERROR;
  }


  // Create and register output array
  fHitArray = new TClonesArray("PndRpcHit");
  ioman->Register("PndRpcHit", "PNDRPC", fHitArray, kTRUE);

  cout << "-I- PndRpcHitProducer: Intialisation successfull" << endl;
  return kSUCCESS;

}
// -------------------------------------------------------------------------



// -----   Public method Exec   --------------------------------------------
void PndRpcHitProducer::Exec(Option_t* opt) {

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
// sizes of strip ("cell") 
  Double_t length_cell=29.8;
  Double_t thickness = 1.0;
  Double_t width = 2.9; 
// velosity of time signal in strip 
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

    x_sm= point->GetX_sm();
    y_sm= point->GetY_sm();
    z_sm= point->GetZ_sm();

//  position error

    dx = x-x_sm;
    dy = y-y_sm;
    dz = z-z_sm;

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
  cout << "-I- PndRpcHitProducer: " << nPoints << " PndRpcPoints, "
       << nPoints << " Hits created." << endl;


}


ClassImp(PndRpcHitProducer)
