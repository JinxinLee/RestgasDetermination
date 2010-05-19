/* **********************************************
 * MVA classifiers trainers interface.          *
 * Author: M. Babai                             *
 * M.Babai@rug.nl                               *
 * Version: 0.1 beta1.                          *
 * LICENSE:                                     *
 * **********************************************
 */
#ifndef PND_MVA_TRAINER_H
#define PND_MVA_TRAINER_H

// C++ includes
#include <cassert>
#include <limits>
#include <ctime>
#include <iomanip>

// Local includes
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"

#include "TMVA/Tools.h"
#include "TMVA/PDEFoam.h"
#include "TMVA/Event.h"

class TRandom3;

class PndMvaTrainer
{
 public:
  /**
   * Constructor.
   *@param InPut, The input filename.
   *@param ClassNames Names of available classes.
   *@param VarNames Names of available variables.
   * Note that all of the members of these lists are selected 
   * and used during the training runs.
   */
  PndMvaTrainer(const std::string& InPut,
		const std::vector<std::string>& ClassNames, 
		const std::vector<std::string>& VarNames,
		bool trim = true);

  //! Destructor
  virtual ~PndMvaTrainer();

  //! Derived classes need to implement this methode.
  //! Minimum requirment.  
  virtual void Train() = 0;

  /**
   * Creates test and train data sets.
   * @param percent Percent of the data set to be used for testing and
   * cross-validation
   */
  void splitTetsSet(int percent = 10);
  
  //! Select input data normalization scheme.
  void NormalizeData(NormType t = NONE){ 
    m_normType = t;
    m_dataSets.NormalizeDataSet(t);

    /// DEBUG
    // m_dataSets.WriteDataSet("InputVarNormalized.root");
    ///DEBUG
  }
  
  /**
   * Setter to set the weightfile name.
   *@param outFile Output filename.
   */
  void SetOutPutFile(const std::string& outFile)
  {m_outFile = outFile;}

  /**
   * Writes the train and test errors evaluations to a given file.
   *@param FileName Output file name.
   */
  void WriteErroVect(const std::string FileName);

 protected:
  /**
   * Write the training and normalization data to outFile.
   */
  void WriteToWeightFile(const std::vector< std::pair<std::string, 
			 std::vector<float>*> >& weights);
  /**
   * Writes generated TMVA Foams to file.
   *@param foams The list of the foams to be stored.
   * Note that the implementation of TMVA foams here is in early
   * experimental stage.
   */
  void WriteToWeightFile(const std::vector<TMVA::PDEFoam*>& foams);
  
  void WriteDataSetToOutFile();
  
  //! Classifier error evaluation
  virtual void EvalClassifierError(){};
  
  //! Indices of the test set.
  std::set <int> m_testSet_indices;
  
  //! Data set. Holds event values
  PndMvaDataSet m_dataSets;
  
  //! Container to keep per step error values.
  std::vector <StepError> m_StepErro;
  //! Output filename.
  std::string m_outFile;
  //! Selected normalization scheme.
  NormType m_normType;
  
  //! Random seed
  unsigned int  m_RND_seed;
  
 private:
  PndMvaTrainer(const PndMvaTrainer& other);
  PndMvaTrainer& operator=(const PndMvaTrainer& other);
};
#endif
