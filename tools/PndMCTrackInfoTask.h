
/** PndMCTrackInfoTask.h
 **
 **/

#ifndef PndMCTrackInfoTask_H
#define PndMCTrackInfoTask_H

#include "PndPersistencyTask.h"
#include "PndMCTrackInfo.h"
#include "PndMCTrack.h"

#include "TClonesArray.h"

#include <string>
#include <vector>
#include <map>

class TClonesArray;

class PndMCTrackInfoTask : public PndPersistencyTask
{
 public:

  /** Default constructor **/
  PndMCTrackInfoTask();

  /** Destructor **/
  virtual ~PndMCTrackInfoTask();


  /** Virtual method Init **/
  virtual void SetParContainers();
  virtual InitStatus Init();
  virtual InitStatus ReInit();

  /** Virtual method Exec **/
  virtual void Exec(Option_t* opt);

  virtual void FinishEvent();
  virtual void FinishTask();

  void SetMaxStage(int stage) { fMaxStage = stage; }
  void AddBranchName(TString name){ fBranches[name]; }

protected:

  void Register();
  void Reset();

  std::vector<int> GetPIDMothers(int motherID);
  void AssignPoints();
  void FillOutputArray();
  void FillPointInfo();

private:

  TClonesArray* fMCTracks;
  TClonesArray* fMCTrackInfo;
  std::map<TString, TClonesArray*> fBranches;
  std::map<int, PndMCTrackInfo> fTrackInfo;
  int fMaxStage;                                ///< maximum number of mothers recorded

  ClassDef(PndMCTrackInfoTask,8);

};

#endif
