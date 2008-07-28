// -------------------------------------------------------------------------
// -----                     PndDrcHit header file                     -----
// -----               Created 11/10/06 by Annalisa Cecchi             -----
// -----                                                               -----
// -------------------------------------------------------------------------


#ifndef PNDDRCHIT_H
#define PNDDRCHIT_H

#include "CbmHit.h"

class PndDrcHit : public CbmHit {

public:    
  
  /** Default constructor **/
  PndDrcHit();

  PndDrcHit(Int_t copyNo, 
	    TVector3 pos, 
	    TVector3 dpos,
	    Double_t thetaC,
	    Double_t errThetaC); 
/*     : fPos(pos),  */
/*     fDpos(dpos), */
/*     fThetaC(thetaC), */
/*     fErrThetaC(errThetaC) */
/*     {}; */
    

/** Copy constructor **/
  PndDrcHit(const PndDrcHit& hit) { *this = hit; };

  /** Destructor **/
  virtual ~PndDrcHit();    
  
  /** Output to screen **/
  virtual void Print(const Option_t* opt ="") const;
  
  /** Modifiers **/
  
    
  /** Accessors **/
  virtual Double_t GetThetaC()  {return fThetaC;}
  virtual Double_t GetErrThetaC()  {return fErrThetaC;}

  virtual void Position(TVector3& pos) { pos.SetXYZ(fX,fY,fZ); }
  virtual void DPosition(TVector3& dpos) { dpos.SetXYZ(fDx,fDy,fDz); }

 protected:
  
  Int_t fCopyNo;
  TVector3 fPos, fDPos;
  Double_t fX,fY,fZ;
  Double_t fDx,fDy,fDz;
  Double_t fThetaC, fErrThetaC;

  ClassDef(PndDrcHit,1)
};

#endif //PNDDRCHIT_H
