//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      store dedx for MC tracks in a class called 
//	   PndTpcRawDEdxCollection	 
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCMCDEDXTASK_HH
#define PNDTPCMCDEDXTASK_HH

// Base Class Headers ----------------
#include "FairTask.h"
#include <vector>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class TH2F;
class PndTpcPoint;

class PndTpcMCDEdxTask : public FairTask {
public:

  // Constructors/Destructors ---------
  PndTpcMCDEdxTask();
  virtual ~PndTpcMCDEdxTask();

  // Operators
  

  // Accessors -----------------------

  
  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {fpersistence=opt;}
  
  /**
  	if activeated PndTpcMCDEdxTask will use PndTpcHitMerger to
	combine hits before the de/dx information gets stored in
	PndTpcRawDEdxCollection
	This is particulary useful to investigate the output
	if the ALICE Monte Carlo is activeated in PndTpcDetector
  */
  void SetHitCombineLength(Bool_t opt=kTRUE, double length=0.5, Bool_t CatchRemaining=kFALSE) {fcombineHitsLength=opt; fcombineLength=length; fcatchRemaining=CatchRemaining; }
  void SetHitCombineNumber(Bool_t opt=kTRUE, unsigned int number=10, Bool_t CatchRemaining=kFALSE) { fcombineHitsNumber=opt; fcombineNumber=number; fcatchRemaining=CatchRemaining;	}
  
  // it is useful for the ALICE-MC because the step max in the PndTpcDetector is
  // called not for the first hit
  void SetStartHit(int StartHit) {fstartHit=StartHit;}	// useful values are 0 or 1

  void SetMCTrackBranchName(const TString& name) {fmcTrackBranchName=name;}
  void SetClusterBranchName(const TString& name) {fmcPointBranchName=name;}
  void SetPmin(const Double_t& pmin)	{fpmin=pmin;}
  void SetPmax(const Double_t& pmax)	{fpmax=pmax;}
  void SetMinTpcHists(const Int_t& min)	{fminTpcHits=min;}
  void SetMaxTpcHists(const Int_t& max)	{fmaxTpcHits=max;}  
  void SetPdgSelection(Int_t pdgcode){fpdgId=pdgcode;fpdgselect=true;}
  
  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
   
  void WriteHistograms(const TString& filename);

private:
  // Private Data Members ------------
  Bool_t fpersistence;
  Bool_t fcombineHitsLength;
  Bool_t fcombineHitsNumber;
  Bool_t fcatchRemaining;
  double fcombineLength;
  unsigned int fcombineNumber;
  int fstartHit;	
  
   //requiered inforamtion for dEdx
  TString fmcTrackBranchName;
  TString fmcPointBranchName;

  //inforamtion to cut on
  TClonesArray* fmcTpcHitArray;
  TClonesArray* fmcTrackArray;
  
   //Output Array
  TClonesArray* fdEdxArray;
  
   //some cuts
   
  bool fpdgselect;
  Int_t fpdgId;
  Double_t fpmin;
  Double_t fpmax;
  Double_t fthetamin;
  Double_t fthetamax;
  Int_t fminTpcHits;
  Int_t fmaxTpcHits;

  //Output histograms
  TH2F* fHistoDEdx;
  TH2F* fHistoSumDESumdx;
  TH2F* fHistoTruncSumDESumdx;
  
  // Private Methods -----------------
  bool CheckMomentum(Double_t P) const;
  bool CheckHits(unsigned int nHits) const;
  void ValidateArrays() const;
  bool CheckPDG(Int_t nPDG) const;
  void GetHitsInTrack(int nTrackNr, std::vector<PndTpcPoint*> &Hits) const;
public:                                                                         
  ClassDef(PndTpcMCDEdxTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
