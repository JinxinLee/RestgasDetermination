#ifndef PNDMDTHITPRODUCER_H
#define PNDMDTHITPRODUCER_H 1

#include "FairTask.h"
#include "PndMdtHit.h"
#include "TVector3.h"

class TClonesArray;

using std::map;
using std::vector;

class PndMdtHitProducer : public FairTask
{

 public:

  /** Default constructor **/  
  PndMdtHitProducer();


  /** Destructor **/
  ~PndMdtHitProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndMdtHit* AddHit(Int_t detID, Int_t stripID, TVector3& pos, TVector3& dpos, Int_t bIndex, Int_t sIndex);
  
  //  Bool_t MdtMapping(); // Creates maps of MDT hits
  //void Reset();        // reset maps

 private: 
  
  /** Input array of PndMdtDigi **/
  TClonesArray* fBoxArray;
  TClonesArray* fStripArray;
  
  /** Output array of PndMdtHit **/
  TClonesArray* fHitArray;  
  
/*   map<Int_t, vector<Int_t> >mapBoxBarrel; */
/*   map<Int_t, vector<Int_t> >mapBoxEndcap; */
/*   map<Int_t, vector<Int_t> >mapBoxForward; */
/*   map<Int_t, vector<Int_t> >mapStripBarrel; */
/*   map<Int_t, vector<Int_t> >mapStripEndcap; */
/*   map<Int_t, vector<Int_t> >mapStripForward; */
  
  ClassDef(PndMdtHitProducer,1);

};

#endif
