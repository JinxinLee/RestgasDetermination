#ifndef ILLUSTRATETASK_H
#define ILLUSTRATETASK_H 


#include "CbmTask.h"
#include "TVector3.h"
#include "CbmGeanePro.h"

#include "TGraph.h"
#include "TPolyLine3D.h"
#include "TPolyMarker3D.h"
#include "TH1D.h"
#include "TTree.h"

class TGeant3;
class TClonesArray;
class TTree;
class CbmPlanePoint;
class AbsTrackRep;
class DetPlane;

class IllustrateTask : public CbmTask
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

  TPolyLine3D *Ltr,*Lst,*Lfit,*LplU,*LplV,*LplN;
  TPolyMarker3D *Mtr,*Msm,*Mpl;


	/** Input array of Points **/
  TClonesArray* fPointArray;

  TGeant3 *gMC3;
  ClassDef(IllustrateTask,1);
  Int_t fEvent;
  CbmGeanePro *fPro;
  TPolyLine3D* getLine(AbsTrackRep* rep,DetPlane startPl,int nSteps,double dist,bool backward=false);
};

#endif
