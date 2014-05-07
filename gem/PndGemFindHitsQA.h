// -------------------------------------------------------------------------
// -----                   PndGemFindHitsQA header file          -----
// -----                  Created 02.06.2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------

/** \class PndGemFindHitsQA
 *  \author R. Karabowicz <r.karabowicz@gsi.de>
 *  \date 19.03.2009
 *  \brief track finding quality assesment task
 * 
 *  Track finding QA, efficiency, mom. resolution and so on
 **/


#ifndef PNDGEMFINDHITSQA_H
#define PNDGEMFINDHITSQA_H

#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"

#include "FairMCPoint.h"
#include "FairTask.h"

#include "PndGemHit.h"
#include "PndGemDigiPar.h"

#include <vector>

class PndGemFindHitsQA : public FairTask
{

 public:

  /** Default constructor **/
  PndGemFindHitsQA();

  /** Default constructor **/
  PndGemFindHitsQA(Int_t iVerbose);
  
  /** Destructor **/
  virtual ~PndGemFindHitsQA();
  
  /** Execution **/
  virtual void Exec(Option_t* opt);

  /** Public modifiers **/
  void SetVerbose(const Int_t& verbose) { fVerbose = verbose; };

  void SetPointEffDist(Double_t dn) {fPointEffDist=dn;};
  
 private:

  PndGemDigiPar* fDigiPar;

  /** Array of MC information **/
  TClonesArray* fMCPointArray;

  /** Array of reconstructed hits **/
  TClonesArray* fGemHitArray;

  /** Event counter **/
  Int_t fNofEvents;  ///< event counter
  
  TList* fHistoList;

  Int_t    fHistPlaneDivs;
  Double_t fHistWidth[4][2];

  Double_t fPointEffDist;

  // XY distance from point to hit
  // vs station(up to 4),sensor(2 per station),position(divide into 4x4 regions)
  TH2F* fhPointToHit[4][2][4][4];
  // distance of point to the closest hit
  //  TH3F* fhPointClosest     [4][2];
  TH2F* fhPointNof         [4][2];
  TH2F* fhPointReco        [4][2];
  TH2F* fhPointRecoEff     [4][2];
  TH1F* fhPointRadNof      [4][2];
  TH1F* fhPointRadReco     [4][2];
  TH1F* fhPointRadRecoEff  [4][2];

  TH2F* fhHitNof           [4][2];
  TH2F* fhHitFake          [4][2];
  TH2F* fhHitFakeProb      [4][2];
  TH1F* fhHitRadNof        [4][2];
  TH1F* fhHitRadFake       [4][2];
  TH1F* fhHitRadFakeProb   [4][2];

  TH2F* fhHitMultipleRate  [4][2];
 
  TH1F* fhCloseHits        [4][2];

  void CreateHistos();

  /** Get parameter containers **/
  virtual void SetParContainers();

  void DivideHistos(TH1* hist1, TH1* hist2, TH1* hist3);

  /** Finish **/
  virtual void Finish();

  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  ClassDef(PndGemFindHitsQA,1);

};

#endif
