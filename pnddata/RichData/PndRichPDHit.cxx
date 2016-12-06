// -------------------------------------------------------------------------
// -----                       PndRichPDHit source file                -----
// -----               Created 01/11/14  by Beloborodov Konstantin     -----
// -----                                                               -----
// -----                                                               -----
// -------------------------------------------------------------------------
#include <iostream>

using std::cout;
using std::endl;

#include "PndRichPDHit.h"

// -----   Default constructor   -------------------------------------------
PndRichPDHit::PndRichPDHit()
   :FairHit(),
   fSensorId(-1),
   fTime(-999.),
   fTimeThreshold(-999.)
{  
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndRichPDHit::PndRichPDHit(Int_t detID, 
                           Int_t sensorId, 
                           TVector3& pos, 
                           TVector3& dpos,
                           Double_t time,
                           Double_t timeThreshold,
                           Int_t index)
:FairHit(detID, pos, dpos, index),  
   fSensorId (sensorId), 
   fTime       (time),
   fTimeThreshold (timeThreshold)
{    
}

// -----   Destructor   ----------------------------------------------------
PndRichPDHit::~PndRichPDHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndRichPDHit::Print(const Option_t* opt) const {
  cout << "RICH Photon hit: Time = " << fTime << endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichPDHit)
