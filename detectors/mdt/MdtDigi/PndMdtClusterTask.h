#ifndef PNDMDTCLUSTERTASK_H
#define PNDMDTCLUSTERTASK_H 1

#include <PndPersistencyTask.h>
#include "PndMdtCluster.h"

class TClonesArray;

using std::map;
using std::vector;

class PndMdtClusterTask : public PndPersistencyTask
{

 public:

  /** Default constructor **/  
  PndMdtClusterTask();


  /** Destructor **/
  ~PndMdtClusterTask();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  void SetPersistence(Bool_t pers)  {  SetPersistency(pers); };
  
 private: 
  
  Bool_t MdtMapping(); // Creates maps of MDT hits
  void Reset();        // reset maps
  
  PndMdtCluster* AddClusterBox(std::vector<Int_t> digiList); 
  PndMdtCluster* AddClusterStrip(std::vector<Int_t> digiList);
  

  /** Input array of PndMdtDigi **/
  TClonesArray* fBoxIArray;
  TClonesArray* fStripIArray;
  
  /** Output array of PndMdtHit **/ 
  TClonesArray* fBoxOArray;
  TClonesArray* fStripOArray;
  
  /** Maps layer - digiIndex **/
  map<Int_t, Int_t>mapBox;
  map<Int_t, Int_t>mapStrip;
  
  ClassDef(PndMdtClusterTask,1);

  };

#endif
