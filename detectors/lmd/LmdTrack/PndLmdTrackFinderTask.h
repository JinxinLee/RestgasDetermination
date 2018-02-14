// -------------------------------------------------------------------------
// -----                      PndLmdTrackFinderTask                    -----
// -----                  Created 22/10/09  by M. Michel               -----
// -------------------------------------------------------------------------

#ifndef PNDLMDTRACKFINDERTASK_H
#define PNDLMDTRACKFINDERTASK_H

#include "FairTask.h"
#include "PndMCTrack.h"
#include "PndSdsClusterStrip.h"
#include "PndSdsGeoPar.h"
#include "PndSdsHit.h"
#include "PndSdsMCPoint.h"
#include "TrackData/PndTrackCand.h"

#include <string>
#include <utility>
#include <vector>

class TClonesArray;
class PndLmdGeometryHelper;

class PndLmdTrackFinderTask : public FairTask {
 public:
  /** Default constructor **/
  PndLmdTrackFinderTask(Int_t inFinderMode = 0,
                        TString hitBranch = "LMDHitsStrip",
                        Int_t innSensPP = 8);
  /** Destructor **/
  virtual ~PndLmdTrackFinderTask();

  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t *opt);

  void SetVerbose(Int_t verbose) { fVerbose = verbose; };
  void SetInaccuracy(Double_t accu) { dXY = accu; };
  void SetSensStripFlag(bool fS) { flagStipSens = fS; };
  void SetSensPixelFlag(bool fS) { flagPixelSens = fS; };

 private:
  bool flagStipSens;
  bool flagPixelSens;
  Double_t dXY;
  Int_t fFinderMode;
  Int_t nSensPP;

  //    std::vector<Int_t> GetHitPerCluster(PndSdsClusterStrip* clusterCand);
  Double_t GetTrackDip(PndMCTrack *myTrack);
  Double_t GetTrackCurvature(PndMCTrack *myTrack);

  TString fHitBranchStrip;

  PndLmdGeometryHelper &lmd_geo_helper;

  /** Input array of PndSdsDigis **/
  TClonesArray *fStripHitArray;

  /** Output array of PndSdsHits **/
  TClonesArray *fTrackCandArray;

  void Register();
  void Reset();
  void ProduceHits();

  bool SortHitsByZ(std::vector<std::vector<std::pair<Int_t, bool> > > &hitsd,
                   Int_t nStripHits);
  bool SortHitsByDet(std::vector<std::vector<std::pair<Int_t, bool> > > &hitsd,
                     Int_t nStripHits);
  bool SortHitsByDet2(std::vector<std::vector<std::pair<Int_t, bool> > > &hitsd,
                      Int_t nStripHits);  // uses PndLmdDim
  void FindHitsI(std::vector<PndTrackCand> &tofill,
                 std::vector<std::vector<std::pair<Int_t, bool> > > &hitsd,
                 Int_t nStripHits);
  void FindHitsII(std::vector<PndTrackCand> &tofill,
                  std::vector<std::vector<std::pair<Int_t, bool> > > &hitsd,
                  Int_t nStripHits);
  void FindHitsIII(std::vector<PndTrackCand> &tofill,
                   std::vector<std::vector<std::pair<Int_t, bool> > > &hitsd,
                   Int_t nStripHits);

  ClassDef(PndLmdTrackFinderTask, 2);
};

#endif /* PndLmdTrackFinderTASK_H */
