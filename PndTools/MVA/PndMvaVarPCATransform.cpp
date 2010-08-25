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

#include "PndMvaVarPCATransform.h"

PndMvaVarPCATransform::PndMvaVarPCATransform()
{}

PndMvaVarPCATransform::~PndMvaVarPCATransform()
{
  // Delete Mean value vector
  if(m_MeanValues){
    delete m_MeanValues;
  }
  // Delete Eigenvectors matrix.
  if(m_EigenVectors){
    delete m_EigenVectors;
  }
}

/**
 * Transforms the current event variables
 @@param evd Vector containing the event to transform.
 *@return Transformed event.
 */
const std::vector<float>& PndMvaVarPCATransform::Transform(const std::vector<float>& evd) const
{
  evd.size();
  return *(new std::vector<float>());
}

// Given a list of n-dimensional data points, Computes PCA for the current dataset.
void PndMvaVarPCATransform::ComputePrincipalComponents(const std::vector< std::pair<std::string, std::vector<float>*> >& dat)
{
  if(dat.size() <= 0){
    std::cerr << "<ERROR> No data available in the given data container.\n"
	      << "Could not perform PCA." << std::endl;
    assert(dat.size() != 0);
  }
  
  unsigned int nvar = (dat[0].second)->size();
  double *dvec = new double[nvar];

  /*
   * Options are:
   * N       Normalize the covariance matrix (default)
   * D       Store input data (default)
  */
  //Not normalizing and not storing input data, for performance
  //reasons. Should perhaps restore normalization.
  TPrincipal pca(nvar, "");

  // Loop through the dataset members.
  for(size_t ev = 0; ev < dat.size(); ev++){
    // Fetch features of the current event.
    std::vector<float>* curEv = dat[ev].second;
    // Copy values.
    for(size_t i = 0; i < curEv->size(); i++){
      dvec[i] = curEv->at(i);
    }
    // Add array to the Tprincipal object.
    pca.AddRow( dvec );
  }
  
  // Perform actual principal component analysis.
  pca.MakePrincipals();

  // retrieve mean values, eigenvectors and sigmas. Need to copy
  // ownership.
  m_MeanValues   = new TVectorD( *(pca.GetMeanValues()) );
  m_EigenVectors = new TMatrixD( *(pca.GetEigenVectors()) );
  
  // Free unused and claimed memory.
  delete [] dvec;
}
