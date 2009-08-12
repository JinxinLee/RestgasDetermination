#ifndef PNDSTORETCANDS_H
#define PNDSTORETCANDS_H 1


#include "FairTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;


class PndStoreTCands : public FairTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/  
  PndStoreTCands();


  /** Destructor **/
  ~PndStoreTCands();


  /** Virtual method Init **/
  virtual InitStatus Init();


  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();
  //void CreateStructure();

 protected:
  
  void propagate(TLorentzVector &l, TVector3 &p, float charge);
  
  TLorentzVector track1; 
  TLorentzVector track2; 

  /**book all the histograms**/

 private: 
  int       evtcnt; //event counter for output
   
  /** Input array of LheTrack **/
  TClonesArray* fTrArray;
  
  /** Input array of EmcCandidates **/
  TClonesArray* fEmcArray;
  
  /** Input array of MCTracks **/
  TClonesArray* fMCTrack;
  
  // Output array
  TClonesArray* fChargedCandidates;
  TClonesArray* fNeutralCandidates; 
  TClonesArray* fMcCandidates;
  TClonesArray* fMicroCandidates;

  bool fStoreNeutral;
  bool fStoreCharged;
  bool fStoreMC;
  
  Int_t mvd_hitidx[1000];
  Int_t stt_hitidx[1000];
  Int_t tpc_hitidx[1000];
  
 
  /** Geo file to use **/

	/** Get parameter containers **/
	virtual void SetParContainers();

 
  ClassDef(PndStoreTCands,1);
  
};

#endif
