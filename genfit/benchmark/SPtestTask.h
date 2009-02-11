#ifndef SPTESTTASK_H
#define SPTESTTASK_H 


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

class SPtestTask : public FairTask
{
 public:
  /** Default constructor **/  
  SPtestTask();
  void WriteToFile(std::string fileName="out.root");
  /** Destructor **/
  ~SPtestTask();

  /** Virtual method Init **/
  virtual InitStatus Init();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  int _nEv;
  double _th,_posSig,_thSm,_phiSm,_mom,_momSm,_res;

 private:

  TGraph *g1;
  TGraph *g2;
  TGraph *g3;
  TGraph *g4;
  TGraph *g5;
  TGraph *g6;
  TGraph *g7;
  TGraph *g8;
  TGraph *g9;

  TPolyLine3D *l1,*uline,*vline;
  TPolyMarker3D *m1;
  TPolyMarker3D *m2;
  TPolyMarker3D *_plane;

  TTree* tree;

  double momRe;
  double momTr;
  double momSi;
  double momPu;
  double uRe;
  double uTr;
  double vRe;
  double vTr;
  double uSi;
  double vSi;
  double uPu;
  double vPu;
  double upRe;
  double upTr;
  double vpRe;
  double vpTr;
  double upSi;
  double vpSi;
  double upPu;
  double vpPu;
  double thTr;
  double thSt;
  double phiTr;
  double phiSt;


	/** Input array of Points **/
  TClonesArray* fPointArray;

  TGeant3 *gMC3;
  ClassDef(SPtestTask,1);
  Int_t fEvent;
  FairGeanePro *fPro;
};

#endif
