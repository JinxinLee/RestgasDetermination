// -------------------------------------------------------------------------
// -----                  CbmStsMatchTracks header file                -----
// -----                  Created 22/11/05  by V. Friese               -----
// -------------------------------------------------------------------------


/** PndSttMatchTracks.h
 *@author V.Friese <v.friese@gsi.de>
 **
 ** Task class for matching a reconstructed PndSttTrack with a simulated
 ** CbmMCTrack. The matching criterion is a maximal number of common
 ** hits/points. The task fills the data class PndSttTrackMatch for
 ** each PndSttTrack.
 **/


#ifndef PNDSTTMATCHTRACKS_H
#define PNDSTTMATCHTRACKS_H 1

#include <vector>
#include <map>
#include "CbmTask.h"
#include "PndSttHit.h"
#include "CbmMCPoint.h"

class TClonesArray;



class PndSttMatchTracks : public CbmTask
{

 public:

  /** Default constructor **/
  PndSttMatchTracks();


  /** Constructor with verbosity level **/
  PndSttMatchTracks(Int_t verbose);


  /** Constructor with name, title and verbosity
   **
   *@param name     Name of taks
   *@param title    Title of task   (default CbmTask)
   *@param verbose  Verbosity level (default 1)
   **/
  PndSttMatchTracks(const char* name, const char* title = "CbmTask",
		    Int_t verbose = 1);


  /** Destructor **/
  virtual ~PndSttMatchTracks();


  /** Intialisation at beginning of each event **/
  virtual InitStatus Init();


  /** Execution **/
  virtual void Exec(Option_t* opt);


  /** Finishing */
  virtual void Finish();

  /** Add an hit collection to perform trackfinding on */
  void AddHitCollectionName(char *hitCollectionName, char *pointCollectionName);
  PndSttHit* GetHitFromCollections(Int_t hitCounter);
  CbmMCPoint* GetPointFromCollections(Int_t hitCounter);

 private:
  void AddAllCollections(); 
  void AddHitCollection(char const *collectionName, char const *pointCollectionName);

  TClonesArray* fTracks;       // Array of PndSttTracks
  TClonesArray* fMatches;      // Array of PndSttTrackMatch

  /** Map from MCTrackID to number of common hits **/
  std::map<Int_t, Int_t> fMatchMap;

  /** Verbosity level **/
  Int_t fVerbose;

  Bool_t fCollectionsComplete;
 
  std::vector<std::string> fHitCollectionNames;
  std::vector<std::string> fPointCollectionNames;
  TList fHitCollectionList;
  TList fPointCollectionList;

  ClassDef(PndSttMatchTracks,1);
};

#endif
