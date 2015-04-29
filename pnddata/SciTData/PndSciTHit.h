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

  /** Default constructor (not for use) **/
  PndSciTHit();


  /** Standard constructor with all paramneters 
   *@param trackID Track ID
   *@param detID   Detector ID
   *@param detName Detector Name
   *@param pos     Position vector [cm]
   *@param dpos    Position error vector [cm]
   *@param charge  Charge measured in hit
   *@param NPixelHits Number of pixels fired for this event
   **/
  PndSciTHit(Int_t detID, TString detName,Double_t time,Double_t dt,
  	    TVector3& pos,TVector3& dpos,Int_t index, Double_t charge);

  //PndSciTHit(PndSciTHit& c);
  /** Destructor **/
  virtual ~PndSciTHit();


  /** Accessors **/

      /** Accessors * FairHit **
    Double_t GetDx()            const { return fDx;};
    Double_t GetDy()            const { return fDy;};
    Double_t GetDz()            const { return fDz;};
    Int_t    GetRefIndex()      const { return fRefIndex;}; Index of the corresponding entry in the MC output file
    void PositionError(TVector3& dpos) const;
    Int_t    GetDetectorID()   const { return fDetectorID;};
    Double_t GetX()             const { return fX;};
    Double_t GetY()             const { return fY;};
    Double_t GetZ()             const { return fZ;};
    void Position(TVector3& pos)       const;

    //** Accessors * FairtimeStamp **
    virtual Double_t GetTimeStamp()             const { return fTimeStamp; };
    virtual Double_t GetTimeStampError()     const { return fTimeStampError;};
    virtual FairLink GetEntryNr() const {return fEntryNr;}
*/

  TString 	GetDetName()const { return fDetName; }
  Double_t 	GetCharge(){return fCharge;};

// Following functions are redundant and should be removed. use the base funktion instead!!
  Double_t 	GetTime()	{return GetTimeStamp();};
  Double_t 	GetDt()		{return GetTimeStampError();};
  TVector3 GetPosition()	const { return TVector3(fX, fY, fZ);}
//-------------------------------------------------------------------------------

  /** Modifiers **/

/*
    //** Modifiers * FAIRHIT **
    void SetDx(Double_t dx) { fDx = dx; }
    void SetDy(Double_t dy) { fDy = dy; }
    void SetDz(Double_t dz) { fDz = dz; }
    void SetDxyz(Double_t dx, Double_t dy, Double_t dz);
    void SetPositionError(const TVector3& dpos);
    void SetRefIndex(Int_t index)   { fRefIndex = index; }
    void SetDetectorID(Int_t detID) { fDetectorID = detID; }
    void SetX(Double_t x) { fX = x; }
    void SetY(Double_t y) { fY = y; }
    void SetZ(Double_t z) { fZ = z; }
    void SetXYZ(Double_t x, Double_t y, Double_t z);
    void SetPosition(const TVector3& pos);

    //** Modifiers * FairTimeStamp **
    virtual void SetTimeStamp(Double_t t) { fTimeStamp = t; }
    virtual void SetTimeStampError(Double_t t) {fTimeStampError = t;}
    virtual void SetEntryNr(FairLink entry) {fEntryNr = entry;}
  */


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
