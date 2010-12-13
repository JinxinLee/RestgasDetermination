// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PNDMCTESTPATTERNRECOQUALITY_H
#define PNDMCTESTPATTERNRECOQUALITY_H


// framework includes
#include "FairTask.h"
#include "PndMCMatch.h"
#include "PndDetectorList.h"
#include "TH1.h"


#include <vector>
#include <map>

class TClonesArray;

class PndMCTestPatternRecoQuality : public FairTask
{
 public:

  /** Default constructor **/
	PndMCTestPatternRecoQuality();

  /** Destructor **/
  virtual ~PndMCTestPatternRecoQuality();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();



 private:
  PndMCMatch* fMCMatch;
  TClonesArray* fTrack;
  TClonesArray* fMCTrack;
  TClonesArray* fTrackCand;
  TH1* fPHisto;
  TH1* fPtHisto;
  TH1* fQualyHisto;

  Int_t fEventNr;

  void Register();

  void Reset();

  void ProduceHits();


  ClassDef(PndMCTestPatternRecoQuality,1);

};

#endif
