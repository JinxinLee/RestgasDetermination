/* ***************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#pragma once
#ifndef PND_LVQ_TRAIN_H
#define PND_LVQ_TRAIN_H

#ifdef _OPENMP
#include <omp.h>
#endif

//Local includes
#include "PndMvaTrainer.h"
#include "PndMvaCluster.h"

// ____ Local CPP definitions _________
#define DEBUG_LVQ_TRAIN 0
// ____________________________________

//! How to initialize LVQ code books.
typedef enum ProtoInitType{
  RAND_FROM_DATA = 0,  // Select randomly from data vector.
  CCM_PR         = 1,  // Random init around Class Conditional Mean.
  KMEANS_PR      = 10, // Init using K-Means clustering.
  FILE_PR        = 20  // Read pre-init from file.
} ProtoInitType;

//! Interface definition for LVQ trainers.
class PndLVQTrain: public PndMvaTrainer
{
  //----------------------------------------
  //================== public ==============
 public:

  /**
   * Constructor:
   * @param InputEvtsParam Input events vector.
   * @param ClassNames class names.
   * @param VarNames variable names of the features.
   */
  explicit PndLVQTrain(std::vector< std::pair<std::string, std::vector<float>* > > const& InputEvtsParam,
		       std::vector<std::string> const& ClassNames, 
		       std::vector<std::string> const& VarNames,
		       bool trim = false);
  /**
   * Constructor:
   * @param InPut: Input file name.
   * @param ClassNames: class names.
   * @param VarNames: variable names of the features.
   */
  explicit PndLVQTrain(std::string const& InPut,
		       std::vector<std::string> const& ClassNames, 
		       std::vector<std::string> const& VarNames,
		       bool trim = true);
  /**
   * Destructor
   */
  virtual ~PndLVQTrain();
  
  /**
   * Store weights in the output File. If output file name is not
   * specified, then write nothing.
   */
  void storeWeights();

  /**
   * Train the classifier accourding to LVQ1 algorithm.
   */
  void Train();
  
  /**
   * Train the classifier accourding to LVQ2.1 algorithm.
   */
  void Train21();

  /**
   * Set CodeBook init type.
   *@param iniTypeVal Initialization type.
   */
  inline void setProtoInitType(ProtoInitType iniTypeVal = RAND_FROM_DATA);

  /**
   * Set the file name which holds the pre-initialized code books.
   *@param val The name of the file which containes the pre
   * initialized code books.
   */
  inline void SetInitProtoFileName(std::string const& fileName);

  /**
   * Sets the learning parameters.
   * @param initConst: Initialization constant, 
   * used to initialize LVQ prototypes.
   * @param etZ: EthaZero, start value for the learning rate.
   * @param etF: Final value for Etha (learning rate)
   * @param Nswp: Number of sweeps through the examples collection set.
   */
  inline void SetLearnPrameters(double const initConst, double const etZ,
				double const etF, unsigned int const Nswp);

  /**
   * Set the number of protoTypes to be used for training.
   * The same number of prototypes are initialized for all available
   * labels(classes).
   *@param numProto  Number of prototypes.
   */  
  void SetNumberOfProto(size_t const numProto);
  
  /**
   * Set the number of protoTypes to be used for training.
   *@param labelMap  Map containing number of prototypes 
   * for each class (label).
   */  
  void SetNumberOfProto(std::map<std::string, size_t> const& labelMap);
  
  /**
   * Set how often the classifier has to be evaluated.
   *@param val Evaluate after #val steps.
   * If (Val == 0) then the classifier is evaluated once at the end of
   * the training prodecure.
   */
  inline void SetErrorStepSize(unsigned int const val = 1000);

  /**
   * Set the window size for LVQ2.1 alg.
   * A value between 0.2 & 0.3 is recommended.
   */
  inline void SetLVQ2_1WindowSize(float const Wsize = 0.3);

  /**
   * Classifier evaluation.
   */
  void EvalClassifierError();

  /**
   * Select if we want to follow the training evaluation per epoch
   * (sweeps) or per step. The number of steps = Sweeps * (#examples).
   *
   *@param val true = evaluate per epoch, false per step. Default is
   * false.
   */
  inline void SetPerEpochEval(bool val);

  /**
   * Getter for evaluation scheme.
   *@return Per epoch or per step.
   */
  inline bool GetPerEpochEval() const;

  ////================= DEBUG Only, NOT TO BE USED
#if DEBUG_LVQ_TRAIN == 1
  inline std::vector< std::pair<std::string, std::vector<float>*> > const& train1sec()
  {
    InitProtoK_Means();
    return m_LVQProtos;
  };
  
  inline std::vector< std::pair<std::string, std::vector<float>*> > const& train2sec()
  {
    InitProtoRand();
    return m_LVQProtos;
  };
  
  /**
   * Evaluate the classifier, using the given test events.
   *@param tstEx The set which is used as test set.
   *
   * Note: tstEx is not normalized or modified. If it is needed, the
   * user has to perform modifications before invoking this method.
   */
  float EvalClassifierError(std::vector< std::pair< std::string, std::vector<float>* > > const& TestEvts) const;
#endif
  ////================= DEBUG Only, NOT TO BE USED
  
  //----------------------------------------
  // protected:
  //================== private =============
 private:
  // To avoid mistakes, :).
  PndLVQTrain(PndLVQTrain const& other);
  PndLVQTrain& operator=(PndLVQTrain const& other);
  
  //! Evaluate the classifier, train and test error.
  void EvalClassifierError(unsigned int stp);

  /**
   * Initialize LVQ prototypes (Code books) using class conditional
   * means (CCM) vectors.
   */
  void InitProtoRand();

  /**
   * Initialize LVQ prototypes (Code books) using Randomly selected
   * vectors from the original data set.
   */
  void InitRandProtoFromData();

  /**
   * Initialize LVQ prototypes (Code books) using K-Means clustering.
   */
  void InitProtoK_Means();

  /**
   * Initialize LVQ prototypes (Code books).
   */
  void InitProtoTypes();
  
  /**
   * Clean prototype container.
   */
  void cleanProtoList();

  /**
   * Updates the LVQ prototypes.
   */
  void UpdateProto(std::vector<float> const& EvtData,
		   std::vector<float>& proto,
		   int const delta, double const ethaT);

  /**
   * Check if the current update creates an invalid codebook (If the
   * vectore is placed outside the extrema). If after the update the 
   * codebook is out of boundary: reinitialize. Else: do nothing.
   *@param p The to be validated prototype.
   */
  void ValidateProtoUpdate(std::vector<float>& p);

  /**
   * Read pre-initialized code books from file and store the vectors
   * in LVQ prototype container.
   */
  void ReadProtoFromFile();
  //=====================================

  //! Holds the LVQ proto-types
  std::vector< std::pair<std::string, std::vector<float>*> > m_LVQProtos;

  //! Container to store distances.
  std::vector <PndMvaDistObj> m_distances;
  
  /**
   * Learn parameters: Init constant, start learning rate, End
   * learning rate.
   */
  double m_initConst; 
  double m_ethaZero; 
  double m_ethaFinal;
  float  m_WindowSize;

  //! Number of sweeps through example set.
  unsigned int m_NumSweep;

  //! Proto init type.
  ProtoInitType m_proto_init;

  //! initial protypes, when reading from file.
  std::string m_initProtoFile;

  //! Each #ErrorStep, steps evaluate the trained classifier.
  unsigned int m_ErrorStep;
  unsigned int m_ProgStep;
  
  //! Map labels (classes) to number of prototypes.
  std::map < std::string, size_t> m_numProtoPerClass;

  //! If evaluate per epoch
  bool m_PerEpoch;
};
//END Interface definition

//_______________________ Inline functions _________________________
inline void PndLVQTrain::setProtoInitType(ProtoInitType iniTypeVal)
{
  m_proto_init = iniTypeVal;
};

inline void PndLVQTrain::SetInitProtoFileName(std::string const& fileName)
{
  m_initProtoFile = fileName;
};

inline void PndLVQTrain::SetLearnPrameters(double const initConst, double const etZ,
					   double const etF, unsigned int const Nswp)
{
  m_initConst = initConst;
  m_ethaZero = etZ;
  m_ethaFinal = etF;
  m_NumSweep = Nswp;
};

inline void PndLVQTrain::SetErrorStepSize(unsigned int const val)
{
  m_ErrorStep = val;
};

inline void PndLVQTrain::SetLVQ2_1WindowSize(float const Wsize)
{
  m_WindowSize = Wsize;
};

inline void PndLVQTrain::SetPerEpochEval(bool val)
{
  m_PerEpoch = val;
};

inline bool PndLVQTrain::GetPerEpochEval() const
{
  return m_PerEpoch;
};
#endif// End of interface definition
