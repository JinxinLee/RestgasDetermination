// -------------------------------------------------------------------------
// -----                     PndDrcPDHit header file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDDRCPDHIT_H
#define PNDDRCPDHIT_H

#include "FairHit.h"

class PndDrcPDHit : public FairHit {

public:    
  
  /** Default constructor **/
  PndDrcPDHit();

  PndDrcPDHit(Int_t detID, 
	    TVector3& pos, 
	    TVector3& dpos,
	    Double_t time,
	    Double_t timeThreshold,
	    Int_t index); 
    

/** Copy constructor **/
  PndDrcPDHit(const PndDrcPDHit& hit) { *this = hit; };

  /** Destructor **/
  virtual ~PndDrcPDHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  
    
  /** Accessors **/
  virtual Double_t GetTime()  {return fTime;}
  virtual Double_t GetTimeThreshold()  {return fTimeThreshold;}
  virtual Int_t GetRefIndex()  {return fRefIndex;}
  TVector3 GetPosition()	  const { return TVector3(fX, fY, fZ);	  }
  
 protected:
  
//  TVector3 fmom;
  Double_t fTime,  fTimeThreshold;
   
  ClassDef(PndDrcPDHit,1)
};

#endif //PNDDRCPDHIT_H
