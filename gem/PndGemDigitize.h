//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigitize header file                 -----
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


#ifndef PNDGEMDIGITIZE_H
#define PNDGEMDIGITIZE_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <list>
#include <map>

class TClonesArray;
class PndGemDigiPar;



class PndGemDigitize : public FairTask
{

 public:

  /** Default constructor **/
  PndGemDigitize();


  /** Standard constructor **/
  PndGemDigitize(Int_t iVerbose);


  /** Constructor with name **/
  PndGemDigitize(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~PndGemDigitize();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Switch saving outside hits **/
  void SaveOutsideHits(Bool_t bt=kTRUE) {fSaveOutsideHits = bt;}


 private:

  //  FairGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  //  PndGemDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  //  PndGemDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  PndGemDigiPar* fDigiPar;
  TClonesArray*     fPoints;       /** Input array of PndGemPoint **/
  TClonesArray*     fDigis;        /** Output array of PndGemDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of PndGemDigiMatches**/
  Int_t             fNPoints;     
  Int_t             fNFailed;
  Int_t             fNOutside;
  Int_t             fNMulti;
  Int_t             fNDigis;

  Int_t             fTNofEvents;
  Int_t             fTNofPoints;
  Int_t             fTNofDigis;

  TStopwatch        fTimer;

  /** Map of active channels (pair detectorId, channel number) 
   ** to index of PndGemDigi **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; //!
 
  TClonesArray* fHitOutsideArray;

  Bool_t fSaveOutsideHits; /** whether to save the hits **/

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();


  /** Finish at the end of each event **/
  virtual void Finish();


  ClassDef(PndGemDigitize,1);

};

#endif


