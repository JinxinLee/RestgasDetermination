// -------------------------------------------------------------------------
// -----                       PndRichPhoton source file                  -----
// -----               Created 01/11/14  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichPhoton.h"

// -----   Default constructor   -------------------------------------------
PndRichPhoton::PndRichPhoton()
{  
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndRichPhoton::PndRichPhoton(Double_t theta, 
                             Double_t phi, 
                             UInt_t tmask):
fTheta(theta),
fPhi(phi),
fTMask(tmask)
{    
}

// -----   Destructor   ----------------------------------------------------
PndRichPhoton::~PndRichPhoton(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndRichPhoton::Print(const Option_t* opt) const {
  cout << "RICH Photon hit: Time = " << fTime << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichPhoton)
