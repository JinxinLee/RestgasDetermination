#ifndef PNDMUODIGIPRODUCER_H
#define PNDMUODIGIPRODUCER_H 1


#include "CbmTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "PndMuoDigi.h"
#include "TVector3.h"
#include "PndGeoMuoPar.h"

class TClonesArray;
class TObjectArray;

class PndMuoDigiProducer : public CbmTask
{

 public:

  /** Default constructor **/  
  PndMuoDigiProducer();


  /** Destructor **/
  ~PndMuoDigiProducer();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  PndMuoDigi* AddDigi(Int_t trackID, Int_t eventID, Short_t sec, Short_t pla, Short_t strip, Short_t box, Short_t tub);

  

 private: 
  PndGeoMuoPar *par; // parameters for geometry
  
  /** Input array of PndMuoPoints **/
  TClonesArray* fPointArray;

  /** Output array of PndMuoDigis **/
  TClonesArray* fDigiArray;  

  TObjArray *fVolumeArray;
  
  ClassDef(PndMuoDigiProducer,1);

};

#endif
