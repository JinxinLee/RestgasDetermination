#ifndef ILLUSTRATETASK_H
#define ILLUSTRATETASK_H 


#include "FairTask.h"
#include "TVector3.h"
#include "FairGeanePro.h"

#include "TGraph.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TH1D.h"
#include "TTree.h"

#include"GeaneTrackRep.h"
#include"DetPlane.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;
class AbsTrackRep;
class DetPlane;

class IllustrateTask : public FairTask
{
 public:
  /** Default constructor **/  
  IllustrateTask();
  /** Destructor **/
  ~IllustrateTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

 private:

  TPolyLine3D *Ltr,*Lst,*LtrBehind,*LstBehind,*Lfit,*LfitBehind,*LplU,*LplV,*LplN;
  TPolyMarker3D *Mtr,*Msm,*Mpl;



	/** Input array of Points **/
  TClonesArray* fPointArray;

  TGeant3 *gMC3;
  ClassDef(IllustrateTask,1);
  Int_t fEvent;
  FairGeanePro *fPro;
  void getLine(TPolyLine3D **infront,TPolyLine3D **behind,GeaneTrackRep GeaneRep,double dist,int nSteps,DetPlane* targetPlane=NULL);
};

#endif
