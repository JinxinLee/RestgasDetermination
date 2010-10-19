/***************************************
 * Class interface of DataSet class.   *
 * Author: M.Babai (M.Babai@rug.nl)    *
 * License:                            *
 * Version:                            *
 ***************************************/
#ifndef PND_MVA_DATASET_H
#define PND_MVA_DATASET_H

// C++ includes
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <limits>
#include <typeinfo>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

#include "TList.h"

// TMVA
//#include "TMVA/PDEFoam.h"

// Local includes
#include "PndMvaClass.h"
#include "PndMvaVariable.h"
#include "PndMvaVarPCATransform.h"

// ========================================================================
// Normalization schemes
typedef enum {NONE = 0, VARX = 1, MINMAX = 2, MEDIAN = 3} NormType;

// ========================================================================
class PndMvaDataSet
{
 public:
  /**
   * Constructor.
   *@param inputFilename  Input File name.
   *@param classNames    Names of available classes.
   *@param varNames     Available variabl names.
   */
  PndMvaDataSet(std::string const &inputFilename,
		std::vector<std::string> const &classNames,
		std::vector<std::string> const &varNames);

  //! Destructor
  virtual ~PndMvaDataSet();
  
  /**
   * Normalize event dataset using one of available methods.
   * @param t Normalization type (VARX, MINMAX, MEDIAN).
   */
  void NormalizeDataSet(NormType const type = NONE);

  /**
   * Write the normalized DataSet to the out-put file.
   * @param  outFile  File name to write to
   */
  void WriteDataSet(std::string const &outFile);  

  /**
   * Initialize the class conditional means vectors.
   */
  void InitClsCondMeans();

  /**
   * Creates a data set with equal number of events for each class.
   */
  void Trim();
  
  //! Get available data.
  inline std::vector< std::pair<std::string, std::vector<float>*> > const &GetData() const;

  //! Get the list of available classes (labels).
  inline std::vector<PndMvaClass> const &GetClasses() const;
  
  //! Get the list of available variables.
  inline std::vector<PndMvaVariable> const &GetVars() const;
  
  //! Get classconditional means for all classes (labels).
  inline std::map< std::string, std::vector<float>* > const &GetClassCondMeans() const;
  
  //! Get name of input file name (weight/event file).
  inline std::string const &GetInFileName() const;

  //========================= PCA =====================//
  /**
   * Parameter decorrelation.
   *
   * Performs PCA (Principal component analysis) on the input dataset.
   */
  void PCATransForm();

  //! If PCA was applied.
  inline bool Used_PCA() const;
  
  //! Get PCA object
  inline PndMvaVarPCATransform const &Get_PCA() const;

  //_________________________ PCA _____________________//

 protected:
  /**
   * Read input event data.
   */
  void ReadInput();
  
 private:
  // Private to avoid mistakes.
  // Copy constructor.
  PndMvaDataSet(PndMvaDataSet const &other);
  PndMvaDataSet& operator=(PndMvaDataSet const &other);
  
  /**
   * Class conditional mean for a given class. Stored in class
   * conditional means container.
   */
  void CompClsCondMean(const std::string& clsName);
  
  /**
   * Computes Variance (unbiased estimator) for each parameter in the
   * feature list.
   * @param clsName The name of the class of events for with we want
   * to compute Var(X).
   */
  void ComputeVariance();
  
  /**
   * Determines the median for parameters of the loaded DataSet.
   */
  void DetermineMedian();
  
  /**
   * Determine Min Max difference.
   */
  void MinMaxDiff();
  
  //! Input File name
  std::string m_input;
  
  //! Classes.
  std::vector<PndMvaClass> m_classes;
  
  //! Variables.
  std::vector<PndMvaVariable> m_vars;
  
  //! Container to keep  the Event data feature vectors
  std::vector< std::pair<std::string, std::vector<float>*> > m_events;
  
  //! Container to keep  the Class Conditional means
  std::map< std::string, std::vector<float>* > m_ClassCondMeans;
  
  // PCA transformation.
  PndMvaVarPCATransform m_PCA;
  
  // If PCA was applied.
  bool m_UsePCA;
};

// ============= Inline implementation ==================
inline const std::vector< std::pair<std::string, std::vector<float>*> >& PndMvaDataSet::GetData() const
{
  return m_events;
};

inline const std::vector<PndMvaClass>& PndMvaDataSet::GetClasses() const
{
  return m_classes;
};

inline const std::vector<PndMvaVariable>& PndMvaDataSet::GetVars() const
{
  return m_vars;
};

inline const std::map< std::string, std::vector<float>* >& PndMvaDataSet::GetClassCondMeans() const
{
  return m_ClassCondMeans;
};

inline const std::string& PndMvaDataSet::GetInFileName() const
{
  return m_input;
};

inline bool PndMvaDataSet::Used_PCA() const
{
  return m_UsePCA;
};

inline const PndMvaVarPCATransform& PndMvaDataSet::Get_PCA() const
{
  return m_PCA;
};
#endif
