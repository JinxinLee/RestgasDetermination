#ifndef SPTESTTASK_H
#define SPTESTTASK_H 


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

  void setFileName(std::string);
 private:

  std::string fileName;
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
  double chi2;
  int nfail;


	/** Input array of Points **/
  TClonesArray* fPointArray;

  TGeant3 *gMC3;
  ClassDef(SPtestTask,1);
  Int_t fEvent;
  FairGeanePro *fPro;
  FairGeaneProNew *fProNew;
};

#endif
