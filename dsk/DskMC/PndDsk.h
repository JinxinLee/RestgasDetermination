/**

  @class PndDsk

  @brief Interface to virtual monte carlo

  This is the interface to the virtual monte carlo.
  Its responsible to create the geometry (ConstructGeometry)
  and to register the activities of Cerekov photons in our media
  (ProcessHits)

  @author Peter Koch
  @date   2008-03-28
  @since  2008-10-23

**/


#ifndef PNDDSK_H
#define PNDDSK_H

#include "TClonesArray.h"
#include "TVector3.h"
#include "FairDetector.h"
class FairVolume;
class PndDskCerenkov;


class PndDsk : public FairDetector
{

private:
  /// pointer to Cerenkov photon array
  TClonesArray *fDskCerenkovCollection;

  /// The particle-track ID
  Int_t    fTrackID;
  /// PDG Code of the particle that produced the hit
  Int_t    fPdgCode;
  /// The ID of the volume that got hit
  Int_t    fVolID;
  /// Point where hit was produced [cm]
  TVector3 fPos;
  /// Momentum in the point where hit was produced [GeV]
  TVector3 fMom;
  /// Total time of flight, including all mothers, till this hit point. [ns]
  Double_t fTof;
  /// Length of the track from creation point till this hit point [cm]
  Double_t fLength;
  /// Energy loss of the particle  in this hit point [GeV]
  Double_t fELoss;
  /// Energy of the particle in this hit point [GeV]
  Double_t fEnergy;
  /// Number of the detector that was hit, if a detector was hit.
  Int_t    fDetNumber;
  /// The type of the detector
  UShort_t fDetType;

  /// For Cerenkov: Hit point of primary particle on the disk \n
  TVector3 fPrimaryHitPoint;
  /// For Cerenkov: Momentum of primary when it hits the disk for the first time \n
  TVector3 fPrimaryHitMomentum;
  /// A Boolean, so we wont store primary values inside the disk, but just when entering
  Bool_t   fPrimaryIsInside;

  /// total number of mirrors per edge
  Int_t    fMirrorsPerEdge;
  /// number of mirror types
  Int_t    fMirrorTypes;

public:
  /// default constructor
  PndDsk();
  /// @brief standard constructor
  /// @param name name of the detector
  /// @param active kTRUE for active detectors, ProcessHits() will be called \n
  ///               kFALSE for inactive detectors
  PndDsk(const char *name, Bool_t active);
  /// default destructor
  virtual ~PndDsk();

  /// @brief Adds Cerenkov to the list
  /// @param trackID ID of cerenkov. (Dont mix up with TGeoTracks trackID!)
  /// @param volID ID of the volume where the first hit is produced - for CbmMcPoint
  /// @param pos position of the Cerenkov [cm]
  /// @param mom momentum of the particle in the first hitpoint [GeV] - for FairMCPoint
  /// @param tof time of flight since primary! vertex in [ns]
  /// @param length length of the track from creation point [cm]
  /// @param eLoss energy loss of the particle in the histpoint - for FairMCPoint
  /// @param energy Energy of the photon
  /// @param motherPdgCode PDG Code of particle that emiited this Cerenkov photon \n
  ///        This is not necessarily the primary particle!
  /// @param primaryHitMomentum Momentum  of particle that hit the disk in the moment when it hits the disk \n
  PndDskCerenkov *AddCerenkov(Int_t trackID, Int_t volID, TVector3 pos, TVector3 mom,
                        Double_t tof, Double_t length, Double_t eLoss,
                        Double_t energy, Int_t motherPdgCode,
                        TVector3 primaryHitMomentum);

  /// Construct the detector using the definitions in the geometry file
  virtual void          ConstructGeometry();
  virtual void          ConstructOpGeometry();
  /// Things that need to be done at the end of an event
  virtual void          EndOfEvent();
  /// @brief Returns a hit collection
  /// @param iColl and integer representing the hit collection. This value is arbitary in this file
  ///              at the moment it is: \n
  ///              0: *fDskCerenkov;
  virtual TClonesArray *GetCollection(Int_t iColl) const;
  /// @brief React on a hit in an activ volume
  /// @param vol pointer to the volume that got hit \n
  /// Here most work is done. It has to be decided to which Collection to point has to be added
  /// and all values for the Collections are calculated here.
  virtual Bool_t        ProcessHits(FairVolume *vol = 0);
  /// Registers the hit collections to the ROOT manager
  virtual void          Register();
  /// Clear hit-collections
  virtual void          Reset();

  /// Define mirror setup. A default, as defined in constructor, 120 mirrors per edge, devided in 3 types
  void                  SetMirrors(Int_t mirrorTypes, Int_t mirrorsPerEdge );

ClassDef(PndDsk,1);

};

#endif // PNDDSK_H
