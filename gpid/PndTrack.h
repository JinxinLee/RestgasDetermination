/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

#pragma once
#ifndef PND_TRACK_H
#define PND_TRACK_H

//PandaRoot & Root headers
#include "TObject.h"
/*
any pointer data member here has to be deleted by destroy func
*/
class PndTrack  : public TObject
{
 public:
  PndTrack();
  ~PndTrack();
  inline Double_t GetTpcDeDx()const { return fTpcDeDx;}
  inline Double_t GetMvdDeDx()const { return fMvdDeDx;}
  inline Double_t GetP()      const { return fP;}
  inline Double_t GetEmcE()   const { return fEmcE;}
  inline Double_t GetTof()    const { return fTof;}
  inline Double_t GetSpeed()  const { return fSpeed;}
  inline Double_t GetLen()    const { return fLen;}
  inline void SetTpcDeDx(Double_t f){ fTpcDeDx = f;}
  inline void SetMvdDeDx(Double_t f){ fMvdDeDx = f;}
  inline void SetP(Double_t f) 	    { fP = f;}
  inline void SetEmcE(Double_t f)   { fEmcE = f;}
  inline void SetTof(Double_t f)    { fTof = f;}
  inline void SetSpeed(Double_t f)  { fSpeed = f;}
  inline void SetLen(Double_t f)    { fLen = f;}

private:
  Double_t fTpcDeDx;
  Double_t fMvdDeDx;
  Double_t fP;
  Double_t fEmcE;
  Double_t fTof;
  Double_t fSpeed;
  Double_t fLen;
  void destroy();  
  ClassDef(PndTrack,1);
};
#endif
