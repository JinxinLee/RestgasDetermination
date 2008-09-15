/******************************************************
Class PndMicroWriter

Collects Micro infromation from Reconstruction and
writes out PndMicroCandidates

Author: K.Goetzen, GSI, 06/2008

*******************************************************/ 


#ifndef PNDMICROWRITER_H
#define PNDMICROWRITER_H 1


#include "CbmTask.h"
#include "TH1F.h"
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include <map>
#include <string>

class TClonesArray;
class TObjectArray;


class PndMicroWriter : public CbmTask
{

 public:
	typedef std::map<Int_t, Float_t> mapper;

  /** Default constructor **/  
  PndMicroWriter();


  /** Destructor **/
  ~PndMicroWriter();


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
   
  /** Input array of Genfit Tracks **/
  TClonesArray* fTrArray;
  
  /** Input array of LHE Tracks **/
  TClonesArray* fLheTrArray;
  
  /** Input array of EmcCandidates **/
  TClonesArray* fEmcArray;
  
  /** Input array of MCTracks **/
  TClonesArray* fMCTrack;
  
  // Output arrays Candidates
  TClonesArray* fChargedCandidates;
  TClonesArray* fNeutralCandidates; 
  TClonesArray* fMcCandidates;
  TClonesArray* fMicroCandidates;
  
  //output array EventInfo
  TClonesArray* fEventInfo;

  bool fStoreNeutral;
  bool fStoreTrack;
  bool fStoreLheTrack;
  bool fStoreMC;
  
  Int_t mvd_hitidx[1000];
  Int_t stt_hitidx[1000];
  Int_t tpc_hitidx[1000];
  
 
  /** Geo file to use **/

	/** Get parameter containers **/
	virtual void SetParContainers();

 
  ClassDef(PndMicroWriter,1);
  
};

#endif
