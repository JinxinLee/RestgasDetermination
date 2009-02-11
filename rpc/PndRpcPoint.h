#ifndef PNDRPCPOINT_H
#define PNDRPCPOINT_H


#include "TObject.h"
#include "TVector3.h"
#include "FairMCPoint.h"

using namespace std;



class PndRpcPoint : public FairMCPoint 
{

 public:

  /** Default constructor **/
  PndRpcPoint();


  /** Constructor with arguments
   *@param trackID  Index of MCTrack
   *@param detID    Detector ID
   *@param pos     Coordinates at entrance to active volume [cm]
   *@param mom    Momentum of track at entrance [GeV]
   *@param tof      Time since event start [ns]
   *@param length   Track length since creation [cm]
   *@param eLoss    Energy deposit [GeV]
   *@param pos_sm   Smeared Coordinates  
   **/
 
  PndRpcPoint(Int_t trackID, Int_t detID,  TVector3 pos, 
	      TVector3 mom,  Double_t tof, 
              Double_t length, Double_t eLoss, Int_t eventID, 
              Double_t t1, Double_t t2, TVector3 pos_sm);


  /** Copy constructor **/
  PndRpcPoint(const PndRpcPoint& point) { *this = point; };


  /** Destructor **/
  virtual ~PndRpcPoint();


  /** Accessors **/
  Double_t GetX()  const { return fX; }
  Double_t GetY()  const { return fY; }
  Double_t GetZ()  const { return fZ; }

  Double_t GetX_sm()  const { return fX_sm; }
  Double_t GetY_sm()  const { return fY_sm; }
  Double_t GetZ_sm()  const { return fZ_sm; }

  Double_t GetPx() const { return fPx; }
  Double_t GetPy() const { return fPy; }
  Double_t GetPz() const { return fPz; }

  Double_t GetTime() const { return fTime; }
  Double_t GetLength() const { return  fLength; }
  Double_t GetTime1() const { return ft1; }
  Double_t GetTime2() const { return ft2; }
  
  Int_t GetTrackID()   const { return ftrackID; }
  Int_t GetEventID()   const { return fEventID; }
  Int_t GetDetId()     const { return fDetectorID; }



  /** Output to screen **/
  virtual void Print(const Option_t* opt) const;


  /** Methods for getting the geometric coordinates 
(gap/cell/module/region)  **/

  Int_t GetGap() const    {return (fDetectorID & 15);};
  Int_t GetCell() const   {return ((fDetectorID>>4) & 1023);};
  Int_t GetModule() const {return ((fDetectorID>>14) & 1023);};
  Int_t GetRegion() const {return fDetectorID>>24;};

  //  private:

  //Int_t fGap;      // Gap number
  //Int_t fCell;     // Cell number
  //Int_t fModule;   // Module number
  //Int_t fRegion;   // Region number


 protected:
  Double_t fX,  fY,  fZ;               //  position
  Double_t fX_sm,  fY_sm,  fZ_sm;      //  smeared position
  Double_t fPx, fPy, fPz;              // momentum
  Double_t ft1, ft2;                   // tof+strip propagation times  
  Int_t  fDetectorID;                   // det number
  Int_t ftrackID;               
  Double_t fTime;
  Double_t fLength;
  Int_t fEventID;
  ClassDef(PndRpcPoint,1)

};


#endif
