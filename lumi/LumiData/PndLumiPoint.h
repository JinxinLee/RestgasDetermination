#ifndef PNDLUMIPOINT_H
#define PNDLUMIPOINT_H

// ROOT includes
#include "TVector3.h"
#include "TString.h"

#include <iostream>

// pandaRoot includes
#include "FairMCPoint.h"

class PndLumiPoint : public FairMCPoint
{
 public:
    /** Default constructor **/
    PndLumiPoint();

    /** Constructor with arguments
	\param trackID       Index of MCTrack
	\param detID         Detector ID
	\param in	         Entry coordinates of track [cm]
	\param out           Exit Coordinates of track [cm]
	\param p_in          Momentum of the incoming track [GeV]
	\param p_out         Momentum of the outgoing track [GeV]
	\param tof           Time since event start [ns]
	\param length        Track length since creation [cm]
	\param eLoss         Energy deposited [GeV]
	\param detname       Name of detector hit [GeV]
	:FairMCPoint(Int_t trackID, Int_t detID, TVector3 pos, TVector3 mom,
	     Double_t tof, Double_t length, Double_t eLoss);
    **/
    PndLumiPoint(Int_t trackID, Int_t detID, TVector3 in, TVector3 p_in,
        		Double_t tof, Double_t length, Double_t eLoss,
        		TVector3 out, TVector3 p_out, TString detname);

    /** Copy constructor **/
    PndLumiPoint(const PndLumiPoint& point) { *this = point; };

    /** Destructor **/
    virtual ~PndLumiPoint();

    /** Accessors **/
    Double_t GetEntryX() const { return fX;}
    Double_t GetEntryY() const { return fY;}
    Double_t GetEntryZ() const { return fZ;}

    Double_t GetExitX() const { return fXOut;};
    Double_t GetExitY() const { return fYOut;};
    Double_t GetExitZ() const { return fZOut;};

    TVector3 GetEntryPoint() const { return TVector3(fX,fY,fZ); }
    TVector3 GetExitPoint()  const { return fPosOut;  }

    TVector3 GetMomentum() const { return TVector3(fPx,fPy,fPz );}

    TString  GetDetName() const { return fDetName;}

    /** Modifiers **/
    void SetEntryPoint(TVector3 newEntryPoint) { fX = newEntryPoint.X();fY = newEntryPoint.Y();fZ = newEntryPoint.Z();};
    void SetExitPoint(TVector3 newExitPoint)  {fPosOut = newExitPoint;  };

    void SetDetName(TString name) {fDetName = name;};

    /** Output to screen **/
    virtual void Print(const Option_t* opt) const;

 protected:
	 TVector3 fPosOut, fMomOut;
	 Double_t fXOut, fYOut, fZOut;
	 TString fDetName;

	 ClassDef(PndLumiPoint,3)
};
#endif
