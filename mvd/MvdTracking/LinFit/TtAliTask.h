#ifndef TTALITASK_H
#define TTALITASK_H

// Base Class Headers ----------------
#include "FairTask.h"
#include <TGraph2D.h>
#include <TGraph2DErrors.h>
#include "TH1F.h"
#include "TVector2.h"

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include <iostream>

using namespace std;

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;


class TtAliTask : public FairTask {

public:

  // Constructors/Destructors ---------
  TtAliTask();
  TtAliTask(Int_t ExcludedBox);
  TtAliTask(const TtAliTask& o) :
    fTCandArray(o.fTCandArray),
    fTCandBranchName(o.fTCandBranchName),
    fTrackcount(o.fTrackcount),
    fEvent(o.fEvent),
    fExclBox(o.fExclBox),
    fPrint(o.fPrint),
    sX(),
    sY(),
    sigX(),
    sigY(),
    m_X(),
    m_Y(),
    hx(o.hy),
    hy(o.hy)
  {
    for (Int_t gg = 0 ; gg < 4 ; gg++)
    {
      sX[gg] = o.sX[gg];
      sY[gg] = o.sY[gg];
      sigX[gg] = o.sigX[gg];
      sigY[gg] = o.sigY[gg];
      m_X[gg] = o.m_X[gg];
      m_Y[gg] = o.m_Y[gg];
    }
  };
  TtAliTask& operator=(const TtAliTask& o) 
  { 
    fTCandArray=o.fTCandArray;
    fTCandBranchName=o.fTCandBranchName;
    fTrackcount=o.fTrackcount;
    fEvent=o.fEvent;
    fExclBox=o.fExclBox;
    fPrint=o.fPrint;
    for (Int_t gg = 0 ; gg < 4 ; gg++)
    {
      sX[gg] = o.sX[gg];
      sY[gg] = o.sY[gg];
      sigX[gg] = o.sigX[gg];
      sigY[gg] = o.sigY[gg];
      m_X[gg] = o.m_X[gg];
      m_Y[gg] = o.m_Y[gg];
    }
    hx=o.hx;
    hy=o.hy;
    return *this;
  };
  virtual ~TtAliTask();

  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

  void SetExcludedBox(Int_t excl){
    if
      (excl > 0 && excl < 7)    fExclBox = excl;
    else
      cout << "Set a wrong value!!! Leaving fExclBox as it was" << std::endl;
  };

  void PrintVal();
  void PrintMeanResiduals();
  void PrintSigmaResiduals();
  TVector2 GetRes();
  void PrintHistos();
  void FlagPrint(Int_t flag){fPrint = flag;}; // 1 to print residuals 

private:

  // Input Data------------
  TClonesArray* fTCandArray;
  TString fTCandBranchName;

  //Bool_t firstIt; 
  Int_t fTrackcount;

  Int_t fEvent;
  Int_t fExclBox;

  Int_t fPrint; // flag 

  Double_t sX[6],sY[6]; // shifts
  Double_t sigX[6],sigY[6]; // sigma residuals
  Double_t m_X[6],m_Y[6]; // residuals

  TH1F *hx;
  TH1F *hy;

  // Fitting ------------
  void  MyFit(Double_t *x,Double_t *y,Double_t *z,Double_t *Erx,Double_t *Ery,Double_t *Erz,Double_t realX, Double_t realY, Double_t realZ, Double_t &DELTAX, Double_t &DELTAY);

  ClassDef(TtAliTask,1);

};

#endif

