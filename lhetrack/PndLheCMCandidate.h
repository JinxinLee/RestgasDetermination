/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// PndLheCMCandidate class - representation of one  track with CM points       //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef PND_CM_CANDIDATE_H
#define PND_CM_CANDIDATE_H

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"

#include "PndLheCandidate.h"
#include "PndLheCMPoint.h"

#include "Riostream.h"


class PndLheCMCandidate : public PndLheCandidate {
  
private:
  
  Int_t fNMapHits;
  TObjArray *fMappingHits;         // Array of pointers to hits of track
  

public:
  
  PndLheCMCandidate();                    // constructor
  PndLheCMCandidate(Int_t tracknumber);   // constructor with tracknumber
  PndLheCMCandidate(Int_t tn, Int_t nh);  //

  virtual  ~PndLheCMCandidate();          // destructor

  virtual  void Print();

  void AddPoint(PndLheCMPoint *point, Bool_t bward);  // adds a point
  void Copy(const PndLheCMCandidate* src);  //

  // getters

  TObjArray  *GetCMHits()          const { return fMappingHits; }
  //  Double_t   GetChi2Bend()         const { return fChi2Bend; }
  //  Double_t   GetChi2Deep()         const { return fChi2Deep; }
  Int_t      GetNumberOfPoints()   const { return fNMapHits; }

  // setters   
  void   SetTrackID(Int_t number);
  //  void   SetChi2Bend(Double_t f)      { fChi2Bend = f; }
  //  void   SetChi2Deep(Double_t f)      { fChi2Deep = f; }
  void   SetPointsUsage();  
  void   Clear();  

   ClassDef(PndLheCMCandidate, 1)    // 

 };

#endif

