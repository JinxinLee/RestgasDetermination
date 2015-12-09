// -------------------------------------------------------------------------
// -----                      PndRichBarPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------


/**  PndRichBarPoint.h
  **/

#ifndef PNDRICHBARPOINT_H
#define PNDRICHBARPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndRichBarPoint : public FairMCPoint
{

 public:    

  /** Default constructor **/
  PndRichBarPoint();


  /** Constructor with arguments **/
  PndRichBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
                  Double_t tof, Double_t length, Int_t pdgCode, 
                  Double_t thetaC, Int_t eventID, Double_t mass);


 /** Copy constructor **/
 // PndRichBarPoint(const PndRichBarPoint& point) { *this = point; }; // Not needed


  /** Destructor **/
  virtual ~PndRichBarPoint();


  /** Accessors **/
  Int_t    GetPdgCode()    const { return fPdgCode;};
  Double_t GetThetaC()     const { return fThetaC; };
  Double_t GetMass()       const { return fMass;};
  Int_t    GetTrackStatus()    const { return fTrackStatus;};


  /** Modifiers ** **/
  void SetPdgCode(Int_t id)          { fPdgCode = id; }; 
  void SetThetaC(Double_t theta)     { fThetaC = theta; }; 
  void SetTrackStatus(Int_t status) {  fTrackStatus = status; };
  

  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t* opt) const ;

		   
 protected:

  Int_t fPdgCode;               // PDG code
  Double_t fThetaC;             // Cherenkov Angle
  Double_t fMass;               // Mass 
  Int_t fTrackStatus;            

  ClassDef(PndRichBarPoint,3)

};

#endif
