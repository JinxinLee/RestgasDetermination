// -------------------------------------------------------------------------
// -----                      PndDrcBarPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------


/**  PndDrcBarPoint.h
  **/

#ifndef PNDDRCBARPOINT_H
#define PNDDRCBARPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "CbmMCPoint.h"

class PndDrcBarPoint : public CbmMCPoint
{

 public:    

  /** Default constructor **/
  PndDrcBarPoint();


  /** Constructor with arguments **/
  PndDrcBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		 Double_t tof, Double_t length, Int_t pdgCode, Double_t angIn, Double_t thetaC, Int_t nBar, Int_t eventID);


 /** Copy constructor **/
  PndDrcBarPoint(const PndDrcBarPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndDrcBarPoint();


  /** Accessors **/
  Int_t    GetEventID()    const { return fEventID;};
  Int_t    GetPdgCode()    const { return fPdgCode;};
  Double_t GetAngIn()      const { return fAngIn; };
  Double_t GetThetaC()     const { return fThetaC; };
  Int_t    GetNBar()       const { return fNBar;};
 
  /** Modifiers ** **/
  void SetEventID(Int_t id)          { fEventID = id; }; 
  void SetPdgCode(Int_t id)          { fPdgCode = id; }; 
  void SetAngIn(Double_t ang)        { fAngIn= ang; }; 
  void SetThetaC(Double_t theta)     { fThetaC = theta; }; 
  void SetNBar(Int_t bar)            { fNBar = bar; };
  

  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t* opt) const ;

		   
 protected:

  Int_t fPdgCode;               // PDG code
  Double_t fAngIn;              // Incident Angle
  Double_t fThetaC;             // Cherenkov Angle
  Int_t fNBar;                  // Rod number
  Int_t fEventID;               // Event ID



  ClassDef(PndDrcBarPoint,2)

};

#endif
