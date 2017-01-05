// -------------------------------------------------------------------------
// -----                     PndRichPDHit header file                  -----
// -----               Created 01/11/14 by Konstantin Beloborodov      -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDRICHPDHIT_H
#define PNDRICHPDHIT_H

#include "FairHit.h"

class PndRichPDHit : public FairHit {

public:    
  
  /** Default constructor **/
  PndRichPDHit();

  PndRichPDHit(Int_t detID, 
               Int_t sensorId, 
               TVector3& pos, 
               TVector3& dpos,
               Double_t time,
               Double_t timeThreshold,
               Int_t index); 
   

/** Copy constructor **/
  PndRichPDHit(const PndRichPDHit& hit) : FairHit(hit),
  fSensorId(hit.fSensorId),
  fTime(hit.fTime),
  fTimeThreshold(hit.fTimeThreshold)
  { *this = hit; };

  /** Destructor **/
  virtual ~PndRichPDHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  
    
  /** Accessors **/
  virtual Double_t GetTime()  {return fTime;}
  virtual Double_t GetTimeThreshold()  {return fTimeThreshold;}
  virtual Int_t GetRefIndex()  {return fRefIndex;}
  TVector3 GetPosition()	  const { return TVector3(fX, fY, fZ);	  }
  Int_t GetSensorId()  {return fSensorId;}

 protected:
  
  Int_t fSensorId;
  Double_t fTime,  fTimeThreshold;
   
  ClassDef(PndRichPDHit,1)
};

#endif //PNDRICHPDHIT_H
