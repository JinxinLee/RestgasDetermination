//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndBarrelTrackFinder header file                 -----
// -----                  Created 12/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndGemDigitise
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 12.02.2009
 *@version 1.0
 **
 ** PANDA task class for digitising GEM
 ** Task level SIM
 ** Produces objects of type PndGemDigi out of PndGemMCPoint.
 **/


#ifndef PNDBARRELTRACKFINDER_H
#define PNDBARRELTRACKFINDER_H 1

#include "FairTask.h"
#include "FairHit.h"

#include "TStopwatch.h"
#include "PndGeoSttPar.h"

#include <list>
#include <map>
#include <vector>

class TClonesArray;
class PndGemDigiPar;

struct TrackParameter{
  Double_t x;
  Double_t y;
  Double_t r;
  Double_t p_z;
  Int_t n;
  // x,y,r parameters of the circle,
  //  //  p=atan(x/y), 
  // p_z - phi angle increase per 1 cm in z, 
  //  //  p_1=atan(x_1/y_1) of the first hit, 
  // n='goodness' of parameter
};

struct TrackBasis{
  std::vector<TrackParameter> trackPars;
  std::vector<FairHit*>       trackHits;
  std::vector<Int_t>          trackHitD;
  std::vector<Int_t>          trackHitN;
  Double_t meanX;
  Double_t meanY;
  Double_t meanR;
  Double_t meanP_Z;
}; 


class PndBarrelTrackFinder : public FairTask
{

 public:

  /** Default constructor **/
  PndBarrelTrackFinder();


  /** Standard constructor **/
  PndBarrelTrackFinder(Int_t iVerbose);


  /** Constructor with name **/
  PndBarrelTrackFinder(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~PndBarrelTrackFinder();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  void UseMvdSttTpcGem(const Bool_t useMvd, const Bool_t useStt, const Bool_t useTpc, const Bool_t useGem); 

 private:

  std::vector<TrackBasis> fTracksVector;

  Bool_t        fIncludeDet[5];
  TClonesArray* fHitArray  [5];
  TString       fDetName   [5];

  /** Event counter **/
  Int_t fTNofEvents;  ///< event counter
  Int_t fTNofTracks;

  Bool_t   FindCircPar(Double_t* c1, Double_t* c2, Double_t* c3, Int_t cno, Double_t* cl);
  Double_t FindCircDist(Double_t* c1, Double_t* c2);
  Int_t    FindInterestingRegions(Double_t* circ, Double_t* tube, Double_t* reg);

  Double_t CalcPhi(Double_t x, Double_t y);
  Double_t CalcP_Z(Double_t* circ, Double_t hx, Double_t hy, Double_t hz);

  /** Get parameter containers **/
  virtual void SetParContainers();

  /** from parameters array of PndSttTube **/  //  CHECK added
  TClonesArray* fTubeArray;
  PndGeoSttPar *fSttParameters;  //  CHECK added

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();


  /** Finish at the end of each event **/
  virtual void Finish();


  ClassDef(PndBarrelTrackFinder,1);

};

#endif


