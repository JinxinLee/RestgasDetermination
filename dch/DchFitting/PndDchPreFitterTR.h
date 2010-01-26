// -------------------------------------------------------------------------
// -----               PndDchPreFitterTR header file                   -----
// -----               Created 26.02.2008  by A.Bubak                  -----
// -------------------------------------------------------------------------

/** \class PndDchPreFitterTR
 *  \author A.Bubak <arkadiusz.bubak@gmail.com>
 *  \date 26.02.2008
 *  \brief Task for preliminary momentum reconstruction in the dipole
 * 
 *   This task performs  momentum reconstruction (rough!)
 *   for the forward spectrometer (dipole part)
 *   based on dch tracks
 **/

#ifndef PNDDCHPREFITTERTR_H
#define PNDDCHPREFITTERTR_H


#include "FairTask.h"
#include "PndDchHit.h"
#include "TVector3.h"

class TClonesArray;
class TH1F;
class TString;
class PndDchStructure;
class TArrayI;
class PndDchPoint;
class PndDchCylinderHit;
class PndTrackCand;
class PndTrackCandHit;

class PndDchPreFitterTR : public FairTask {
  
 public:
  
  /** Default constructor **/
  PndDchPreFitterTR();
  
  /** Destructor **/
  virtual ~PndDchPreFitterTR();
  
  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Finish **/
  virtual void Finish();
  
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
 private: 
 
  /** Private method GetCircleParameters(...)
   ** fits a circle by less square method
   **/
  Bool_t GetCircleParameters(TClonesArray *fInHits, TVector3& result);

  /** Private method GetLineParameters(...)
   ** fit line by less square method
   **/
  Bool_t GetLineParameters(TClonesArray *fInHits, TVector2& result) const;

  /** Private method GetMomentum(...)
   ** extracts momentum vector from the track parameters
   **/
  Bool_t GetMomentum(TVector3& momentum);

  /** Private method AtInHitAtZ(...) **/
  Bool_t  GetInHitAtZ(Double_t zpos, TVector3 &point);
 
  /** Private method GetCrossPoint(TVector2 *line1, TVector2 *line2, TVector2 &point) **/
  Bool_t GetCrossPoint(PndDchCylinderHit *chit1, PndDchCylinderHit *chit2, TVector2 &point);

  /** Private method GetChargeSign()
      calculates charge sign based on the current content of the 
      arrays keeping reconstructed points positions 
      and reconstructed tracklets**/
  Int_t GetChargeSign();

  /** Private method GetHitPointInChamber(TClonesArray* chitArray) **/    
  std::map<Int_t , TVector3> GetHitPointsInChambers(PndTrackCand* tr);

  TClonesArray* fPointArray; ///< Input array of PndDchPoints
  TClonesArray* fInHitArray; ///< Input array of PndDchInHits
    TClonesArray* fDchTrackMatchArray; ///< Input array of TrackMatches
      ///< pozdrawiam cala rodzine hawrankow z doksydzenu
  TClonesArray* fInHitsBeforeXZ; ///< array of hit points (TVector2) in XZ plane upstream the dipole (Chamber3 and Chamber4)  
  TClonesArray* fInHitsAfterXZ;  ///< array of hit points (TVector2) in XZ plane downstream the dipole (Chamber7 and Chamber8)  

  /** array of hit points (TVector2) in XZ plane for the region of dipole 
   ** which are hits form (Chamber7 and Chamber8 plus 2 artificial points when the track 
   ** is entering and exiting the field region)  **/
  TClonesArray* fInHitsInXZ;

  TClonesArray* fInHitsYZ;  ///< array of hit points (TVector2) in YZ plane (hits from Chamber3 to Chamber8)
  Double_t fZFieldBegin;    ///< upstream z - coordinate of the field border
  Double_t fZFieldEnd;      ///< downstream z - coordinate of the field border
  Double_t fField;          ///< effective field strength

  TVector2 fTrackYZ;        ///< parametrised track projection in YZ-plane		     
  TVector2 fTrackBeforeXZ;  ///< parametrised track projection in XZ-plane before the dipole 
  TVector2 fTrackAfterXZ;   ///< parametrised track projection in XZ-plane after the dipole  
  TVector3 fTrackInXZ;	    ///< parametrised track projection in XZ-plane in the dipole     

  TClonesArray* fTrackArray; ///< Output array of PndTracksCand; 

  PndDchStructure *fStructure; ///< pointer to geo structure of dch setup  
  TArrayI* fDetIdList;	       ///< pointer to dch detector ID list	   
  Int_t fDetIdListSize;	       ///< size of dch detector ID list           
  
  ClassDef(PndDchPreFitterTR,1)
  
};

#endif
