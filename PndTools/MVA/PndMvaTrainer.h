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
#include <cstdlib>

// Local includes
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"

#include "TMVA/Tools.h"
#include "TMVA/PDEFoam.h"
#include "TMVA/Event.h"

class TRandom3;

class PndMvaTrainer
{
  //==============================================
  //================ Public =======================
 public:
  /**
   * Constructor.
   *@param InPut, The input filename.
   *@param ClassNames Names of available classes.
   *@param VarNames Names of available variables.
   * Note that all of the members of these lists are selected 
   * and used during the training runs.
   */
  PndMvaTrainer(std::string const& InPut,
		std::vector<std::string> const& ClassNames, 
		std::vector<std::string> const& VarNames,
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
  inline void SetTetsSetSize(int percent = 50);
  
  //! Select input data normalization scheme.
  void NormalizeData(NormType t = NONORM);
  
  /**
   * Parameter decorrelation.
   *
   * Performs PCA (Principal component analysis) on the input dataset.
   */
  void PCATransForm();

  /**
   * Setter to set the weightfile name.
   *@param outFile Output filename.
   */
  inline void SetOutPutFile(std::string const& outFile);

  /**
   * Writes the train and test errors evaluations to a given file.
   *@param FileName Output file name.
   */
  void WriteErroVect(std::string const& FileName);

  /**
   * Initialize data structures.
   */
  virtual void Initialize();

  /**
   * Get the indices of the events selected to be used for testing.
   */
  inline std::set <int> const& GetTestEvetIdx() const;
  
  //==============================================
  //================ Protected ===================
 protected:

  inline void SetAppType(AppType t);

  /**
   * Write the training and normalization data to outFile.
   */
  void WriteToWeightFile(std::vector< std::pair<std::string, 
			 std::vector<float>*> > const& weights);
  /**
   * Writes generated TMVA Foams to file.
   *@param foams The list of the foams to be stored.
   * Note that the implementation of TMVA foams here is in early
   * experimental stage.
   */
  void WriteToWeightFile(std::vector<TMVA::PDEFoam*> const& foams);
  
  //void WriteDataSetToOutFile();
  
  //! Classifier error evaluation
  virtual void EvalClassifierError()
  {};
  
  //! Indices of the test set.
  std::set <int> m_testSet_indices;
  
  //! Data set. Holds event values
  PndMvaDataSet m_dataSets;
  
  //! Container to keep per step error values.
  std::vector <StepError> m_StepErro;
  
  //! Output filename.
  std::string m_outFile;
  
  //! Random seed
  unsigned int  m_RND_seed;

  void splitTetsSet();
  //==============================================
  //================ Private =====================
 private:
 //! To avoid mistakes.
  PndMvaTrainer(PndMvaTrainer const& other);
  PndMvaTrainer& operator=(PndMvaTrainer const& other);
  // Either trim or not
  bool m_trim;
  int  m_testSetSize;
};// End of class definition.

//========================= Inline implementations =================
inline void PndMvaTrainer::SetOutPutFile(std::string const& outFile)
{
  m_outFile = outFile;
};

inline void PndMvaTrainer::SetAppType(AppType t)
{
  m_dataSets.SetAppType(t);
};

inline std::set <int> const& PndMvaTrainer::GetTestEvetIdx() const
{
  return m_testSet_indices;
};

inline void PndMvaTrainer::SetTetsSetSize(int percent)
{
  m_testSetSize = percent;
};
#endif
