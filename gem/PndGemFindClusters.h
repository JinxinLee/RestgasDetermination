//* $Id: */

// -------------------------------------------------------------------------
// -----                     PndGemFindClusters header file                -----
// -----                  Created 15/02/2009 by R. Karabowicz          -----
// -------------------------------------------------------------------------


/** PndGemFindClusters
 *@author Radoslaw Karabowicz <r.karabowicz@gsi.de>
 *@since 15/02/2009
 *@version 1.0
 **
 ** PANDA task class for finding hits in the GEM
 ** Task level RECO
 ** Produces objects of type PndGemHits out of PndGemDigi.
 **/


#ifndef PNDGEMFINDCLUSTERS_H
#define PNDGEMFINDCLUSTERS_H 1


#include "FairTask.h"

#include "TStopwatch.h"

#include <map>
#include <set>


class TClonesArray;
class PndGemDigiPar;
class PndGemSensor;
class PndGemStation;

struct DigiCluster {
  Int_t                 detId;
  std::vector<Int_t>    digiNr;
  std::vector<Int_t>    chanNr;
  std::vector<Double_t> sigADC;
  Double_t              cluTDC;
  Double_t              cluADC;
  Double_t              cluPos;
  Double_t              cluMPs;
  Double_t              cluMVl;
};

class PndGemFindClusters : public FairTask
{


 public :

  /** Default constructor **/
  PndGemFindClusters();


  /** Standard constructor **/
  PndGemFindClusters(Int_t iVerbose);


  /** Constructor with task name **/
  PndGemFindClusters(const char* name, Int_t iVerbose=0);


  /** Destructor **/
  virtual ~PndGemFindClusters();


  /** Execution **/
  virtual void Exec(Option_t* opt);



 private:

  PndGemDigiPar*    fDigiPar;     /** Digitisation parameters **/
  TClonesArray*     fDigis;       /** Input array of PndGemDigi **/
  TClonesArray*     fClusters;    /** Output array of PndGemDigi **/

  std::map <Double_t, Int_t> fTimeOrderedDigis;
  std::vector <DigiCluster>  fDigiClusters;

  Int_t             fTNofEvents;
  Int_t             fTNofDigis;
  Int_t             fTNofClusters;

  TStopwatch fTimer;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Sort digis timewise  **/
  Int_t SortDigis();


  /** Find clusters  **/
  void FindClusters();


  /** Write clusters  **/
  Int_t WriteClusters();


  /** Compare digi to clusters **/
  Bool_t CompareDigiToClusters(Int_t digiNumber);


  /** Analyze clusters **/
  void AnalyzeClusters();


  /** Clear clusters **/
  void ClearClusters();


  /** Finish at the end of each event **/
  virtual void Finish();


  ClassDef(PndGemFindClusters,1);

};

#endif
