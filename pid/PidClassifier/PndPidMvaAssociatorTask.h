/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 23-03-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
#ifndef PID_ASSOCIATOR_H
#define PID_ASSOCIATOR_H

//standard C++ includes
#include <iostream>

// Root includes.
#include "TClonesArray.h"

// PANDA and Fair includes.
#include "FairTask.h"
#include "FairRootManager.h"
#include "PndMCTrack.h"
#include "PndPidCandidate.h"
#include "PndPidProbability.h"

#include "PndKnnClassify.h"
#include "PndLVQClassify.h"

class PndPidMvaAssociatorTask: public FairTask
{
public:
  /**
   * Default Constructor.
   */
  PndPidMvaAssociatorTask();
  
  /**
   * Constructor.
   */
  PndPidMvaAssociatorTask(const char *name, const char *title="PndPidAssociatorTask");
  
  /**
   * Destructor.
   */  
  virtual ~PndPidMvaAssociatorTask();

  virtual void Exec(Option_t* option);
  virtual InitStatus Init();
  
  void Register();
  void Reset();
  
  void SetVerbose(Bool_t verb){fVerbose = verb;};
	
  /** Set parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();
  
  void SetVarNames(const std::vector<std::string>& vNames)
  {fVarNames = vNames;}

  void SetClassNames(const std::vector<std::string>& clNames)
  {fClassNames = clNames;}

  void SetWeightFileName(const std::string& wFileName)
  {fWeightsFileName = wFileName;}

  //============== Protected members.
  //(WHY?? protected)
 protected: 
  TClonesArray* fPidChargedCand; //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand; //! PndPidCandidate TCA for neutral particles
  TClonesArray* fPidChargedProb; //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb; //! PndPidProbability TCA for neutral particles
  TClonesArray* fMCTrack;        //! Monte-Carlo Truth track TCA
  
  //=============== Private members.
 private:
  PndPidMvaAssociatorTask(const PndPidMvaAssociatorTask& other);
  PndPidMvaAssociatorTask& operator=(const PndPidMvaAssociatorTask& other);

  void DoPidMatch(PndPidCandidate& pidcand, PndPidProbability& prob);

  FairRootManager *fManager;

  // Classifiers variables and functions
  //
  void SetDefaultWeightsPath();
  const std::vector<float>& PrepareEvtVect(const PndPidCandidate& pidcand)const;
  //
  std::vector<std::string> fVarNames;
  std::vector<std::string> fClassNames;
  std::string fWeightsFileName;
  PndKnnClassify* fKnnCls;

  ClassDef(PndPidMvaAssociatorTask, 1)
};
#endif//End of interface definition (PndPidAssociatorTask)
