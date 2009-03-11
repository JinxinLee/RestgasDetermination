//* $Id: */

// -------------------------------------------------------------------------
// -----                    PndGemDigitize header file                 -----
// -----                  Created 30/08/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndGemDigitise
 *@author Volker Friese <v.friese@gsi.de>
 *@since 30.08.06
 *@version 1.0
 **
 ** CBM task class for digitising STS
 ** Task level SIM
 ** Produces objects of type PndGemDigi out of PndGemPoint.
 **/


#ifndef PNDGEMDIGITIZE_H
#define PNDGEMDIGITIZE_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <list>
#include <map>

class TClonesArray;
class PndGemDigiPar;
//class FairGeoStsPar;
//class PndGemDigiPar;
//class PndGemDigiScheme;
//class PndGemStation;



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
  TStopwatch        fTimer;

  /** Map of active channels (pair detectorId, channel number) 
   ** to index of StsDigi **/
  std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; //!
 
  TClonesArray* fHitOutputArray;

  /** Get parameter containers **/
  virtual void SetParContainers();


  /** Intialisation **/
  virtual InitStatus Init();


  /** Reinitialisation **/
  virtual InitStatus ReInit();


  /** Reset eventwise counters **/
  void Reset();



  ClassDef(PndGemDigitize,1);

};

#endif


