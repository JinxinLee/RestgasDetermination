// -------------------------------------------------------------------------
// -----                      PndMCTrackInfo header file                   -----
// -----                  Created 16/09/19  by T.Stockmann             -----
// -------------------------------------------------------------------------


/** PndMCTrackInfo.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 ** Data class to contain additional information for an MCTrack
 **
 **/


#ifndef PNDMCTRACKINFO_H
#define PNDMCTRACKINFO_H 1

#include "FairMultiLinkedData_Interface.h"

#include "TVector3.h"
#include <iostream>

class PndMCTrackInfo : public FairMultiLinkedData_Interface
{

 public:


  /**  Default constructor  **/
  PndMCTrackInfo();


  /**  Destructor  **/
  virtual ~PndMCTrackInfo();


  /**  Output to screen  **/
  void Print();

  friend std::ostream& operator<< (std::ostream& out, PndMCTrackInfo& track) {
	track.Print();
	return out;
  }


  /**  Accessors  **/
  std::vector<int>      GetDaughters();
  TVector3              GetDecayVertex() const { return fDecayVertex; }
  Double_t              GetDecayTime()   const { return fDecayTime; }
  Int_t                 GetStage()       const { return fStage; }
  std::vector<int>      GetPIDMothers()  const { return fPIDMothers; }
  std::vector<int>      GetPIDDaughters()const { return fPIDDaughters; }
  Int_t                 GetNHits(TString branchName);
  
  // Setters
  void AddDaughter      (FairLink doughter);
  void AddPIDDaughter   (int pid) {fPIDDaughters.push_back(pid); }
  void SetPIDMothers    (std::vector<int> pid) { fPIDMothers = pid; fStage = fPIDMothers.size();}
  void SetDecayVertex   (TVector3 vertex) {fDecayVertex = vertex;}
  void SetDecayTime     (Double_t time) {fDecayTime = time;}

  void FillPoints();

 protected:


private:

  TVector3  fDecayVertex;
  Double_t  fDecayTime;
  std::vector<int> fPIDMothers;
  std::vector<int> fPIDDaughters;
  std::vector<int> fDaughters;
  Int_t fStage;
  Int_t fMVDPoints;
  Int_t fSTTPoints;
  Int_t fGEMPoints;
  Int_t fFTSPoints;
  Int_t fTotalPoints;

  ClassDef(PndMCTrackInfo,4);

};

#endif
