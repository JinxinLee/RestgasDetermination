/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// PndTpcLheCMTrack class - representation of one STS track with CM points        //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef PND_STS_CM_Track_H
#define PND_STS_CM_Track_H

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"

#include "PndTpcLheTrack.h"
#include "PndTpcLheCMPoint.h"
#include "PndTpcLheHit.h"

#include "Riostream.h"


class PndTpcLheCMTrack : public PndTpcLheTrack {
  
private:
  
  Int_t fNMapHits;
  TObjArray *fMappingHits;         // Array of pointers to hits of track
  

public:
  
  PndTpcLheCMTrack();                    // constructor
  PndTpcLheCMTrack(Int_t tracknumber);   // constructor with tracknumber
  PndTpcLheCMTrack(Int_t tn, Int_t nh);  //

  virtual  ~PndTpcLheCMTrack();          // destructor

  virtual  void Print();

  void AddPoint(PndTpcLheCMPoint *point, Bool_t bward);  // adds a point
  void Copy(const PndTpcLheCMTrack* src);  //

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

   ClassDef(PndTpcLheCMTrack, 1)    // 

 };

#endif

