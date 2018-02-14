//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      An ideal pattern recognition
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Sebastian Neubert    TUM            (original author)
//
//      Alicia Sanchez HIM : changed according to hypernuclear project
//                           requirements
//-----------------------------------------------------------

#ifndef PNDHYPIDEALPRTASK_HH
#define PNDHYPIDEALPRTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"

#include "PndMCTrack.h"
#include "PndGeoHypPar.h"
#include "PndHypHit.h"
#include "PndHypPoint.h"

#include "../../pnddata/SdsData/PndSdsMCPoint.h"
#include "../../pnddata/SdsData/PndSdsHit.h"

#include "GFTrackCand.h"

#include <string>
#include <vector>
#include "TH1.h"
#include<map>

// Collaborating Class Headers -------


// Collaborating Class Declarations --
class TClonesArray;
class GFRecoHitFactory;


class PndHypIdealPRTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndHypIdealPRTask();
  ~PndHypIdealPRTask();

  // Operators
  

  // Modifiers -----------------------
  //void SetPointBranchName(const TString& name) {fPointBranchName=name;}
  void AddHitBranch(unsigned int detId, const TString& m){fHitBranchNameMap[detId]=m;};
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;}

  // Operations ----------------------
  
    
  /** Virtual method Init **/
  //virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();
    
  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);
  
   void WriteHistograms();

private:

  // Private Data Members ------------
  //TString _pointBranchName;
  TClonesArray* fHitArray;
  TClonesArray* fPointArray;
  TClonesArray* fSdsArray;
  TClonesArray* fTrackArray;
  TClonesArray* fMcArray;
  
  Bool_t fPersistence;
  

  std::map<unsigned int,TString> fHitBranchNameMap;
  std::map<unsigned int,TClonesArray*> fHitBranchMap;
  std::map<Int_t, GFTrackCand*> fTrackCandMap;
   int fEventNr;
   TH1D* fPH; // momentum histo;
   Int_t GetChargeIon(Int_t ion);
   void AddHitToTrack(Int_t trackID, Int_t detnum, Int_t iHit);
   void ClearTrackCandMap();

  GFRecoHitFactory* fTheRecoHitFactory;
 
  // Private Methods -----------------
  
  

  ClassDef(PndHypIdealPRTask,2)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
