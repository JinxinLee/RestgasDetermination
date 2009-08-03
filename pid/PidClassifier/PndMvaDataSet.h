#pragma once
#ifndef PNDMVADATASET_H
#define PNDMVADATASET_H

// C++ includes
#include <vector>
#include <map>
#include <set>
#include <iostream>
#include <string>
#include <algorithm>
#include <cmath>
#include <cassert>
#include <limits>

// ROOT
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

// Local includes
#include "PndMvaClass.h"
#include "PndMvaVariable.h"

// ========================================================================
// Normalization schemes
typedef enum {NONE = 1, VARX = 2, MINMAX = 3, MEDIAN = 4} NormType;

// ========================================================================
// ========================================================================
/**
 * Class to hold the computed Euclidean distances between the current
 * example and the available LVQ protoTypes (codeBook) in LVQ2.1
 * implementation.
 */
struct PndMvaDistObj
{
  //! Constructor
 PndMvaDistObj():m_idx(-1), m_dist(0.0), m_cls("UNKNOWN"){};
  
 PndMvaDistObj(const int id, const float dist, const std::string& cls)
   : m_idx(id), m_dist(dist), m_cls(cls){};
  
  // Destructor
  virtual ~PndMvaDistObj(){};
  
  //! Operator < 
  inline bool operator< (const PndMvaDistObj& other)const{
    return (m_dist < other.m_dist);
  };
  
  //! Operator > 
  inline bool operator> (const PndMvaDistObj& other)const{
    return (m_dist > other.m_dist);
  };
  
  int m_idx;/**< Index of the prototype. */
  float m_dist;/**< Distance to the current example. */
  std::string m_cls;/**< Class name of the prototype. */
};

// ========================================================================
// ========================================================================
class PndMvaDataSet
{
 public:

  PndMvaDataSet(const std::string& inputFilename,
		const std::vector<std::string>& classNames,
		const std::vector<std::string>& varNames);

  virtual ~PndMvaDataSet();
  
  /**
   * Normalize event dataset using one of available methods.
   * @param t Normalization type (VARX, MINMAX, MEDIAN).
   */
  void NormalizeDataSet(const NormType type);

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

  const std::vector< std::pair<std::string, std::vector<float>*> >& GetData() const;
  const std::vector<PndMvaClass>& GetClasses() const;
  const std::vector<PndMvaVariable>& GetVars() const;
  const std::map< std::string, std::vector<float>* >& GetClassCondMeans() const;
  const std::string& GetInFileName() const;
 
 protected:
  /**
   * Read input event data.
   *@param InPut Input file name.
   */
  void ReadInput();
  
 private:
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
}

inline const std::vector<PndMvaClass>& PndMvaDataSet::GetClasses() const
{
  return m_classes;
}

inline const std::vector<PndMvaVariable>& PndMvaDataSet::GetVars() const
{
  return m_vars;
}

inline const std::map< std::string, std::vector<float>* >& PndMvaDataSet::GetClassCondMeans() const
{
  return m_ClassCondMeans;
}

inline const std::string& PndMvaDataSet::GetInFileName() const
{
  return m_input;
}
#endif
