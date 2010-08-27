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
#include "TVector3.h"
#include "TMatrixTSym.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

//const Double_t fz0 = 1099.; //z-coordinate of first LMD plane
//void SumDistance2(int &, double *, double & sum, double * par, int ); //for Fitter
//void LocalFCN(int &, double *, double & sum, double * par, int ); //for Fitter in local coordinates
//minimization distance in 3D
//double distance2(double x,double y,double z, double *p);
//minimization distance in 2D in local coordinates
//double distance_l(double x, double y, double z, double errx, double erry, double errz, double *p);//in local coordinates
//minimization perpendicular distance between point and 3D line
//double distance_perp(double x,double y,double z, double errx,double erry,double errz, double *p);

class PndLmdLinFitTask : public FairTask {

public:

  // Constructors/Destructors ---------
  PndLmdLinFitTask();
  PndLmdLinFitTask(const PndLmdLinFitTask& o){};
  PndLmdLinFitTask& operator=(const PndLmdLinFitTask& o) { return *this;};
  virtual ~PndLmdLinFitTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

protected:

  static Double_t fz0; //z-coordinate of first LMD plane
  static void SumDistance2(int &, double *, double & sum, double * par, int ); //for Fitter
  static void LocalFCN(int &, double *, double & sum, double * par, int ); //for Fitter in local coordinates
  //minimization distance in 3D
  static double distance2(double x,double y,double z, double *p);
  //minimization distance in 2D in local coordinates
  static double distance_l(double x, double y, double z, double errx, double erry, double errz, double *p);
  //minimization perpendicular distance between point and 3D line
  static double distance_perp(double x,double y,double z, double errx,double erry,double errz, double *p);

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
  double line3Dfit(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, Double_t* fitparerr);

  ClassDef(PndLmdLinFitTask,1);

};

#endif

