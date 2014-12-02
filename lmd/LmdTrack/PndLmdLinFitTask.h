//-----------------------------------------------------------
//
// Description:
//      3D Straight Line fitter
//
// Author List:
//      Mathias Michel
//      Anastasia Karavdina 
//-----------------------------------------------------------

#ifndef PNDLMDLINFITTASK_H
#define PNDLMDLINFITTASK_H

// Base Class Headers ----------------
#include "FairTask.h"
#include <TGraph2D.h>
#include <TGraph2DErrors.h>
#include "TTree.h"
#include "TVirtualFitter.h"
// Collaborating Class Headers -------
#include <map>
#include "TString.h"
#include "TVector3.h"
#include "TMatrixTSym.h"
#include "TMatrixDSym.h"
#include "PndGeoHandling.h"
#include "PndLmdDim.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

class PndLmdLinFitTask : public FairTask {

public:

  // Constructors/Destructors ---------
  PndLmdLinFitTask();
  PndLmdLinFitTask(TString tTCandBranchName, TString tRecoBranchName);
  PndLmdLinFitTask(const PndLmdLinFitTask& o){};
  PndLmdLinFitTask& operator=(const PndLmdLinFitTask& o) { return *this;};
  virtual ~PndLmdLinFitTask();

  virtual InitStatus Init();

  virtual void Exec(Option_t* opt);

  virtual void Finish();

  /**
     * static instance
     */
    static PndLmdLinFitTask* Instance();

  void SetRadLen(double x){ftotRadLen = x;} //rad.length X/X0
 

protected:

  
  static void SumDistance2(int &, double *, double & sum, double * par, int ); //for Fitter
  static void LocalFCN(int &, double *, double & sum, double * par, int ); //for Fitter in local coordinates
  static void LocalFCN_MS(int &, double *, double & sum, double * par, int); //for Fitter with multiple scattering
  /* //minimization distance in 3D */
   static double distance2(double x,double y,double z, double *p);

  //minimization distance in 2D in local coordinates
   static double distance_l(double x, double y, double z, double errx, double erry, double errz, double *p);

  //minimization distance with kink angles
  static double distance_MS(double x, double y, double z, double errx, double erry, double errz, double *p, double *zpr);

  //minimization perpendicular distance between point and 3D line
  static double distance_perp(double x,double y,double z, double errx,double erry,double errz, double *p);
  double ScatteredAngle(double radLen);//calculate uncertancy for kink angle
  // Input Data------------
  TClonesArray* fTCandArray;
  TClonesArray* fRecoArray;
  TClonesArray* fTruePointArray;

  TString fTCandBranchName;
  TString fRecoBranchName;
  TString fTruePointBranch;
  // Output Data----------
  TClonesArray* fTrackArray;

  //Bool_t firstIt; 

  Int_t fTrackcount;

  Int_t fEvent;
 
  // Fitting ------------
  // void line(double t, double *p, double &x, double &y, double &z);
  double line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar, Double_t* fitparErr);
  double line3Dfit(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, TMatrixDSym *covmatrix);
  double line3DfitMS(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar, TMatrixDSym *covmatrix);// fit with kink angle
  //double line3Dfit(Int_t nd, TGraph2DErrors* gr, TVector3 posSeed, TVector3 dirSeed, Double_t* fitpar);

  double GetSigmaMS(int side){ if (side<1) return fsigmaMSa;
    else return fsigmaMSb;}


  //  double GetSigmaMSmerged(){return fsigmaMSmerged;}
  //  double GetSigmaMScone(){return fsigmaMScone;}
  double fPbeam;
  double fsigmaMSa;//single hit before CVD diamond
  double fsigmaMSb;//single hit after CVD diamond
  ///tmp vars for test tree
  double falx0a,falx0b,falx1a,falx1b,falx2a,falx2b,falx3a,falx3b;
  double faly0a,faly0b,faly1a,faly1b,faly2a,faly2b,faly3a,faly3b;
  double ferralx0a,ferralx0b,ferralx1a,ferralx1b,ferralx2a,ferralx2b,ferralx3a,ferralx3b;
  double ferraly0a,ferraly0b,ferraly1a,ferraly1b,ferraly2a,ferraly2b,ferraly3a,ferraly3b;
  double fchi2;
  double fzhit0;
  int fnpoints;
  ///END (tmp vars for test tree)
  bool hitMergedfl[4];
  double fPDGCode;
  int fCharge;
  PndGeoHandling* fGeoH;
  TTree *ttal;//for check new FCN function: LocalFCN_MS
  /** static pointer to this*/
  static PndLmdLinFitTask* fInstance;
  TVirtualFitter *fmin;
  PndLmdDim* lmddim;
 double ftotRadLen;//X/X0
  ClassDef(PndLmdLinFitTask,1);

};

#endif

