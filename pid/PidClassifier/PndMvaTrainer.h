/* **********************************************
 * MVA classifiers trainers interface.          *
 * Author: M. Babai                             *
 * M.Babai@rug.nl                               *
 * Version: 0.1 beta1.                          *
 * LICENSE:                                     *
 * **********************************************
 */

//#pragma once
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
   * FIXME DOC
   */
  PndMvaTrainer(const std::string& InPut,
		const std::vector<std::string>& ClassNames, 
		const std::vector<std::string>& VarNames,
		bool trim = true);
  /**
   * FIXME DOC
   */
  virtual ~PndMvaTrainer();
  
  virtual void Train() = 0;

  /**
   * Creates test and train data sets.
   * @param percent Percent of the data set to be used for testing and
   * cross-validation
   */
  void splitTetsSet(int percent = 10);
  

  void NormalizeData(NormType t = NONE){ 
    m_normType = t;
    m_dataSets.NormalizeDataSet(t);
  }

  void SetOutPutFile(const std::string& outFile)
  {m_outFile = outFile;}

  void WriteErroVect(const std::string FileName);

 protected:
  /**
   * Write the training and normalization data to outFile.
   */
  void WriteToWeightFile(const std::vector< std::pair<std::string, 
			 std::vector<float>*> >& weights);

  void WriteToWeightFile(const std::vector<TMVA::PDEFoam*>& foams);
  
  virtual void EvalClassifierError(){};
  
  //! Indices of the test set.
  std::set <int> m_testSet_indices;
  
  //! Data set. Holds event values
  PndMvaDataSet m_dataSets;
  
  std::vector <StepError> m_StepErro;
  
  std::string m_outFile;
  
  NormType m_normType;
  
  //! Random seed
  unsigned int  m_RND_seed;
  
 private:
  PndMvaTrainer(const PndMvaTrainer& other);
  PndMvaTrainer& operator=(const PndMvaTrainer& other);
};
#endif
