// Fit Task for linear tracks in the Bonn Tracking Station (6 sensors)
// The output is a TClonesArray for each event with all the possible tracks
// stored in a PndLinTrack(modifed version with eloss) object

// Simone Bianco 21/03/2010

#ifndef PNDMVDLINFITTASK_H
#define PNDMVDLINFITTASK_H

// Base Class Headers ----------------
#include "FairTask.h"
#include <TGraph2D.h>
#include <TGraph2DErrors.h>

// Collaborating Class Headers -------
#include <map>
#include "TString.h"

// Collaborating Class Declarations --
class TClonesArray;
class TGeoManager;

void SumDistance2(int &, double *, double & sum, double * par, int ); //for Fitter
double distance2(double x,double y,double z, double *p);

class PndMvdLinFitTask : public FairTask {

public:

  // Constructors/Destructors ---------
  PndMvdLinFitTask();
  PndMvdLinFitTask(const PndMvdLinFitTask& o){};
  PndMvdLinFitTask& operator=(const PndMvdLinFitTask& o) { return *this;};
  virtual ~PndMvdLinFitTask();

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
  double line3Dfit(Int_t nd, TGraph2DErrors* gr, Double_t* fitpar);

  ClassDef(PndMvdLinFitTask,1);

};

#endif

