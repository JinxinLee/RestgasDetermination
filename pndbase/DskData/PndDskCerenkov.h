/**

  @class PndDskCerenkov

  @brief MC data storage class for a Cerenkov photon

  This calss will store information about a single Cerenkov photon. It will calculate the
  projected way on the fly (assuming that every track is done completely one after the other)

  @author Peter Koch
  @date   2008-03-28
  @since  2008-03-21

**/


#ifndef PNDDSKCERENKOV_H
#define PNDDSKCERENKOV_H

#include "FairMCPoint.h"
#include "TVector3.h"

// class PndDskCerenkov : public TObject
class PndDskCerenkov : public FairMCPoint
{

private:
  // information, that we want to store in root file
  // which is *not* already handled by FairMCPoint:
  /// Detector number that finally registered the photon.
  /// Starting with 1 at top left detector, clockwise. 0 is not hit any detector
  Int_t    fDetNumber;
  /// Detector type. This is a number from 0 to numOfDetectorTypes-1
  UShort_t fDetType;
  /// Energy of the Cerenkov
  Double_t fEnergy;
  /// Current Position of the Cerenkov
  TVector3 fCurPos;
  /// Projected Way, The total way of the photon projected on x-y-plane
  Double_t fPWay;
  /// PDG Code of the particle that emitted this Cerenkov photon
  Int_t    fMotherPdgCode;
  /// Primarys momentum when it hits on the disk
  TVector3 fPrimaryHitMomentum;


public:
  /// Default constructor
  PndDskCerenkov();
  /// Standard constructor
  /// @param trackID ID of the Cerenkov
  /// @param volID ID of the volume where the first hit is produced - for CbmMcPoint
  /// @param pos position of the creation point of this Cerenkov [cm]
  /// @param mom momentum of the Cerenkov when its created [GeV]
  /// @param tof time of flight since primary! vertex in [ns]
  /// @param length length of the track from creation point [cm]
  /// @param eLoss energy loss of the particle in the histpoint (always equal zero)
  /// @param energy Energy of the photon
  /// @param motherPdgCode PDG Code of particle that emitted this Cerenkov photon
  /// @param primaryHitMomentum Momentum of primary when it hits on the disk
  PndDskCerenkov(Int_t particleID, Int_t volID, TVector3 pos, TVector3 mom,
              Double_t tof, Double_t length, Double_t eLoss,
              Double_t energy,
              Int_t motherPdgCode, TVector3 primaryHitMomentum);
  /// Copy-constructor
  PndDskCerenkov(const PndDskCerenkov& point) { *this = point; }
  /// Standard destructor
  virtual ~PndDskCerenkov();

  /// increase projected way
  /// @param newPos current position of the Cerenkov
  void          CalcPWay(TVector3 newPos);
  /// Print some info about this Cerenkov
  virtual  void Print(const Option_t *opt) const { };

  /// Return detector number
  Int_t    GetDetNumber()          const { return fDetNumber; }
  /// Return Energy [GeV]
  Double_t GetEnergy()             const { return fEnergy; }
  /// Return projected way [cm]
  Double_t GetPWay()               const { return fPWay; }
  /// Return PDG Code of of particle that emitted this Cerenkov photon
  Int_t    GetMotherPdgCode()      const { return fMotherPdgCode; }
  /// Return Mothers inverse beta; later needed to calc Cerenkov-Angle
  TVector3 GetPrimaryHitMomentum() const { return fPrimaryHitMomentum; }

  /// Set final values when Cerenkov is detected
  void SetFinalValues(Int_t detNumber, UShort_t detType, Double_t tof, Double_t length);


ClassDef(PndDskCerenkov,1);

};


#endif // PNDDSKCERENKOV_H
