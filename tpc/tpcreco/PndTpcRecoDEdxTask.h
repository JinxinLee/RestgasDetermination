//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      store dedx for reconstructed tracks
// 
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Viola Michael    TUM            (original author)
//
//
//-----------------------------------------------------------

#ifndef PNDTPCRECODEDXTASK_HH
#define PNDTPCRECODEDXTASK_HH

// Base Class Headers ----------------
#include "CbmTask.h"
#include <vector>

// Collaborating Class Headers -------

// Collaborating Class Declarations --
class TClonesArray;
class TH2F;
class PndTpcCluster;
class PndTpcRawDEdxCollection;
class PndTpcDEDXFits;
class PndTpcDEDXStorageHelper;

class PndTpcRecoDEdxTask : public CbmTask {
public:

  // Constructors/Destructors ---------
  PndTpcRecoDEdxTask();
  virtual ~PndTpcRecoDEdxTask();

  // Operators
  

  // Accessors -----------------------

  
  // Modifiers -----------------------
  void SetPersistence(Bool_t opt=kTRUE) {_persistence=opt;}
  void SetSpatialSorting(Bool_t opt=kTRUE) {_spatialSorting=opt;}
  void SetDiagnosticOutput(Bool_t opt=kTRUE) {_diagnosticOutput=opt;}
  
  void SetTrackBranchName(const TString& name) {_trackBranchName=name;}
  void SetClusterBranchName(const TString& name) {_clusterBranchName=name;}
  
  // Set cuts
  void SetPmin(Double_t pmin)	{_pmin=pmin;}
  void SetPmax(Double_t pmax)	{_pmax=pmax;}
  void SetMinTpcHists(Int_t min)	{_minTpcHits=min;}
  void SetMaxTpcHists(Int_t max)	{_maxTpcHits=max;}  
  void SetPdgSelection(Int_t pdgcode)	{_pdgId=pdgcode;_pdgselect=true;}
  void SetMCID(Int_t mcid)			{_mcID=mcid;_mcIDselect=true;}
  
  //Set PID Files
  void SetBBFitsFile(const TString& filename)	{ BBFitFile=filename;	}
  void SetResolutionFile(const TString& filename) { ResoFile=filename;	}
  
  // Operations ----------------------
  virtual InitStatus Init();
  virtual void Exec(Option_t* opt);
   
  void WriteHistograms(const TString& filename);

private:

  // Private Data Members ------------
  Bool_t _persistence;
  Bool_t _spatialSorting;	// PrepareClusterList will try to sort clusters along track if true
  Bool_t _diagnosticOutput;	// an additional output class will be created
  
   //requiered inforamtion for dEdx
  TString _trackBranchName;
  TString _clusterBranchName;
  TClonesArray* _clusterArray;
  TClonesArray* _trackArray;
  
   //access additional inforamtion
  TString _mcBranchName;	
  TString	_mcTpcBranchName; 
  TString _trackFitStatBranchName;
  TClonesArray* _mcTrackArray;
  TClonesArray* _mcTpcHitArray;
  TClonesArray* _trackFitStatArray;
  
   //Output Array
  TClonesArray* _dEdxArray;
  //optional output array for debbuging
  TClonesArray* _diaArray;
  
  PndTpcDEDXFits *_pid;
  
   //some cuts
  bool _pdgselect;
  Int_t _pdgId;
  bool _mcIDselect;
  Int_t _mcID;
  Double_t _pmin;
  Double_t _pmax;
  Double_t _thetamin;
  Double_t _thetamax;
  Int_t _minTpcHits;
  Int_t _maxTpcHits;

  //some histograms for direct printing
  TH2F* _HistoDEdx;
  TH2F* _HistoSumDESumdx;
  TH2F* _HistoTruncSumDESumdx;
  
  //output infos
  Double_t P;
  Double_t MCP;
  Int_t PDG;
  Int_t TrackNR;
  std::vector<int> MCTrackNR;
  Int_t MotherID;
  bool bSorted;	//flag indicates, if clusters are sorted in z
  
  //PIDFiles
   TString BBFitFile;
   TString ResoFile;
   
   int nEvent;
  
  // Private Methods -----------------
  void ValidateArrays() const;
  
  bool CheckRecoMomentum(Double_t P) const;
  bool CheckHits(unsigned int nHits) const;
  bool CheckDetector(unsigned int DetID) const;
  bool CheckOrder(const std::vector<Double_t> &check) const;
  
  bool CheckClusterMCID(const PndTpcCluster &cl) const;
  Int_t GetClusterMCID(const PndTpcCluster &cl) const;
  
  void SetMCTrackNR(Int_t nMCTrackNR);
  void ResetOutputInfo();
  void FillData(const std::vector<PndTpcDEDXStorageHelper> &data, PndTpcRawDEdxCollection* Col) const;
  void PrepareClusterList(std::vector<PndTpcCluster *> &PreparedClusterList, Track *track) const;
public:
  ClassDef(PndTpcRecoDEdxTask,1)

};

#endif

//--------------------------------------------------------------
// $Log$
//--------------------------------------------------------------
