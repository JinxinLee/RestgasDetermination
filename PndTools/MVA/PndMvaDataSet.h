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

// TMVA
//#include "TMVA/PDEFoam.h"

// Local includes
#include "PndMvaClass.h"
#include "PndMvaVariable.h"

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
  PndMvaDataSet(const std::string& inputFilename,
		const std::vector<std::string>& classNames,
		const std::vector<std::string>& varNames);
  //! Destructor
  virtual ~PndMvaDataSet();
  
  /**
   * Normalize event dataset using one of available methods.
   * @param t Normalization type (VARX, MINMAX, MEDIAN).
   */
  void NormalizeDataSet(const NormType type = NONE);

  /**
   * Write the normalized DataSet to the out-put file.
   * @param  outFile  File name to write to
   */
  void WriteDataSet(const std::string& outFile);  
  
  /**
   * Initialize the class conditional means vectors.
   */
  void InitClsCondMeans();

  /**
   * Creates a data set with equal number of events for each class.
   */
  void Trim();

  
  //! Get available data.
  inline const std::vector< std::pair<std::string, std::vector<float>*> >& GetData() const;

  //! Get the list of available classes.
  inline const std::vector<PndMvaClass>& GetClasses() const;
  
  //! Get the list of available variables.
  inline const std::vector<PndMvaVariable>& GetVars() const;
  
  //! Get classconditional means for all classes.
  inline const std::map< std::string, std::vector<float>* >& GetClassCondMeans() const;
  
  //! Get name of input file name (weight/event file).
  inline const std::string& GetInFileName() const;
 
 protected:
  /**
   * Read input event data.
   */
  void ReadInput();
  
 private:
  // Private to avoid mistakes.
  // Copy constructor.
  PndMvaDataSet(const PndMvaDataSet& other);
  PndMvaDataSet& operator=(const PndMvaDataSet& other);

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
};

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
#endif
