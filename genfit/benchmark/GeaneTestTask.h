#ifndef GEANETESTTASK_H
#define GEANETESTTASK_H 


#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"

#include "TGraph.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TH1D.h"
#include "TTree.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;

class GeaneTestTask : public FairTask
{
 public:
  /** Default constructor **/  
  GeaneTestTask();
  /** Destructor **/
  ~GeaneTestTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

 private:


  TGeant3 *gMC3;
  ClassDef(GeaneTestTask,1);
  Int_t fEvent;
  FairGeanePro *fPro;
};

#endif
