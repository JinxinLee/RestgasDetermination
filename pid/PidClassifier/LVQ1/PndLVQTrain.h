/* **************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * Version: 0.1 beta1.                   *
 * LICENSE:                              *
 * ***************************************
 */
#ifndef Pnd_LVQ_Train_H
#define Pnd_LVQ_Train_H

//Local includes
#include "PndMvaTrainer.h"

//! Interface definition for LVQ trainers.
class PndLVQTrain: public PndMvaTrainer
{
 public:
  /**
   * Constructor:
   * @param InPut: Input file name.
   * @param ClassNames: class names.
   * @param VarNames: variable names of the features.
   */
  PndLVQTrain(const std::string& InPut,
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames,
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
  
  // Test functions Modified training schemes, May BE deleted after
  // testing. At each step: select number of classes random examples.
  void TrainSec  ();
  void Train21Sec();

  /**
   * Sets the learning parameters.
   * @param initConst: Initialization constant, 
   * used to initialize LVQ prototypes.
   * @param etZ: EthaZero, start value for the learning rate.
   * @param etF: Final value for Etha (learning rate)
   * @param Nswp: Number of sweeps through the examples collection set.
   */
  inline void SetLearnPrameters(const double initConst, const double etZ,
                         const double etF, const int Nswp)
  { m_initConst = initConst; m_ethaZero = etZ; 
    m_ethaFinal = etF; m_NumSweep = Nswp;};
  /**
   * Set the number of protoTypes to be used for training.
   *@param numProto  Number of prototypes.
   */  
  inline void SetNumberOfProto(const unsigned int numProto)
  {m_numProto = numProto; };
  
 private:
  // To avoid mistakes, :).
  PndLVQTrain(const PndLVQTrain& other);
  PndLVQTrain& operator=(const PndLVQTrain& other);

  //! Evaluate the classifier, train and test error.
  void EvalClassifierError(unsigned int stp);

  /**
   *Initialize LVQ prototypes (Code books) using class conditional
   *means vectors.
   *@param numProto number of code books to use.
   */
  void InitProtoTypes();
  
  /**
   * Clean prototype container.
   */
  void cleanProtoList();

  /**
   * Updates the LVQ prototypes.
   */
  void UpdateProto(const std::vector<float>& EvtData, 
		   std::vector<float>& proto, 
		   const int delta, const double ethaT);
  //=====================================

  //! Holds the LVQ proto-types
  std::vector< std::pair<std::string, std::vector<float>*> > m_LVQProtos;

  //! Container to store distances.
  std::vector <PndMvaDistObj> m_distances;
  
  /**
   * Learn parameters: Init constant, start learning rate, End
   * learning rate.
   */
  double m_initConst, m_ethaZero, m_ethaFinal;
  
  //! Number of sweeps through example set.
  unsigned int m_NumSweep;

  //! number of proto-types to train
  unsigned int m_numProto;
};
#endif //END Interface
