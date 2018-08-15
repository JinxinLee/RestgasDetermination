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
#include "FairTask.h"
#include "TH1.h"
#include "FairGeanePro.h"

#include "PndGeoHandling.h"

#include <vector>
#include <map>

class TClonesArray;
class PndSdsCluster;

class PndMvdMSAnaTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndMvdMSAnaTask();

  /** Destructor **/
  ~PndMvdMSAnaTask();
  PndMvdMSAnaTask(const PndMvdMSAnaTask& ) = delete;
  PndMvdMSAnaTask& operator=(const PndMvdMSAnaTask& ) = delete;
  

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
		
		FairGeanePro* fPro;
		PndGeoHandling* fGeoH;
    int fEventNr;
		bool fUseMVDPoint;
		
		std::map<int, std::vector<int> > fTrackPixHitIdMap;				//Track -> PixHitId
		std::map<int, std::vector<int> > fTrackStripHitIdMap;			//Track -> StripHitId

  void Register();
  
  void Reset();
  
  void ProduceHits();
  
  std::map<int, std::vector<int> > AssignHitsToTracks();
  
  

  ClassDef(PndMvdMSAnaTask,1);

};

#endif
