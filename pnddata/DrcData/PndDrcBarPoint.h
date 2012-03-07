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
#include "FairMCPoint.h"

class PndDrcBarPoint : public FairMCPoint
{

 public:    

  /** Default constructor **/
  PndDrcBarPoint();


  /** Constructor with arguments **/
  PndDrcBarPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
		 Double_t tof, Double_t length, Int_t pdgCode, Double_t angIn, 
		 Double_t thetaC, Int_t nBar, Int_t eventID, Double_t mass);


 /** Copy constructor **/
  PndDrcBarPoint(const PndDrcBarPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndDrcBarPoint();


  /** Accessors **/
  Int_t    GetPdgCode()    const { return fPdgCode;};
  Double_t GetAngIn()      const { return fAngIn; };
  Double_t GetThetaC()     const { return fThetaC; };
  Int_t    GetNBar()       const { return fNBar;};
  Int_t    GetDSide()      const { return (fNBar/1000);};
  Int_t    GetSide()       const { return ((fNBar%1000)/10);};
  Int_t    GetBox()        const { return (fNBar%10);};
  Double_t GetMass()       const { return fMass;};


  /** Modifiers ** **/
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
  Double_t fMass;               // Mass 

  ClassDef(PndDrcBarPoint,3)

};

#endif
