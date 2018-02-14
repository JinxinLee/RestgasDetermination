#ifndef PNDMVDDIGIANA_H
#define PNDMVDDIGIANA_H

#include "FairTask.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "PndSdsPixel.h"
#include "PndSdsDigiPixel.h"
#include "FairGeoVector.h"
#include "FairGeoTransform.h"
#include "TVector3.h"
#include "TRandom.h"
#include "TGeoMatrix.h"
#include "TGeoBBox.h"
#include "PndSdsStripDigiPar.h"
#include "PndSdsPixelDigiPar.h"

#include "TH3.h"

#include <string>
#include <vector>
 
class TClonesArray;

class PndMvdDigiAna : public FairTask
{
 public:

  /** Default constructor **/  
 PndMvdDigiAna();
  
  PndMvdDigiAna(TString DetName);

  /** Destructor **/
  virtual ~PndMvdDigiAna();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  

 protected:
  
  TString fBranchName;

  /** Input array **/
  TClonesArray* fHitArray;  
  TClonesArray* fPixelArray;
  TClonesArray* fStripArray;
  TClonesArray* fFePixelArray;
  
  TH3F* fPixelPos;
  TH3F* fStripPos;

  PndSdsPixelDigiPar* fDigiPar;
  
  void Register();
  void Reset();  
  void ProduceHits();

  void WriteHistograms(const TString& filename);
  void PrintHistograms(const TString& outpsfile);
  TString fDetName;
  ClassDef(PndMvdDigiAna,2);

};

#endif /*MVDDIGIANA_H*/
