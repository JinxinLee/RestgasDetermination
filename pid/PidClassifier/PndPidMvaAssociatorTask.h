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

// Standard C++ includes
#include <string>
#include <vector>

// Root includes.
class TClonesArray;

// PANDA and Fair includes.
#include "FairTask.h"
class PndPidCandidate;
class PndPidProbability;

// MVA headers
class PndMvaClassifier;

// ========================================================================
//! Method types for selecting which classifier to use.
typedef enum 
  {
    UNKNOWN_METHOD = 0,
    KNN = 1,// KNN methode
    LVQ = 2, // LVQ
    TMVA_MLP = 3, // Multi label TMVA ANN.
    TMVA_BDT = 4 // Multi label TMVA BDT.
  } Mva_MethodType;

// ========================================================================
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
  PndPidMvaAssociatorTask(char const* name);

  /**
   * Destructor.
   */  
  virtual ~PndPidMvaAssociatorTask();
  
  /**
   * Execute the method.
   */
  virtual void Exec(Option_t* option);
  
  /**
   * Initialize the method. Called by runner.
   */
  virtual InitStatus Init();
  
  void Register();
  
  void Reset();
  
  inline void SetVerbose(Bool_t verb);
  
  /**
   * Set parameter containers
   */
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
  
  /**
   * Set the classifier type.
   *@param methodT The method to be used.
   */
  inline void SetClassifier(Mva_MethodType const& methodT);

  /**
   * Set the classifier type.
   *@param methodName The method to be used.
   */
  void SetClassifier(std::string const& methodName);

  //______________ KNN- Classifier parameter _____________
  /**
   *@param val nNumber of neighbors to be uset for KNN classifier.
   */
  inline void SetNumNeigh(size_t val);
  
  /**
   * Set the scale factor and the event weight for KNN classifier.
   * @param scFact  Scale factor.
   * @param weight  Events weight.
   */
  inline void SetKnnEventParams(float scFact,  double weight);

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
  /* Set the default path where the weights are stored. */
  void SetDefaultWeightsPath();

  //! Variable names container.
  std::vector<std::string> fVarNames;
  
  //! Class names container.
  std::vector<std::string> fClassNames;
  
  //! Path to the file holding weights (proto-types, examples, ...)
  std::string fWeightsFileName;
  
  //! Number of neighbors
  size_t fNumNeigh;
  float  fScFact;
  double fWeight;

  //! MVA classifier object.
  PndMvaClassifier* fClassifier;

  //! MVA Method name.
  Mva_MethodType fMethodType;

  TClonesArray* fPidChargedCand; //! PndPidCandidate TCA for charged particles
  TClonesArray* fPidChargedProb; //! PndPidProbability TCA for charged particles
  TClonesArray* fMCTrack;        //! Monte-Carlo Truth track TCA
  // TClonesArray* fPidNeutralCand; //! PndPidCandidate TCA for neutral particles
  // TClonesArray* fPidNeutralProb; //! PndPidProbability TCA for neutral particles
  std::string   fMethodName;

  ClassDef(PndPidMvaAssociatorTask, 0);
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

inline void PndPidMvaAssociatorTask::SetNumNeigh(size_t val)
{
  fNumNeigh = val;
};

inline void PndPidMvaAssociatorTask::SetClassifier(Mva_MethodType const& methodT)
{
  fMethodType = methodT;
};

inline void PndPidMvaAssociatorTask::SetKnnEventParams(float scFact, double weight)
{
  fScFact = scFact;
  fWeight = weight;
};
#endif//End of interface definition (PndPidMvaAssociatorTask)
