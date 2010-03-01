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

// Root includes.
#include "TClonesArray.h"

// PANDA and Fair includes.
#include "FairTask.h"
#include "FairRootManager.h"

class PndPidCandidate;
class PndPidProbability;
//C++ Libs.

class PndPidAssociatorTask : public FairTask
{
public:
  /**
   * Constructor.
   */
  PndPidAssociatorTask(const char *name, const char *title="PndPidTask");

  /**
   * Default Constructor.
   */
  PndPidAssociatorTask();

  /**
   * Destructor.
   */  
  virtual ~PndPidAssociatorTask();

  virtual void Exec(Option_t * option);
  virtual InitStatus Init();
  
  void Register();
  void Reset();
  
  void SetVerbose(Bool_t verb)
  {fVerbose = verb;};
	
  /** Get parameter containers **/
  virtual void SetParContainers();
  virtual void Finish();
  
 protected:
  
  TClonesArray* fPidChargedCand;    //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand;    //! PndPidCandidate TCA for neutral particles
  TClonesArray* fPidChargedProb;    //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb;    //! PndPidProbability TCA for neutral particles
  TClonesArray* fMCTrack;           //! Monte-Carlo Truth track TCA
  
  // At this moment still cheating, using PDG-code.
  void DoPidMatch(PndPidCandidate* pidcand, PndPidProbability* prob);
  
 private:
  PndPidAssociatorTask(const PndPidAssociatorTask& other);
  PndPidAssociatorTask& operator=(const PndPidAssociatorTask& other);

  FairRootManager *fManager;

  ClassDef(PndPidAssociatorTask, 1)   // PndPidAssociatorTask
};
#endif
