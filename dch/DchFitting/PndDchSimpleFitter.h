// -------------------------------------------------------------------------
// -----               PndDchSimpleFitter header file                  -----
// -----               Created 14.08.2007  by P.Hawranek               -----
// -------------------------------------------------------------------------

/** \class PndDchSimpleFitter
 *  \author P.Hawranek
 *  \date 14.08.2007
 *  \brief Task for momentum reconstruction in the dipole
 * 
 *   This task performs a very naive momentum reconstruction (rough!)
 *   for the forward spectrometer (dipole part)
 *   based on the MCpoints
 **/

#ifndef PNDDCHSIMPLEFITTER_H
#define PNDDCHSIMPLEFITTER_H

// Pnd includes
#include "FairTask.h"
// ROOT includes
#include "TVector3.h"

class TClonesArray;
class TH1F;
class TString;

class PndDchSimpleFitter : public FairTask {
  
 public:
  
  /** Default constructor **/
  PndDchSimpleFitter();
  
  /** Destructor **/
  virtual ~PndDchSimpleFitter();
  
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
  Bool_t GetCircleParameters(TClonesArray *fPoints, TVector3& result);

  /** Private method GetLineParameters(...)
   ** fit line by less square method
   **/
  Bool_t GetLineParameters(TClonesArray *fPoints, TVector2& result) const;

  /** Private method GetMomentum(...)
   ** extracts momentum vector from the track parameters
   **/
  Bool_t GetMomentum(TVector3 circle, TVector2 lineXZ, TVector2 lineYZ, TVector3& momentum);
  
  TClonesArray* fPointArray; ///< Input array of PndDchPoints
  TClonesArray* fPointsBeforeXZ; ///< array of hit points (TVector2) in XZ plane upstream the dipole (Chamber3 and Chamber4)  
  TClonesArray* fPointsAfterXZ;  ///< array of hit points (TVector2) in XZ plane downstream the dipole (Chamber7 and Chamber8)  

  /** array of hit points (TVector2) in XZ plane for the region of dipole 
   ** which are hits form (Chamber7 and Chamber8 plus 2 artificial points when the track 
   ** is entering and exiting the field region)  **/
  TClonesArray* fPointsInXZ;

  TClonesArray* fPointsYZ; ///< array of hit points (TVector2) in YZ plane (hits from Chamber3 to Chamber8)
  Double_t fZFieldBegin;   ///< upstream z - coordinate of the field border
  Double_t fZFieldEnd;     ///< downstream z - coordinate of the field border
  Double_t fField;         ///< effecitive field strength
  TH1F* fPullPx;   ///< pull distribution of px
  TH1F* fPullPy;   ///< pull distribution of py
  TH1F* fPullPz;   ///< pull distribution of pz
  TH1F* fPullP;    ///< pull distribution of p
  TString fOutFile;///< name of file to save histograms
  
  ClassDef(PndDchSimpleFitter,1)
  
};

#endif
