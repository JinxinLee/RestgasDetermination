// -------------------------------------------------------------------------
// -----                 PndMvdMSAnaTask header file             -----
// -----                  Created 20/03/07  by R.Kliemt               -----
// -------------------------------------------------------------------------


/** PndMvdMSAnaTask.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef PndMvdMSAnaTask_H
#define PndMvdMSAnaTask_H


// framework includes
#include "CbmTask.h"
#include "TH1.h"
#include "TrackCand.h"
#include "CbmGeanePro.h"

#include "PndMvdGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndMvdCluster;

class PndMvdMSAnaTask : public CbmTask
{
 public:

  /** Default constructor **/  
  PndMvdMSAnaTask();

  /** Destructor **/
  ~PndMvdMSAnaTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();
  
 private:
		TClonesArray* fMCHits;
		TClonesArray* fMCTracks;
		
		TClonesArray* fTrackParGeane;
		TClonesArray* fTrackParIni;
		TClonesArray* fTrackParFinal;
		TClonesArray* fDetName;
		
		CbmGeanePro* fPro;
		PndMvdGeoHandling* fGeoH;
		bool fUseMVDPoint;
		
				
		
		int fEventNr;
		
		std::map<int, std::vector<int> > fTrackPixHitIdMap;				//Track -> PixHitId
		std::map<int, std::vector<int> > fTrackStripHitIdMap;			//Track -> StripHitId

  void Register();
  
  void Reset();
  
  void ProduceHits();
  
  std::map<int, std::vector<int> > AssignHitsToTracks();
  
  

  ClassDef(PndMvdMSAnaTask,1);

};

#endif
