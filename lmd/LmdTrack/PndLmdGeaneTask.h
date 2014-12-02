// -------------------------------------------------------------------------
// -----                   PndLmdGeaneTask header file                 -----
// -----                  Created 20/03/07  by R.Kliemt                -----
// -----                  modified for Lmd by M. Michel & A.Karavdina  -----
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
//#include "GFTrackCand.h"
#include "FairGeanePro.h"
#include "TMatrixD.h"
#include "TVector3.h"
#include "TNtuple.h"
#include "FairTrackParP.h"
//#include "PndSdsGeoHandling.h"
#include "PndGeoHandling.h"
#include <vector>
#include <map>
#include "PndMultiField.h"
class TClonesArray;
class PndSdsCluster;
//class TGeant3;//GEANE uses Geant3!
//class TGeant4;

class PndLmdGeaneTask : public FairTask
{
 public:

  /** Default constructor **/  
  PndLmdGeaneTask();

  ///Set up beam momuntum value and position of interaction point
  PndLmdGeaneTask(Double_t pBeam, TVector3 IP);

  /** Destructor **/
  ~PndLmdGeaneTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
  virtual void Finish();

  FairTrackParP* PropToPlane(FairTrackParP* fStartPst, double zpos,int dir,bool& isProp);//propagation between planes
  FairTrackParP* PropToXZPlane(FairTrackParP* fStartPst, double zpos,int dir,bool& isProp);//propagation between planes
 FairTrackParH* PropToLine(FairTrackParH* fStartPst, double zpos,int dir,bool& isProp);//propagation to line
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
  Int_t fPDGid;
  TVector3 vtx;
  FairField *pndField;		
  std::map<int, std::vector<int> > fTrackPixHitIdMap;	 //Track -> PixHitId
  std::map<int, std::vector<int> > fTrackStripHitIdMap;	 //Track -> StripHitId

  // Input Data ----------
  TClonesArray* fMCHits;
  TClonesArray* fMCTracks;

  TClonesArray* fTracks; //Real tracks
  TClonesArray* fHits; //Real hits


  TClonesArray* fRecHits;   //needed for tests only!
  TClonesArray* fRecCandTracks;   //needed for tests only!


  // Output Data -------
  

  //----------------------------------------

  void Register();
  
  void Reset();
  
  void ProduceHits();
  
  std::map<int, std::vector<int> > AssignHitsToTracks();
  ClassDef(PndLmdGeaneTask,1);

};

#endif
