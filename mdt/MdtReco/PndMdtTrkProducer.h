#ifndef PNDMDTTRKPRODUCER_H
#define PNDMDTTRKPRODUCER_H 1

#include <map>
#include <vector>

#include "FairTask.h"
#include "PndMdtTrk.h"
#include "PndMdtRecoPar.h"

#include "PndTrack.h"
#include "FairGeanePro.h"

class TClonesArray;

using std::map;
using std::vector;

class PndMdtTrkProducer : public FairTask
{

 public:

  /** Default constructor **/  
  PndMdtTrkProducer();


  /** Destructor **/
  ~PndMdtTrkProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void SetParContainers();
 
  virtual void AlgorithmWithLheGenTrack();

  void SetRecMethod(Int_t rec_method) {fRec_method = rec_method;} 
 private: 
  Int_t fRec_method; //0; previous method, use mdt info only; 1; use lhetrack as seed
  
  PndMdtTrk* AddTrk(PndMdtTrk* track);
  
  Bool_t MdtMapping(); // Creates maps of MDT hits
  void Reset();        // reset maps
  void SetGeometry();  // setting layer positions
  
  /** Input array of PndMdtHit **/
  TClonesArray* fHitArray; 
 
  /** Input array of LheGenTrack **/
  TClonesArray* fLheGenTrack;
 
  /** Output array of PndMdtTrk **/
  TClonesArray* fTrkArray; 
  
  //PndMdtRecoPar *fRecoPar;
 
  map<Int_t, vector<Int_t> >mapMdtBarrel;
  map<Int_t, vector<Int_t> >mapMdtEndcap;
  map<Int_t, vector<Int_t> >mapMdtForward;

  map<Int_t, TVector3> mapHitDirection; 
  map<Int_t, Float_t> mapHitDistance;
  Float_t mdtLayerPos[3][20];
  Float_t mdtIronThickness[3][20];
  Float_t mdtModule1MaxZ;

  ClassDef(PndMdtTrkProducer,1);

};

#endif
