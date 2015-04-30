// --------------------------------------------------------------------------
// -----                    Forward tof header file                	-----
// -----                    created by A. Sanchez                  	-----
// -----                   modified by D. Steinschaden                 	-----
// -----                   last update    04.2015 		       	-----
// --------------------------------------------------------------------------


/** PndSciTHit.h
 *@author A. Sanchez <a.sanchez@gsi.de>
 **
 ** A hit in a tof wall station of SciT. In addition to the base class
 ** FairHit, it holds the number of the reconstructed hits (column and row).
 **/

#ifndef PNDSCITHIT_H
#define PNDSCITHIT_H


#include "TVector3.h"
#include "TString.h"
#include "FairHit.h"


class PndSciTHit : public FairHit
{

 public:

  //** Default constructor (not for use) **/
  PndSciTHit();


  PndSciTHit(Int_t detID, TString detName,Double_t time,Double_t dt,
  	    TVector3& pos,TVector3& dpos,Int_t index, Double_t charge);

  //PndSciTHit(PndSciTHit& c);
  //** Destructor **/
  virtual ~PndSciTHit();


  //** Accessors **/

  TString 	GetDetName()const { return fDetName; }
  Double_t 	GetCharge(){return fCharge;};

// Following functions are redundant and should be removed. 
//use the base funktion provided by FairHit instead!!

  Double_t 	GetTime()	{return GetTimeStamp();};
  Double_t 	GetDt()		{return GetTimeStampError();};
  TVector3 GetPosition()	const { return TVector3(fX, fY, fZ);}
//-------------------------------------------------------------------------------

  /** Modifiers **/

  void SetDetName(TString name){fDetName = name;};
  void SetCharge(Double_t charge){fCharge = charge;};
   

  /** Screen output **/
  virtual void Print(const Option_t* opt = 0) const;


 public:

  TString fDetName;  // Detector name
  Double_t fCharge;//,ftime,fdt; TS

  ClassDef(PndSciTHit,2);

};

#endif
