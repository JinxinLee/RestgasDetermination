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
  PndPidMvaAssociatorTask(char const* name, char const* title="PndPidMvaAssociatorTask");
  
  /**
   * Destructor.
   */  
  virtual ~PndPidMvaAssociatorTask();
  
  virtual void Exec(Option_t* option);
  
  virtual InitStatus Init();
  
  void Register();
  
  void Reset();
  
  inline void SetVerbose(Bool_t verb);
  
  /** Set parameter containers **/
  virtual void SetParContainers();
  
  virtual void Finish();
  
  /**
   *@param vNames Input variable names.
   */
  inline virtual void SetVarNames(std::vector<std::string> const& vNames);
  
  /**
   *@param clNames Input class names.
   */
  inline void SetClassNames(std::vector<std::string> const& clNames);
  
  /**
   *@param wFileName Input weight file. If not specified the standard
   * file from the standard location is loaded.
   */
  inline void SetWeightFileName(std::string const& wFileName);
  
  inline void SetNumNeigh(int val);

  inline void BookClassifier(std::string const& methodName);
  
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
  
  //! Number of neighbors
  int fNumNeigh;

  //! MVA classifier object.
  PndMvaClassifier* fClassifier;

  //! MVA Method name.
  std::string fMethodName;

  TClonesArray* fPidChargedCand; //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidNeutralCand; //! PndPidCandidate TCA for neutral particles
  TClonesArray* fPidChargedProb; //! PndPidProbability TCA for charged particles
  TClonesArray* fPidNeutralProb; //! PndPidProbability TCA for neutral particles
  TClonesArray* fMCTrack;        //! Monte-Carlo Truth track TCA
  
  ClassDef(PndPidMvaAssociatorTask, 1);
};

inline void PndPidMvaAssociatorTask::SetVerbose(Bool_t verb)
{
  fVerbose = verb;
};

inline void PndPidMvaAssociatorTask::SetVarNames(std::vector<std::string> const& vNames)
{
  fVarNames = vNames;
};

inline void PndPidMvaAssociatorTask::SetClassNames(std::vector<std::string> const& clNames)
{
  fClassNames = clNames;
};

inline void PndPidMvaAssociatorTask::SetWeightFileName(std::string const& wFileName)
{
  fWeightsFileName = wFileName;
};

inline void PndPidMvaAssociatorTask::SetNumNeigh(int val)
{
  fNumNeigh = val;
};

inline void PndPidMvaAssociatorTask::BookClassifier(std::string const& methodNameStr)
{
  fMethodName = methodNameStr;
};
#endif//End of interface definition (PndPidMvaAssociatorTask)
