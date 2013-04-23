//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndBarrelTrackFinder header file                 -----
// -----                  Created 05/12/2010 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndGemDigitise
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 08.12.2009
 *@version 1.0
 **
 ** PANDA task class for finding tracks in CT nad GEM
 ** Task level TRACK
 ** Produces objects of type PndTrack and PndTrackCand
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
  Double_t z_p;
  Int_t n;
  // x,y,r parameters of the circle,
  //  //  p=atan(x/y), 
  // z_p - z increase divided by phi angle increase,
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
  Double_t meanZ_P;
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

  void UseMvdSttGem   (const Bool_t useMvd, const Bool_t useStt, const Bool_t useGem); 

 private:

  std::vector<TrackBasis> fTracksVector;

  std::vector<Int_t>    fHitDetId;
  std::vector<Int_t>    fHitDetNo;
  // previous hits
  std::vector<FairHit*> fHitVector;
  std::vector<Int_t>    fHitVectDI;
  std::vector<Int_t>    fHitVectHN;

  Bool_t        fIncludeDet[4];
  TClonesArray* fHitArray  [4];
  TString       fDetName   [4];
  Int_t         fDetType   [4];
  TString       fHitArrayName[4];

  /** Output array of global tracks **/
  TClonesArray* fBarrelTrackArray;     	    ///< Output array of PndTrackCands 
  TClonesArray* fBarrelTrackCandArray;

  /** Event counter **/
  Int_t fTNofEvents;  ///< event counter
  Int_t fTNofTracks;

  Double_t fMaximalDist;
  Double_t fReasonableDist;
  Double_t fMaximalZ_PD;
  Double_t fReasonableZ_PD;
  Double_t fMaximalRadDiff;
  Double_t fMaximalPhiDiff;

  Bool_t MatchHitToTrack          (FairHit* thisHit, Int_t detId, Int_t hitNo, Int_t trackNo);
  Bool_t MatchSkewedSttHitTT      (FairHit* thisHit, Int_t detId, Int_t hitNo, Int_t trackNo);
  Bool_t MatchParallelSttHitTT    (FairHit* thisHit, Int_t detId, Int_t hitNo, Int_t trackNo);
  Bool_t MatchHitWithZInfoTT      (FairHit* thisHit, Int_t detId, Int_t hitNo, Int_t trackNo);

  Bool_t ExtractMeanRPhiFromTrack (Int_t trackNo);
  Bool_t ExtractMeanZ_PFromTrack  (Int_t trackNo);

  Bool_t MatchHitToHit            (FairHit* thisHit, Int_t detId, Int_t hitNo, Int_t prevHNo);
  void   RemoveHitFromPreviousHits(Int_t hitNo);

  Bool_t HitBelongsToTrack(Int_t detId, Int_t hitNo, Int_t trackNo);

  void   AddHitToPreviousHits     (FairHit* thisHit, Int_t detId, Int_t hitNo);

  void   PrintTracks();
  Int_t  CleanTracks();
  Int_t  WriteTracks();

  void   RemoveShortTracks();
  void   RemoveCloneTracks();

  Bool_t   FindCircPar(Double_t* c1, Double_t* c2, Double_t* c3, Int_t cno, Double_t* cl);
  Double_t FindCircDist(Double_t* c1, Double_t* c2);
  Int_t    FindInterestingRegions(Double_t* circ, Double_t* tube, Double_t* reg);

  Double_t CalcPhi(Double_t x, Double_t y);
  Double_t CalcZ_P(Double_t* circ, Double_t hx, Double_t hy, Double_t hz);

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


