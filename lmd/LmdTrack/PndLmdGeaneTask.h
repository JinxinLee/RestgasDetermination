// -------------------------------------------------------------------------
// -----                   PndLmdGeaneTask header file                 -----
// -----                  Created 20/03/07  by R.Kliemt                -----
// -----                  modified for Lmd by M. Michel                -----
// -------------------------------------------------------------------------


/** PndLmdGeaneTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PndLmdGeaneTask_H
#define PndLmdGeaneTask_H


// framework includes
#include "FairTask.h"
#include "TH1.h"
#include "GFTrackCand.h"
#include "FairGeanePro.h"

//#include "PndSdsGeoHandling.h"
#include "PndGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndSdsCluster;
class TGeant3;

class PndLmdGeaneTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndLmdGeaneTask();

  /** Destructor **/
  ~PndLmdGeaneTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();
  
 private:
  
		
  TClonesArray* fTrackParGeane;
  TClonesArray* fTrackParIni;
  TClonesArray* fTrackParFinal;
  TClonesArray* fDetName;
		
  FairGeanePro* fPro;
  PndGeoHandling* fGeoH;
  int fEventNr;
  bool fUseMVDPoint;
  Double_t fPbeam;
		
  std::map<int, std::vector<int> > fTrackPixHitIdMap;	 //Track -> PixHitId
  std::map<int, std::vector<int> > fTrackStripHitIdMap;	 //Track -> StripHitId

  // Input Data ----------
  TClonesArray* fMCHits;
  TClonesArray* fMCTracks;

  TClonesArray* fTracks; //Real tracks
  TClonesArray* fHits; //Real hits
  // Output Data -------


  void Register();
  
  void Reset();
  
  void ProduceHits();
  
  std::map<int, std::vector<int> > AssignHitsToTracks();
  
  ClassDef(PndLmdGeaneTask,1);

};

#endif
