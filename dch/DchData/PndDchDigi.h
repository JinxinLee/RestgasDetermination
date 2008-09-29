// -------------------------------------------------------------------------
// -----                  PndDchCylinderHit header file                -----
// -----                Created 31.07.2007  by A. Wronska              -----
// -------------------------------------------------------------------------

/** \class PndDchDigi
 *  \author P.Hawranek, A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 31.07.2007
 *  \brief Storage class for digis from dch
 **/

#ifndef PNDDCHDIGI_H
#define PNDDCHDIGI_H

// ROOT includes
#include "TObject.h"


class PndDchPoint;

class PndDchDigi : public TObject  {
  
 public:
  
  /** Default constructor **/
  PndDchDigi();
  
  /** Constructor with arguments
    *  \param timeStamp
    *  \param plane - number of plane
    *  \param chamber - number of chamber 
    *  \param wire - number of wire
    *  \param driftTime - drift time to the wire
    *  \param refIndex - index of CbmMCPoint for this digi
    **/
  PndDchDigi(Double_t timeStamp,
	     Int_t plane,
	     Int_t chamber,
	     Int_t wire,
	     Double_t driftTime,
	     Int_t refIndex);
  
  /** Destructor **/
  virtual ~PndDchDigi();
  
  /** Accessors  **/
  Double_t GetTimeStamp() const { return fTimeStamp;}
  Int_t    GetPlane()     const { return fPlane;}
  Int_t    GetChamber()   const { return fChamber;}
  Int_t    GetWire()      const { return fWire;} 
  Double_t GetDriftTime() const { return fDriftTime;} 
  Int_t    GetRefIndex()  const { return fRefIndex;}
  
  /** Output to screen **/
  virtual void Print(const Option_t*) const;

 private:
  Double_t fTimeStamp;  ///< time stamp of the hit
  Int_t    fPlane;      ///< number of fired plane
  Int_t    fChamber;    ///< number of fired chamber
  Int_t    fWire;       ///< number of fired wire
  Double_t fDriftTime;  ///< measured drift time
  Int_t    fRefIndex;   ///< index of the corresponding MCpoint

  ClassDef(PndDchDigi,1);

};

#endif
