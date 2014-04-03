//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndGemFindHits header file                -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndGemFindHits
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15/02/2009
 *@version 1.0
 **
 ** PANDA task class for finding hits in the GEM
 ** Task level RECO
 ** Produces objects of type PndGemHits out of PndGemDigi.
 **/


#ifndef PNDGEMFINDHITS_H
#define PNDGEMFINDHITS_H 1


#include "FairTask.h"

#include "TStopwatch.h"
#include "TH2F.h"

#include <map>
#include <set>
#include <vector>

class TH1F;
class TClonesArray;
class PndGemMonitor;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;
class PndGemCluster;

class PndGemFindHits : public FairTask
{


 public :

  /** Default constructor **/
  PndGemFindHits();


  /** Standard constructor **/
  PndGemFindHits(Int_t iVerbose);


  /** Constructor with task name **/
  PndGemFindHits(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~PndGemFindHits();
  void ConfirmHits();
  void ActivateDigis();


  /** Execution **/
  virtual void Exec(Option_t* opt);

  void SetUseClusters(Bool_t bt=kTRUE) {fUseClusters = bt;}

 private:

  PndGemMonitor*    fMonitor;  //!   /** GEM monitor **/
  PndGemDigiPar*    fDigiPar;     /** Digitisation parameters **/
  TClonesArray*     fDigis;       /** Input array of PndGemDigi **/
  TClonesArray*     fHits;        /** Output array of PndGemHit **/
  std::map<PndGemSensor*, std::set<Int_t> > fDigiMapF;  /** sensor digis (front) **/
  std::map<PndGemSensor*, std::set<Int_t> > fDigiMapB;  /** sensor digis (back)  **/

  Int_t             fTNofEvents;
  Int_t             fTNofDigis;
  Int_t             fTNofHits;

  TStopwatch fTimer;

  Double_t fPrepTime;
  Double_t fSortTime;
  Double_t fCreateTime;
  Double_t fConfirmTime;
  Double_t fActivateTime;
  Double_t fAllTime;

  Bool_t fUseClusters;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Make sensorwise sets for sigis  **/
  void MakeSets();


  /** Sort digis sensorwise  **/
  void SortDigis();


  /** Find hits in one sensor **/
  Int_t FindHits(PndGemSensor* sensor,
		 std::set<Int_t>& fSet, std::set<Int_t>& bSet);


  /** Finish at the end of each event **/
  virtual void Finish();


  ClassDef(PndGemFindHits,1);

};

#endif
