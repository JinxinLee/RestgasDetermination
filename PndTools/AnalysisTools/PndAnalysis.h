#ifndef PNDANALYSIS_H
#define PNDANALYSIS_H 1

#include <string>
#include <vector>

#include "TString.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "TMatrixD.h"

#include "TCandList.h"
#include "FairRootManager.h"
#include "FairTrackParP.h"
class TChain;
class TClonesArray;
class TBranch;

class TDatabasePDG;
class FairMCEventHeader;
class PndAnaPidSelector;
class PndEventInfo;

class TGeant3;

class PndAnalysis {
public:
  PndAnalysis(TString tname1="", TString tname2="");
  virtual ~PndAnalysis();
  
  void Rewind();
  void Reset() { Rewind(); }
  Int_t  GetEvent(Int_t n=-1);
  FairMCEventHeader* GetEventHeader();
  Int_t  GetEntries(); 
  Bool_t FillList(TCandList &l, TString listkey="All");
  void SetVerbose(Int_t level){fVerbose = level;}
  //void SetPidChargedName(TString s) {fChargedPidName = s;}
  //void SetPidNeutralName(TString s) {fNeutralPidName = s;}  
  //void SetTracksName(TString s) {fTracksName = s;}
  //void SetTracksName2(TString s) {fTracksName2 = s;}
  Bool_t PropagateToIp(TCandidate* cand);
  Bool_t PropagateToZAxis(TCandidate* cand);
  Bool_t PropagateToPoint(TCandidate* cand, TVector3* mypoint);
  FairTrackParP GetFirstPar(TCandidate* cand);
  Bool_t ResetCandidate(TCandidate* cand);
  Bool_t ResetDaughters(TCandidate* cand);

  //FIXME: This is an aweful solution to access the correct
  //track array from a fitter object. [R.K.03'11]
  TClonesArray* GetTrackArrayPointer() const {return fTracks;};
  
private:
  
  void Init();
  void BuildMcCands();
  TClonesArray* ReadTCA(TString tcaname);
  Bool_t Propagator(int mode, FairTrackParP &tStart, TCandidate* cand, 
                    TVector3* point=NULL, Bool_t skipcov=kFALSE);
    
  // Private Member Variables
  
  FairRootManager*   fRootManager;
  PndAnaPidSelector* fPidSelector;
  Int_t              fEvtCount;
  Int_t              fChainEntries;
  Bool_t             fEventRead;
  Bool_t             fBuildMcCands;
  static Int_t       fVerbose;
  
  TDatabasePDG *fPdg;
  
  TClonesArray *fChargedCands;
  TClonesArray *fNeutralCands;
  TClonesArray *fChargedProbability;
  TClonesArray *fNeutralProbability;
  TClonesArray *fTracks;
  TClonesArray *fTracks2;
  TClonesArray *fMcCands;
  TClonesArray *fMcTracks;
  
  TCandList allCands;
  TCandList chargedCands;
  TCandList neutralCands;  
  TCandList mcCands;
  
  TString fChargedPidName;
  TString fNeutralPidName;
  TString fTracksName;
  TString fTracksName2;
  ClassDef(PndAnalysis,0);
};


#endif

