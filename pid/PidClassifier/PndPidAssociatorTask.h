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

class PndPidAssociatorTask : public FairTask
{
public:
  /**
   * Default Constructor.
   */
  PndPidAssociatorTask();
  
  /**
   * Constructor.
   */
  PndPidAssociatorTask(const char *name, const char *title="PndPidAssociatorTask");
  
  /**
   * Destructor.
   */  
  virtual ~PndPidAssociatorTask();

  virtual void Exec(Option_t* option);
  virtual InitStatus Init();
  
  void Register();
  void Reset();
  
  void SetVerbose(Bool_t verb){fVerbose = verb;};
	
  /** Set parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();
  
  // Protected members (WHY??)
 protected: 
  TClonesArray* fPidChargedCand; //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand; //! PndPidCandidate TCA for neutral particles
  TClonesArray* fPidChargedProb; //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb; //! PndPidProbability TCA for neutral particles
  TClonesArray* fMCTrack;        //! Monte-Carlo Truth track TCA
  
  // Private members.
 private:
  PndPidAssociatorTask(const PndPidAssociatorTask& other);
  PndPidAssociatorTask& operator=(const PndPidAssociatorTask& other);

  //
  void DoPidMatch(PndPidCandidate& pidcand, PndPidProbability& prob);

  FairRootManager *fManager;
  PndKnnClassify* knnCls;

  ClassDef(PndPidAssociatorTask, 1)
};
#endif//End of interface definition (PndPidAssociatorTask)
