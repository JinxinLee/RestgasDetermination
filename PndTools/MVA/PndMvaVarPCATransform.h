/* **********************************************
 * MVA variable transformation interface.       *
 * Principal Components Analysis (PCA).         *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version:                                     *
 * License:                                     *
 * **********************************************
 */
/*
 * This code is directly based on the Cern Root implementation of PCA.
*/

#ifndef PND_MVA_VAR_PCA_TRANSFORM_H
#define PND_MVA_VAR_PCA_TRANSFORM_H

// C & C++ includes
#include <iostream>
#include <vector>
#include <utility>
#include <cassert>

// ROOT includes.
#include "TPrincipal.h"

class PndMvaVarPCATransform
{
 public:
  //! Constructor
  PndMvaVarPCATransform();

  //! Destructor.
  virtual ~PndMvaVarPCATransform();

  /**
   * Prepare Transformation for the given dataset events.
   *@param dat Collection of the event feature vectors.
   */
  bool InitPCATranformation(const std::vector<std::pair<std::string, std::vector<float>*> >& dat);

  /**
   * Transforms the current event variables
   @@param evd Vector containing the event to transform.
   *@return Transformed event.
   */
  const std::vector<float>& Transform(const std::vector<float>& evd) const;

  //! Get mean values vector
  inline const TVectorD& GetMeanValues() const;

  //! Get Eigenvectors matrix.
  inline const TMatrixD& GetEigenVectors() const;

 private:
  // To avoid mistakes.
  PndMvaVarPCATransform(const PndMvaVarPCATransform& ot);
  PndMvaVarPCATransform& operator=(const PndMvaVarPCATransform& ot);
  
  // Given a list of n-dimensional data points, Computes PCA for the current dataset.
  void ComputePrincipalComponents(const std::vector< std::pair<std::string, std::vector<float>*> >& dat);
  
  // Mean values
  TVectorD* m_MeanValues;

  // Eigenvectors
  TMatrixD* m_EigenVectors;
};

/**
 * Get mean values vector
 */
inline const TVectorD& PndMvaVarPCATransform::GetMeanValues() const
{
  return *m_MeanValues;
}

/**
 * Get Eigenvectors matrix.
 */
inline const TMatrixD& PndMvaVarPCATransform::GetEigenVectors() const
{
  return *m_EigenVectors;
}
#endif// ENd of interface definition
