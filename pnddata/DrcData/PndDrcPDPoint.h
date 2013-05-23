// -------------------------------------------------------------------------
// -----                      PndDrcPDPoint header file                   -----
// -----                  Created 09/04/08  by E. Fioravanti and A. Cecchi               -----
// -------------------------------------------------------------------------


/**  PndDrcPDPoint.h
  **/



#ifndef PNDDRCPDPOINT_H
#define PNDDRCPDPOINT_H

#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

class PndDrcPDPoint : public FairMCPoint
{

 public:    

  /** Default constructor **/
  PndDrcPDPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos      Point coordinates [cm]
   *@param mom      Momentum of track at MCPoint [GeV]
   *@param momAtEV  Momentum of track at entering the EV [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   **/
  PndDrcPDPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
  		 TVector3 momAtEV,
		 Double_t tof, Double_t length, Int_t pdgCode, Int_t eventID);


 /** Copy constructor **/
  PndDrcPDPoint(const PndDrcPDPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndDrcPDPoint();


  /** Accessors **/
  Int_t    GetPdgCode()    const{ return fPdgCode;    };
  TVector3 GetMomAtEV()	   const{ return fmomAtEV;    };
  Int_t    GetDetectorID() const{ return fDetectorID; };
  
 
  /** Modifiers **/
  void SetPdgCode(Int_t id)          { fPdgCode = id; }; 


  /** Output to screen **/
  //  virtual void Print(const Option_t* opt = 0) const = 0;
  virtual void Print(const Option_t* opt) const ;

		   
 protected:

  Int_t fPdgCode;
  TVector3 fmomAtEV;



  ClassDef(PndDrcPDPoint,3)

};

#endif
