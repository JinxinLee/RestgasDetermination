//----------------------------------------------------------
//
// Description:
// Storage class for Digis in the DCH
//
//------------------
#include "PndDchDigi.h"
#include "PndDchPoint.h"

#include <iostream>
using std::cout;
using std::endl;

PndDchDigi::PndDchDigi() {
  fTimeStamp = 0.;
  fPlane = 0;
  fChamber = 0;
  fWire = 0; 
  fDriftTime = 0;
  fRefIndex = 0;
}

PndDchDigi::~PndDchDigi() {} 

PndDchDigi::PndDchDigi(Double_t timeStamp,
		       Int_t plane,
		       Int_t chamber,
		       Int_t wire,
		       Double_t driftTime,
		       Int_t refIndex) {
  fTimeStamp = timeStamp;
  fPlane = plane;
  fChamber = chamber;
  fWire = wire;
  fDriftTime = driftTime;
  fRefIndex = refIndex;
}

void PndDchDigi::Print(const Option_t*) const {
  cout << "PndDchDigi: " << endl;
  cout << " timeStamp   = "<<fTimeStamp;
  cout << " Chamber   = "<<fChamber;
  cout << " Plane = "<<fPlane;
  cout << " Wire  = "<<fWire;
  cout << " DriftTime   = "<<fDriftTime;
  cout<<" made from Point # "<<fRefIndex<<endl;
}
