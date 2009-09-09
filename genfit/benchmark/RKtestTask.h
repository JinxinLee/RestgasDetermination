#ifndef RKTESTTASK_H
#define RKTESTTASK_H 

#include"AbsBField.h"
#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"
#include "FairGeaneProNew.h"

#include "TGraph.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TH1D.h"
#include "TTree.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;

class RKtestTask : public FairTask
{
 public:
  /** Default constructor **/  
  RKtestTask();
  /** Destructor **/
  ~RKtestTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  void setField(AbsBField* f){field=f;}

 private:

  AbsBField* field;//!

 public:
  ClassDef(RKtestTask,1);
  Int_t fEvent;

};

#endif
