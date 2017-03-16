// -------------------------------------------------------------------------
// -----                 PNDMCMATCHSELECTORTASK header file             -----
// -----                  Created 18/01/10  by T.Stockmanns             -----
// -------------------------------------------------------------------------


/** PNDMCMATCHSELECTORTASK.h
 *@author T.Stockmanns <t.stockmanns@fz-juelich.de>
 **
 ** Displays all available informations for a given event
 **/


#ifndef OnlineDBuilderTask_H
#define OnlineDBuilderTask_H


// framework includes
#include "FairTask.h"
#include "PndDetectorList.h"
#include "PndTrackCand.h"
#include "TH2.h"
#include "THStack.h"

#include "TLorentzVector.h"


#include <vector>
#include <map>

class TClonesArray;
class PndTrack;
class FairTrackParP;

class OnlineDBuilderTask : public FairTask
{
 public:

  /** Default constructor **/
	OnlineDBuilderTask(TString trackBranchName);

  /** Destructor **/
  virtual ~OnlineDBuilderTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual void AddTrackBranch(TString trackBranchName){
	  fTrack[trackBranchName] = 0;
  }



  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void Finish();

  //! Sets the branch name of the track data which should be analyzed.
 // void SetTrackBranchName(TString name) {fTrackBranchName = name;} //alternatively PndTrackCandData can be analysed. Therefore pndTrackData has to be set to kFALSE

 private:
//
//  virtual void FillMapTrackQualifikation();


  /** Analyses the track data and assigns quality indicator to track.
   * Quality indicators:
   * 1  : Not assigned
   * 2  : All hits of the MC track were found and no additional once
   * 3  : Some hits of the MC track were found and no additional once
   * 4  : 70 % of all hits found belong to this MC track
   */


  std::vector<std::pair<TLorentzVector, int> > CombineFirstWithTwoSecond(std::vector<PndTrack*> first, std::vector<PndTrack*> second);
  TVector3 CorrectTrackParForPrimaryVertex(FairTrackParP input);
  std::map<TString, TClonesArray*> fTrack; //!
  TClonesArray* fMCTrack;

  TClonesArray* fDPlusArray;
  TClonesArray* fDMinusArray;

  TClonesArray* fDPlusArrayTrue;
  TClonesArray* fDMinusArrayTrue;


  FairRootManager* ioman;

  void Register();

  void Reset();


  ClassDef(OnlineDBuilderTask,1);

};

#endif
