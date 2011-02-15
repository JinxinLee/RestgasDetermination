/* ***************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
#ifndef PND_LVQ_TRAIN_H
#define PND_LVQ_TRAIN_H

#ifdef _OPENMP
#include <omp.h>
#endif

//Local includes
#include "PndMvaTrainer.h"
#include "PndMvaCluster.h"

//! How to initialize LVQ code books.
typedef enum
{
  RANDOM_PR = 0,// Random initialization.
  KMEANS_PR = 1,// Init using K-Means clustering.
  FILE_PR = 2   // Read pre-init from file.
} ProtoInitType;

//! Interface definition for LVQ trainers.
class PndLVQTrain: public PndMvaTrainer
{
  //----------------------------------------
  //================== public ==============
 public:
  /**
   * Constructor:
   * @param InPut: Input file name.
   * @param ClassNames: class names.
   * @param VarNames: variable names of the features.
   */
  PndLVQTrain(std::string const& InPut,
	      std::vector<std::string> const& ClassNames, 
	      std::vector<std::string> const& VarNames,
	      bool trim = true);
  /**
   * Destructor
   */
  virtual ~PndLVQTrain();
  
  /**
   * Train the classifier accourding to LVQ1 algorithm.
   */
  void Train();
  
  /**
   * Train the classifier accourding to LVQ2.1 algorithm.
   */
  void Train21();

  ////================= DEBUG Only
  inline std::vector< std::pair<std::string, std::vector<float>*> >& train1sec()
  {
    InitProtoK_Means();
    return m_LVQProtos;
  };
  ////================= DEBUG Only

  /**
   * Set CodeBook init type.
   *@param val Initialization type.
   */
  inline void setProtoInitType(ProtoInitType iniTypeVal = RANDOM_PR);

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
				double const etF, int const Nswp);

  /**
   * Set the number of protoTypes to be used for training.
   * The same number of prototypes are initialized for all available
   * labels(classes).
   *@param numProto  Number of prototypes.
   */  
  void SetNumberOfProto(unsigned int const numProto);
  
  /**
   * Set the number of protoTypes to be used for training.
   *@param labelMap  Map containing number of prototypes 
   * for each class (label).
   */  
  void SetNumberOfProto(std::map<std::string, unsigned int> const& labelMap);
  
  /**
   * Set how often the classifier has to be evaluated.
   *@param val Evaluate after #val steps.
   */
  inline void SetErrorStepSize(unsigned int const val);

  //----------------------------------------
  //================== private =============
 private:
  // To avoid mistakes, :).
  PndLVQTrain(PndLVQTrain const& other);
  PndLVQTrain& operator=(PndLVQTrain const& other);

  //! Evaluate the classifier, train and test error.
  void EvalClassifierError(unsigned int stp);

  /**
   * Initialize LVQ prototypes (Code books) using class conditional
   * means vectors.
   */
  void InitProtoRand();
    
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
  std::map < std::string, unsigned int> m_numProtoPerClass;
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
					   double const etF, int const Nswp)
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
#endif
