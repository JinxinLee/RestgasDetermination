


#ifndef PNDDRCDIGITIZE_H
#define PNDDRCDIGITIZE_H 1

#include "FairTask.h"

#include "TStopwatch.h"

#include <list>
#include <map>

class TClonesArray;
class PndDrcDigiPar;



class PndDrcDigitize : public FairTask
{

 public:

  /** Default constructor **/
  PndDrcDigitize();


  /** Standard constructor **/
  PndDrcDigitize(Int_t iVerbose);


  /** Constructor with name **/
  PndDrcDigitize(const char* name, Int_t iVerbose);


  /** Destructor **/
  virtual ~PndDrcDigitize();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Switch saving outside hits **/
  //void SaveOutsideHits(Bool_t bt=kTRUE) {fSaveOutsideHits = bt;}


 private:

  //  FairGeoStsPar*     fGeoPar;       /** Geometry parameter container **/
  //  PndGemDigiPar*    fDigiPar;      /** Digitisation parameter container **/
  //  PndGemDigiScheme* fDigiScheme;   /** Digitisation scheme **/
  PndDrcDigiPar* fDigiPar;
  TClonesArray*     fPoints;       /** Input array of PndGemPoint **/
  TClonesArray*     fDigis;        /** Output array of PndGemDigi **/
  TClonesArray*     fDigiMatches;  /** Output array of PndGemDigiMatches**/
  //Int_t             fNPoints;     
  //Int_t             fNFailed;
  //Int_t             fNOutside;
  //Int_t             fNMulti;
  //Int_t             fNDigis;

  //Int_t             fTNofEvents;
  //Int_t             fTNofPoints;
  //Int_t             fTNofDigis;

  TStopwatch        fTimer;

  /** Map of active channels (pair detectorId, channel number) 
   ** to index of PndGemDigi **/
  //std::map<std::pair<Int_t, Int_t>, Int_t> fChannelMap; //!
 
  //TClonesArray* fHitOutsideArray;

  //Bool_t fSaveOutsideHits; /** whether to save the hits **/

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


  ClassDef(PndDrcDigitize,1);

};

#endif


