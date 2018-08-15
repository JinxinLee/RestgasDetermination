/*
 * PndPatternDBGenerator.h
 *
 *  Created on: Nov 8, 2017
 *      Author: Michael Papenbrock
 */

#ifndef PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBGENERATOR_H_
#define PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBGENERATOR_H_

#include <FairTask.h>
#include <PndGeoSttPar.h>
#include <PndMCTrack.h>
#include <PndPattern.h>
#include <PndSttHit.h>

typedef std::vector<PndSttHit*> HitArray;
typedef std::multimap<int,int> HitSectorMap;
typedef std::vector<PndPattern> PndPatterns;

class PndPatternDBGenerator: public FairTask {
public:
  PndPatternDBGenerator();
  virtual ~PndPatternDBGenerator();

  void SetOutputFilename(TString filename = "patternDB.root") {foutputFilename = filename;}

protected:
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
  virtual void FinishTask();

private:
  void GenerateTrackPatterns();
  HitSectorMap FillSectorHitMap(HitArray sttHitArray);
  PndPatterns FillSectorPatterns(HitArray hitArray, PndMCTrack* mcTrack);
  PndPatterns FillTrackPatterns(HitArray hitArray, PndMCTrack* mcTrack);
  void AddPatternsToTree(PndPatterns patterns, TTree* tree);

  PndGeoSttPar *fSttParameters;
  TClonesArray *fEventHeader;
  TClonesArray *fTubeArray;
  TClonesArray *fSttHitArray;
  TClonesArray *fMCTrackArray;
  TClonesArray *trackCands;

  TFile *foutputFile;
  TTree *fsectorPatternTree;
  TTree *ftrackPatternTree;

  PndPattern *bPattern;

  int sttBranchID;
  int mcTrackID;
  TString foutputFilename;

  int nTotalTracks;
  int nMultipleMCTrackLinks;

  ClassDef(PndPatternDBGenerator,1)
};

#endif /* PNDTRACKERS_PNDPATTERNMATCHER_PNDPATTERNDBGENERATOR_H_ */
