// -------------------------------------------------------------------------
// -----                   PndSttFindTracks header file                -----
// -----                  Created 29/03/06  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndSttFindTracks
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for track finding in the STT. 
 ** Input: TClonesArray of PndSttHit 
 ** Output: TClonesArray of PndTrackCand
 **
 ** Uses as track finding algorithm classes derived from PndSttTrackFinder.
 **/


#ifndef PNDSTTFINDTRACKS
#define PNDSTTFINDTRACKS 1

#include "FairTask.h"
#include "PndGeoSttPar.h"

#include <string>
#include <vector>

class PndSttTrackFinder;
class TClonesArray;

class PndSttFindTracks : public FairTask
{

 public:

  /** Default constructor **/
  PndSttFindTracks();


  /** Standard constructor
   *@param finder   Pointer to STT track finder concrete class
   *@param verbose  Verbosity level
   **/
  PndSttFindTracks(PndSttTrackFinder* finder, Int_t verbose = 1);


  /** Constructor with name and title
   *@param name     Name of class
   *@param title    Task title
   *@param finder   Pointer to STT track finder concrete class
   *@param verbose  Verbosity level
   **/
  PndSttFindTracks(const char* name, const char* title = "FairTask", 
		   PndSttTrackFinder* finder = NULL, Int_t verbose = 1);


  /** Destructor **/
  virtual ~PndSttFindTracks();


  /** Initialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Task execution **/
  virtual void Exec(Option_t* opt);


  /** Finish at the end of each event **/
  virtual void Finish();

  void SetParContainers();

  /** Accessors **/
  PndSttTrackFinder* GetFinder() { return fFinder; };
  Int_t GetNofTracks()           { return fNofTracks; };


  /** Set concrete track finder **/
  void UseFinder(PndSttTrackFinder* finder) { fFinder = finder; };

  /** Add an hit collection to perform trackfinding on */
  void AddHitCollectionName(char *hitCollectionName, char *pointCollectionName);

  /** set persistence flag **/
  void SetPersistence(Bool_t persistence) { fPersistence = persistence; }

  /** set the helix hit production flag true or false **/
  void SetHelixHitProduction(Bool_t hhprod = kTRUE) { fHelixHitProduction = hhprod; }
   

 private:
  void AddAllCollections(); 
  void AddHitCollection(char const *collectionName, char const *pointCollectionName);

  PndSttTrackFinder* fFinder;    // Pointer to TrackFinder concrete class
  TClonesArray* fTrackCandArray; // Output array of PndTrackCand
  TClonesArray* fTrackArray;     // Output array of PndTrack

  TClonesArray* fHelixHitArray;  // Output array of PndSttHelixHit

  Int_t fNofTracks;              // Number of tracks created
  Int_t fVerbose;                // Verbosity level

  std::vector<std::string> fHitCollectionNames;
  std::vector<std::string> fPointCollectionNames;

  Bool_t fCollectionsComplete;

  /** object persistence **/
  Bool_t  fPersistence; //!

  /** production to file of helix hit from PR or not **/
  Bool_t fHelixHitProduction; //!

  Int_t fEventCounter; //!

  /** from parameters array of PndSttTube **/  //  CHECK added
  TClonesArray* fTubeArray;
  
  PndGeoSttPar *fSttParameters;  //  CHECK added
  
  ClassDef(PndSttFindTracks,1);
};

#endif
