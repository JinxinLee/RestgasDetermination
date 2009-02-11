// -------------------------------------------------------------------------
// -----               PndDchPreFitter header file                     -----
// -----               Created 06.02.2008  by A.Wronska                -----
// -------------------------------------------------------------------------

/** \class PndDchPreFitter
 *  \author A.Wronska <aleksandra.wronska@uj.edu.pl>
 *  \date 06.02.2008
 *  \brief Task for momentum reconstruction in the dipole
 * 
 *   This task performs  momentum reconstruction (rough!)
 *   for the forward spectrometer (dipole part)
 *   based on dch hits
 **/

#ifndef PNDDCHPREFITTER_H
#define PNDDCHPREFITTER_H


#include "FairTask.h"
#include "PndDchHit.h"
#include "TVector3.h"

class TClonesArray;
class TH1F;
class TString;
class PndDchStructure;
class TArrayI;
class PndDchPoint;


class PndDchPreFitter : public FairTask {
  
 public:
  
  /** Default constructor **/
  PndDchPreFitter();
  
  /** Destructor **/
  virtual ~PndDchPreFitter();
  
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
 
  /** Private method AddHits **/
  Int_t AddHits(Int_t trackID);

   TClonesArray* fPointArray; ///< Input array of PndDchPoints
  TClonesArray* fInHitArray;  ///< Input array of PndDchInHits

 
  TClonesArray* fInHitsBeforeXZ; ///< array of hit points (TVector2) in XZ plane upstream the dipole (Chamber3 and Chamber4)  
  TClonesArray* fInHitsAfterXZ;  ///< array of hit points (TVector2) in XZ plane downstream the dipole (Chamber7 and Chamber8)  

  /** array of hit points (TVector2) in XZ plane for the region of dipole 
   ** which are hits form (Chamber7 and Chamber8 plus 2 artificial points when the track 
   ** is entering and exiting the field region)  **/
  TClonesArray* fInHitsInXZ;

  TClonesArray* fInHitsYZ;  ///< array of hit points (TVector2) in YZ plane (hits from Chamber3 to Chamber8)
  Double_t fZFieldBegin;    ///< upstream z - coordinate of the field border
  Double_t fZFieldEnd;      ///< downstream z - coordinate of the field border
  Double_t fField;          ///< effecitive field strength

  TVector2 fTrackYZ;        ///< parametrised track projection in YZ-plane		     
  TVector2 fTrackBeforeXZ;  ///< parametrised track projection in XZ-plane before the dipole 
  TVector2 fTrackAfterXZ;   ///< parametrised track projection in XZ-plane after the dipole  
  TVector3 fTrackInXZ;      ///< parametrised track projection in XZ-plane in the dipole     

  TH1F* fPullPx;   ///< pull distribution of px	       
  TH1F* fPullPy;   ///< pull distribution of py        
  TH1F* fPullPz;   ///< pull distribution of pz        
  TH1F* fPullP;	   ///< pull distribution of p	       
  TH1F* fPullX;    ///< pull distribution of x	       
  TH1F* fPullY;    ///< pull distribution of y	       
  TString fOutFile;///< name of file to save histograms
 
  TClonesArray* fHitArray;     ///< Output array of PndDchHits	
  TClonesArray* fTrackArray;   ///< Output array of PndDchTracks
  
  
  PndDchStructure *fStructure; ///< pointer to geo structure of dch setup  
  TArrayI* fDetIdList;         ///< pointer to dch detector ID list	   
  Int_t fDetIdListSize;        ///< size of dch detector ID list           
  
  ClassDef(PndDchPreFitter,1)
  
};

#endif
