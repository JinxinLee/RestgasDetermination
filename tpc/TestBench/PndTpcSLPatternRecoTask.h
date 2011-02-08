//-----------------------------------------------------------
// File and Version Information:
// $Id$
//
// Description:
//      Straight line pattern recognition using a 2-dim
//      Projection. Internally, a Hough Transform is used.
//      
//
// Environment:
//      Software developed for the PANDA Detector at FAIR.
//
// Author List:
//      Felix Boehmer           TUM      (original author)  
//
//-----------------------------------------------------------

#ifndef TPCSLRPRTASK_H
#define TPCSLRPRTASK_H

//Base class
#include "FairTask.h"
#include "FairField.h"

#include "TClonesArray.h"
#include "TString.h"
#include "TF1.h"
#include "TStyle.h"

#include <vector>




// Collaborating Class Declarations ------------------------

class PndTpcCluster;
class Hough2DNode;
class PndTpcDigiPar;
class FairField;
class TFile;

class PndTpcSLPatternRecoTask : public FairTask {

 public:
  
  // Constructor ----------------------------------------------
  
  PndTpcSLPatternRecoTask();
  ~PndTpcSLPatternRecoTask();
  
  virtual InitStatus Init();	 
  virtual void Exec(Option_t* opt);
  
  
  // Setters --------------------------------------------------
  
  void SetPersistence(Bool_t opt=kTRUE) {fPersistence=opt;} 
  void SetClusterBranchName(TString cln) {fClusterBranchName=cln;}
  void SetTrackBranchName(TString tn) {fTrackBranchName=tn;}

  void SetParameterSpace(double* mins, double* maxs);
  
  void SetDepth(int depth) {fDepth=depth;}
  void SetThresh(int thresh) {fThresh=thresh;}
  void SetMinCandHits(int min) {fMin=min;}
  void SetClusterAmpCut(double c) {fAmpCut=c;}
  void SetCutTracksParallelZ(unsigned int limit) {fZStackLimit=limit;}
  void SetNClusterLimit(unsigned int ncl) {fClLimit=ncl;}
  void SetAbsMomentum(unsigned int s) {fMomScale=s;}  //set absolute momentum
  
  void SetDistSorting(bool opt) {fDistSorting=opt;}
  void SetXSorting(bool opt) {fXSorting=opt;}

  //void SetProjectionXZ() {fXZ=true;fZY=false;fXY=false;} //find lines in X-Z
  //void SetProjectionZY() {fZY=true;fXZ=false;fXY=false;} //find lines in Z-Y (cosmics)
  //void SetProjectionXY() {fZY=false;fXZ=false;fXY=true;} //find lines in X-Y(CERN?)
  
  void CutSmallPad(Bool_t opt=kTRUE) {_cutsmallpad=opt;}
  void CutBigPad(Bool_t opt=kTRUE) {_cutbigpad=opt;}

  void SetStoreHistograms(TString file);
  void SetDebugOutput(Bool_t opt=true) {fDebug=opt;}
 
  
 private:

  void KillHistograms(std::map<std::string, TH1*>&);

  double fMins[4];
  double fMaxs[4];
  double fAmpCut;

  unsigned int fDepth;    //termination depth of the FHT search
  unsigned int fThresh;   //STATIC threshold of the FTH search 
  unsigned int fMin;      //minimum track candidate size
  
  Bool_t fPersistence;
  Bool_t fDistSorting;
  Bool_t fXSorting;
  Bool_t fXZ;
  Bool_t fZY;
  Bool_t fXY;
  Bool_t _cutbigpad;
  Bool_t _cutsmallpad ;
  Bool_t fStore;
  Bool_t fDebug;

  TString fClusterBranchName;
  TString fTrackBranchName;
  TString fHistoFileName;

  TFile* fHistoFile;

  unsigned fClLimit; //limit nClusters an event may have
  
  TClonesArray* fClusterArray;
  TClonesArray* fTrackArray;
  TClonesArray* fMonitorArray;
    
  PndTpcDigiPar* fPar;
  TF1* fRep; //Hough space representation
  FairField* fField;

  int counter;
  unsigned int fZStackLimit;
  unsigned int fMomScale;

  std::vector<Color_t> colors;
  std::map<std::string, TH1*> fHistCont;   //container for histograms


  // Private methods -----------------------------------------
  //bool compareNodes (Hough2DNode* n1, Hough2DNode* n2); 

  
 public:
  ClassDef(PndTpcSLPatternRecoTask,3)
	
};

#endif
