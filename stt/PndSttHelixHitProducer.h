#ifndef PNDSTTHELIXHITPRODUCER_H
#define PNDSTTHELIXHITPRODUCER_H 1


#include "FairTask.h"
#include "PndSttHelixHit.h"
#include "TVector3.h"
#include "TH1F.h"
#include "TH2F.h"

class TClonesArray;
class TObjectArray;

class PndSttHelixHitProducer : public FairTask
{

 public:

  /** Default constructor **/  
  PndSttHelixHitProducer();


  /** Destructor **/
  ~PndSttHelixHitProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void WriteHistograms();

 private: 

  /** Input array of CbmSttPoints **/
  TClonesArray* fPointArray;
  /** Input array of CbmSttHits **/
  TClonesArray* fHitArray;
  /** Input array of CbmSttTracks **/
  TClonesArray* fTrackArray;

  /** Output array of PndSttHelixHits **/
  TClonesArray* fHelixHitArray;  

  // histograms
  TH1F *hx, *hy, *hz;
  TH1F *hxs, *hys, *hzs;
  TH2F *hzresvsslope;


  ClassDef(PndSttHelixHitProducer,1);

};

#endif
