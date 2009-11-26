#ifndef PNDMDTTRKPRODUCER_H
#define PNDMDTTRKPRODUCER_H 1

#include <map>
#include <vector>

#include "FairTask.h"
#include "PndMdtTrk.h"
#include "PndMdtRecoPar.h"

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
  
 private: 
  
  PndMdtTrk* AddTrk(PndMdtTrk* track);
  
  Bool_t MdtMapping(); // Creates maps of MDT hits
  void Reset();        // reset maps
  void SetGeometry();  // setting layer positions
  
  /** Input array of PndMdtHit **/
  TClonesArray* fHitArray; 
  
  /** Output array of PndMdtTrk **/
  TClonesArray* fTrkArray; 
  
  PndMdtRecoPar *fRecoPar;
 
  map<Int_t, vector<Int_t> >mapMdtBarrel;
  map<Int_t, vector<Int_t> >mapMdtEndcap;
  map<Int_t, vector<Int_t> >mapMdtForward;
  
  Float_t mdtLayerPos[3][20];
  Float_t mdtModule1MaxZ;

  ClassDef(PndMdtTrkProducer,1);

};

#endif
