//-----------------------------------------------------------
//
// Description:
//      3D Straight Line fitter
//
// Author List:
//      Mathias Michel
//
//-----------------------------------------------------------

#ifndef PNDLMDLINFITTASK_H
#define PNDLMDLINFITTASK_H

// Base Class Headers ----------------
#include "FairTask.h"
#include <TGraph2D.h>
#include <TGraph2DErrors.h>

// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "TMatrixTSym.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

const Double_t fz0 = 1099.; //z-coordinate of first LMD plane
void SumDistance2(int &, double *, double & sum, double * par, int ); //for Fitter
void LocalFCN(int &, double *, double & sum, double * par, int ); //for Fitter in local coordinates
double distance2(double x,double y,double z, double *p);
double distance_l(double x, double y, double z, double errx, double erry, double errz, double *p);//in local coordinates
class PndLmdLinFitTask : public FairTask {

public:

  // Constructors/Destructors ---------
  PndLmdLinFitTask();
  PndLmdLinFitTask(const PndLmdLinFitTask& o){};
  PndLmdLinFitTask& operator=(const PndLmdLinFitTask& o) { return *this;};
  virtual ~PndLmdLinFitTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

private:

  // Input Data------------
  TClonesArray* fTCandArray;
  TClonesArray* fRecoArray;
  TString fTCandBranchName;
  TString fRecoBranchName;

  // Output Data----------
  TClonesArray* fTrackArray;

  //Bool_t firstIt; 

  Int_t fTrackcount;

  Int_t fEvent;

 
  // Fitting ------------
  void line(double t, double *p, double &x, double &y, double &z);
  double line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar, Double_t* fitparErr);
 

  ClassDef(PndLmdLinFitTask,1);

};

#endif

