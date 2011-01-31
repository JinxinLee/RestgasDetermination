/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Created: 23-03-2010                *
 * Modified:                          *
 *                                    *
 * ************************************/
#ifndef PID_MVA_ASSOCIATOR_H
#define PID_MVA_ASSOCIATOR_H

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
//#include "PndLVQClassify.h"

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
  PndPidMvaAssociatorTask(char const* name, char const* title="PndPidMvaAssociatorTask");
  
  /**
   * Destructor.
   */  
  virtual ~PndPidMvaAssociatorTask();
  
  virtual void Exec(Option_t* option);
  
  virtual InitStatus Init();
  
  void Register();
  
  void Reset();
  
  void SetVerbose(Bool_t verb)
  {fVerbose = verb;};
  
  /** Set parameter containers **/
  virtual void SetParContainers();
  
  virtual void Finish();
  
  /**
   *@param vNames Input variable names.
   */
  virtual void SetVarNames(std::vector<std::string> const& vNames)
  {fVarNames = vNames;};
  
  /**
   *@param clNames Input class names.
   */
  void SetClassNames(std::vector<std::string> const& clNames)
  {fClassNames = clNames;};
  
  /**
   *@param wFileName Input weight file. If not specified the standard
   * file from the standard location is loaded.
   */
  void SetWeightFileName(std::string const& wFileName)
  {fWeightsFileName = wFileName;};
  
  void SetNumNeigh(int val)
  {fNumNeigh = val;};
  
  //=============== Private members.
 private:
  // Copy const.
  PndPidMvaAssociatorTask(PndPidMvaAssociatorTask const& other);
  // Assignment operator.
  PndPidMvaAssociatorTask& operator=(PndPidMvaAssociatorTask const& other);
  
  /**
   * Performs the actual classification.
   *@param pidcand Current pid candidate to be classified.
   *@param prob    Output probabilities.
   */
  void DoPidMatch(PndPidCandidate& pidcand, PndPidProbability& prob);
  
  std::vector<float> const* PrepareEvtVect(PndPidCandidate const& pidcand) const;
  
  FairRootManager *fManager;
  
  // Classifiers variables and functions
  void SetDefaultWeightsPath();

  //! Variable names container.
  std::vector<std::string> fVarNames;
  
  //! Class names container.
  std::vector<std::string> fClassNames;
  
  //! Path to the file holding weights (proto-types, examples, ...)
  std::string fWeightsFileName;
  
  //! MVA classifier object.
  PndKnnClassify* fKnnCls;
  
  //! Number of neighbors
  int fNumNeigh;
  
  TClonesArray* fPidChargedCand; //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand; //! PndPidCandidate TCA for neutral particles
  TClonesArray* fPidChargedProb; //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb; //! PndPidProbability TCA for neutral particles
  TClonesArray* fMCTrack;        //! Monte-Carlo Truth track TCA
  
  ClassDef(PndPidMvaAssociatorTask, 1);
};
#endif//End of interface definition (PndPidMvaAssociatorTask)
