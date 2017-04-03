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
   :FairTimeStamp(0),
   fSensorId(-1),
   fTime(-999.),
   fTimeThreshold(-999.)
{  
   SetTimeStamp(0);
}
// -------------------------------------------------------------------------


// -----   Standard constructor   ------------------------------------------

PndRichPDHit::PndRichPDHit(Int_t index,
                           Int_t detID, 
                           Int_t sensorId, 
                           TVector3& pos, 
                           TVector3& dpos,
                           Double_t time,
                           Double_t timeThreshold,
                           Double_t timeStamp)
:FairTimeStamp(timeStamp),
   fSensorId (sensorId),
   fTime       (time),
   fTimeThreshold (timeThreshold)
{
   AddIndex(index);
   fX = pos.X();
   fY = pos.Y();
   fZ = pos.Z();
}

PndRichPDHit::PndRichPDHit(std::vector<Int_t> index,
                           Int_t detID, 
                           Int_t sensorId, 
                           TVector3& pos, 
                           TVector3& dpos,
                           Double_t time,
                           Double_t timeThreshold,
                           Double_t timeStamp)
:FairTimeStamp(timeStamp),
   fSensorId (sensorId),
   fTime       (time),
   fTimeThreshold (timeThreshold)
{
   AddIndex(index);
   fX = pos.X();
   fY = pos.Y();
   fZ = pos.Z();
}

// -----   Destructor   ----------------------------------------------------
PndRichPDHit::~PndRichPDHit(){}
// -------------------------------------------------------------------------


// -----   Public method Print   -------------------------------------------
void PndRichPDHit::Print(const Option_t* opt) const {
  cout << "RICH Photon hit: Time = " << fTime << " opt="<<opt<< endl;
}
// -------------------------------------------------------------------------

ClassImp(PndRichPDHit)
