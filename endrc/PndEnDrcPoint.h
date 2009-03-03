#ifndef PNDENDRCPOINT_H
#define PNDENDRCPOINT_H

////////////////////////////////////////////////////////////////////////////////
//
//  PndEnDrcPoint
//
//  MonteCarlo data storage class
//
//  created 2008 D. Glazier, K. Foehl Edinburgh
//
////////////////////////////////////////////////////////////////////////////////

#include "FairMCPoint.h"
#include "TVector3.h"

class PndEnDrcPoint : public FairMCPoint
{

public:
  /// Default constructor
  PndEnDrcPoint();

  ///Constructor with arguments
  /// @param trackID       Index of MCTrack
  /// @param detID         Detector ID
  /// @param pos           Point coordinates [cm]
  /// @param mom           Momentum of track at MCPoint [GeV]
  /// @param tof           Time since event start [ns]
  /// @param length        Track length since creation [cm]
  /// @param eLoss         Energy deposit [GeV]

  PndEnDrcPoint(
        Int_t    trackID,
        Int_t    detID,
        Int_t    pdgCode,
        TVector3 pos,
        TVector3 mom,
        Double_t tof,
        Double_t length,
        Double_t eLoss,
	Double_t PhotE,
	Double_t FPz,
	Double_t Angle);

  PndEnDrcPoint(const PndEnDrcPoint& point) { *this = point; };
  virtual ~PndEnDrcPoint();

  virtual void Print(const Option_t *opt) const;

  Int_t    GetPdgCode()    const { return fPdgCode; };
  Double_t GetPhotE() const { return fPhotE; };
  Double_t GetFPz() const { return fFPz; }; 
  Double_t GetAngle() const { return fAngle; };
protected:

  // information, that we want to store in root file
  // which is *not* already handled by FairMCPoint:
  Int_t    fPdgCode;
  Double_t fPhotE;
  Double_t fFPz;
  Double_t fAngle;
  ClassDef(PndEnDrcPoint,1)
};


#endif // PNDENDRCPOINT_H
