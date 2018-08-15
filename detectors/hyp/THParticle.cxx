/////////////////////////////////////////////////////////////
//
//  PndHypPoint
//
//  Geant point for Hyp detector
//
//
///////////////////////////////////////////////////////////////

#include <iostream>
using std::cout;
using std::endl;
#include "THParticle.h"

// -----   Default constructor   -------------------------------------------
THParticle::THParticle() : TParticle() {
  
  fEventID    = -1;
  fstatus =0;
  fIndex =0;
  
  fCalcMass   = 0.0;
  fA =fZ = fH = fpdgCode = 0;
  fPx = fPy = fPz =fVx = fVy = fVz = 0.;
 
}
// -------------------------------------------------------------------------



// -----   Standard constructor   ------------------------------------------
THParticle::THParticle(Int_t pdg,  Int_t status,Int_t evtID,
			 Int_t index, Double_t mass,
		       Int_t A,Int_t Z,Int_t H, 
			 const TLorentzVector &p,
            const TLorentzVector &v)
  : TParticle(pdg, status, 0,0,0,0,p,v)
  {
  
  fstatus = status; 
  fIndex = index; 
  fEventID = evtID;
  fpdgCode = pdg;
  fPx = p.Px();
  fPy = p.Py(); 
  fPz = p.Pz();
  fVx = v.X();
  fVy = v.Y(); 
  fVz = v.Z();
  
  fA      = A;
  fZ = Z;
  fH = H;
  fCalcMass   = mass;
  
 

  
 
}


// -----   Destructor   ----------------------------------------------------
THParticle::~THParticle() { }
// -------------------------------------------------------------------------

// -----   Public method Print   -------------------------------------------
void THParticle::Print(const Option_t* ) const {
  cout << "-I- THParticle: HYP fragment for track " << fpdgCode 
       << " in event " <<fEventID<< endl;
  
 
}
// -------------------------------------------------------------------------



ClassImp(THParticle)
