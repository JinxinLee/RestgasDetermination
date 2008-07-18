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
#include "CbmTask.h"
#include <vector>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class TH2F;
class PndTpcPoint;

class PndTpcMCDEdxTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcMCDEdxTask();
  virtual ~PndTpcMCDEdxTask();

  // Operators
  

  // Accessors -----------------------

  
  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  
  /**
  	if activeated PndTpcMCDEdxTask will use PndTpcHitMerger to
	combine hits before the de/dx information gets stored in
	PndTpcRawDEdxCollection
	This is particulary useful to investigate the output
	if the ALICE Monte Carlo is activeated in PndTpcDetector
  */
  void SetHitCombineLength(Bool_t opt=kTRUE, double length=0.5, Bool_t CatchRemaining=kFALSE) {_combineHitsLength=opt; _combineLength=length; _catchRemaining=CatchRemaining; }
  void SetHitCombineNumber(Bool_t opt=kTRUE, unsigned int number=10, Bool_t CatchRemaining=kFALSE) { _combineHitsNumber=opt; _combineNumber=number; _catchRemaining=CatchRemaining;	}
  
  void SetMCTrackBranchName(const TString& name) {_mcTrackBranchName=name;}
  void SetClusterBranchName(const TString& name) {_mcPointBranchName=name;}
  void SetPmin(const Double_t& pmin)	{_pmin=pmin;}
  void SetPmax(const Double_t& pmax)	{_pmax=pmax;}
  void SetMinTpcHists(const Int_t& min)	{_minTpcHits=min;}
  void SetMaxTpcHists(const Int_t& max)	{_maxTpcHits=max;}  
  void SetPdgSelection(Int_t pdgcode){_pdgId=pdgcode;_pdgselect=true;}
  
  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
   
  void WriteHistograms(const TString& filename);

private:
  // Private Data Members ------------
  Bool_t _persistence;
  Bool_t _combineHitsLength;
  Bool_t _combineHitsNumber;
  Bool_t _catchRemaining;
  double _combineLength;
  unsigned int _combineNumber;
  
   //requiered inforamtion for dEdx
  TString _mcTrackBranchName;
  TString _mcPointBranchName;

  //inforamtion to cut on
  TClonesArray* _mcTpcHitArray;
  TClonesArray* _mcTrackArray;
  
   //Output Array
  TClonesArray* _dEdxArray;
  
   //some cuts
   
  bool _pdgselect;
  Int_t _pdgId;
  Double_t _pmin;
  Double_t _pmax;
  Double_t _thetamin;
  Double_t _thetamax;
  Int_t _minTpcHits;
  Int_t _maxTpcHits;

  //Output histograms
  TH2F* _HistoDEdx;
  TH2F* _HistoSumDESumdx;
  TH2F* _HistoTruncSumDESumdx;
  
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
