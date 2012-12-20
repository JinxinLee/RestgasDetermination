// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PndMCTimebasedPatternRecoQuality_H
#define PndMCTimebasedPatternRecoQuality_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"
#include "PndMCDataCrawler.h"
#include "PndTrackCand.h"
#include "TH1.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTimebasedPatternRecoQuality : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTimebasedPatternRecoQuality();

  /** Destructor **/
  virtual ~PndMCTimebasedPatternRecoQuality();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

  virtual void FillMCTrackMap();



 private:
  PndMCDataCrawler* fMCCrawler;
  TClonesArray* fTrack;
  TClonesArray* fMCTrack;
  TClonesArray* fTrackCand;
  TH1* fPHisto;
  TH1* fPtHisto;
  TH1* fQualyHisto;
  TH1* fTimeHisto;

  std::map<FairLink, PndTrackCand*> fMCTrackMap;
  std::map<FairLink, Int_t> fMCTrackQualityMap;

  Int_t fEventNr;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCTimebasedPatternRecoQuality,1);

};

#endif
