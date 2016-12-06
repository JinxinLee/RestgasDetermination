// --------------------------------------------------------------------------
// -----                     PndSciTpoint header file                  	-----
// -----                    created by A. Sanchez                  	-----
// -----                   modified by D. Steinschaden                 	-----
// -----                   last update    04.2015 		       	-----
// --------------------------------------------------------------------------

#ifndef PNDSCITPOINT_H
#define PNDSCITPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "FairMCPoint.h"
#include "TString.h"

class PndSciTPoint : public FairMCPoint 
{

 public:

  //** Default constructor **/
  PndSciTPoint();


  PndSciTPoint( Int_t eventID, Int_t trackID,
	      Int_t detectorID,TString detName,
	      TVector3 posin, 
	      TVector3 momin, 
	      TVector3 posout, 
	      TVector3 momout,
	      Double_t tof, 
	      Double_t length, 
	      Double_t eLoss
);
  
  //** Copy constructor **/
  PndSciTPoint(const PndSciTPoint& point) :
  fDetName(point.fDetName),
  fXout(point.fXout),
  fYout(point.fYout),
  fZout(point.fZout),
  fPxout(point.fPxout),
  fPyout(point.fPyout),
  fPzout(point.fPzout)
  { *this = point; };
  
  //** Destructor **/
  virtual ~PndSciTPoint();
  
  //** Accessors **/

  TString GetDetName() const { return fDetName;};

  Double_t GetXout()          const { return fXout; };
  Double_t GetYout()          const { return fYout; };
  Double_t GetZout()          const { return fZout; };
  Double_t GetPxout()         const { return fPxout; };
  Double_t GetPyout()         const { return fPyout; };
  Double_t GetPzout()         const { return fPzout; };
 
  void PositionOut(TVector3& pos)   { pos.SetXYZ(fXout, fYout, fZout); };
  void MomentumOut(TVector3& mom)   { mom.SetXYZ(fPxout, fPyout, fPzout); };
   
  /** Modifiers **/

  void SetPositionOut(const TVector3& pos);
  void SetMomentumOut(const TVector3& mom);  
   
  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


 protected:

  TString fDetName;

  Double_t  fXout, fYout, fZout;
  Double_t  fPxout,fPyout,fPzout;
  
  
  ClassDef(PndSciTPoint,4)

};


inline void PndSciTPoint::SetPositionOut(const TVector3& pos) {
  fXout = pos.X();
  fYout = pos.Y();
  fZout = pos.Z();
}

inline void PndSciTPoint::SetMomentumOut(const TVector3& mom) {
  fPxout = mom.Px();
  fPyout = mom.Py();
  fPzout = mom.Pz();
}



#endif
