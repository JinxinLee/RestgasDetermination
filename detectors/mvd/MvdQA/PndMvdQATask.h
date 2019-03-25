// -------------------------------------------------------------------------
// -----                 PndMvdQATask header file             -----
// -----                  Created 20/3/2019  by T.Stockmanns               -----
// -------------------------------------------------------------------------


/** PndMvdQATask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 **
 **/


#ifndef PndMvdQATask_H
#define PndMvdQATask_H

// Root includes
#include "TVector3.h"
#include "TRandom.h"
#include "TString.h"
#include "TGeoManager.h"
#include "TH1.h"

// framework includes
#include "PndBlackBoxTask.h"
#include "PndSdsHit.h"
#include "PndGeoHandling.h"


class TClonesArray;

class PndMvdQATask : public PndBlackBoxTask
{
 public:

  /** Default constructor **/
  PndMvdQATask();

  

  /** Destructor **/
  virtual ~PndMvdQATask();

  
  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();
  void SetWorkdir(TString path){fWorkdir = path;} //only needed to give cdash the path to the stored pictures
  
 protected:
  void CalculateDifference(PndSdsHit* hit);
  std::vector<Double_t> DoubleGaussFit(TH1*);
  bool TestPixel();
  bool TestStrip();
  void DrawPixelHistos();
  void DrawStripHistos();
  void SetPixelBoundaries();
  void SetStripBoundaries();


  void PrintDartMeasurementPixel();
  void PrintDartMeasurementStrip();

private:

  PndGeoHandling* fGeoH;
  TClonesArray* fMcPoints;
  TClonesArray* fStripHits;
  TClonesArray* fPixelHits;
  Bool_t fProperDifference; ///< if method CalculateDifference worked
  TVector3 fSpatialDifference;
  TString fWorkdir;
  double fTimeDifference;
  double fEnergyDifference;

  std::vector<double> fPixelSpatialRes;
  std::vector<std::pair<double, double> > fPixelBoundaries;
  std::vector<double> fStripSpatialRes;
  std::vector<std::pair<double, double> > fStripBoundaries;


  TH1* fHStripSingle;
  TH1* fHStripDouble;
  TH1* fHStripAll;
  TH1* fHStripClusterSize;
  TH1* fHStripTime;
  TH1* fHStripEnergy;

  TH1* fHPixelSingle;
  TH1* fHPixelDouble;
  TH1* fHPixelAll;
  TH1* fHPixelClusterSize;
  TH1* fHPixelTime;
  TH1* fHPixelEnergy;





  ClassDef(PndMvdQATask,1);

};

#endif
