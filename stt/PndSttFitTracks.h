// -------------------------------------------------------------------------
// -----                    PndSttFitTracks header file                -----
// -----                  Created 18/02/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndSttFitTracks
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for track fitting in the STT.
 ** Input: TClonesArray of PndSttTrack
 ** Parameters of these objects are updated
 **
 ** Uses as track fitting algorithm classes derived from PndSttTrackFitter.
 **/


#ifndef PNDSTTFITTRACKS
#define PNDSTTFITTRACKS 1


#include "FairTask.h"

#include <string>
#include <vector>

class PndSttTrackFitter;
class TClonesArray;

class PndSttFitTracks : public FairTask
{

 public:

  /** Default constructor **/
  PndSttFitTracks();


  /** Standard constructor 
   **
   *@param name   Name of class
   *@param title  Task title
   *@param fitter Pointer to STT track fitter concrete class
   **/
  PndSttFitTracks(const char* name, const char* title = "FairTask",
		  PndSttTrackFitter* fitter = NULL);


  /** Destructor **/
  virtual ~PndSttFitTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();


  /** Accessors **/
  PndSttTrackFitter* GetFitter() { return fFitter; };
  Int_t GetNofTracks()           { return fNofTracks; };


  /** Set concrete track finder **/
  void UseFitter(PndSttTrackFitter* fitter) { fFitter = fitter; };

  void AddHitCollectionName(char *hitCollectionName);


 private:
  void AddAllCollections(); 
  void AddHitCollection(char const *collectionName);

  PndSttTrackFitter* fFitter;    // Pointer to GFTrackFinder concrete class
  TClonesArray* fTrackCandArray; // Input array of STT track candidates // CHECK add
  TClonesArray* fTrackArray;     // Output array of STT tracks  // CHECK add
  Int_t fNofTracks;              // Number of tracks successfully fitted
  std::vector<std::string> fHitCollectionNames;
  Bool_t fCollectionsComplete;

  ClassDef(PndSttFitTracks,1);

};

#endif
