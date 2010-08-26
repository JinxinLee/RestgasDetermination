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

using namespace std;

PndMvaVarPCATransform::PndMvaVarPCATransform()
{}

PndMvaVarPCATransform::~PndMvaVarPCATransform()
{
  cout << "<INFO> Cleaning claimed memory and removing objects."
       << endl; 
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
 * Prepare Transformation for the given dataset events.
 *@param dat Collection of the event feature vectors.
 */
bool PndMvaVarPCATransform::InitPCATranformation(const vector<pair<std::string, vector<float>*> >& dat)
{
  if( dat.size() <= 0 ){
    std::cerr << "<ERROR> No data available in the given data container.\n"
	      << "Could not perform PCA." << std::endl;
    assert(dat.size() != 0);
  }
  cout << "<INFO> Initializing PCA object and computing PCA transformation parameters."
       << endl;
  ComputePrincipalComponents(dat);

  if(m_MeanValues && m_EigenVectors){
    return true;
  }
  return false;
}

/**
 * Transforms the current event variables
 @@param evd Vector containing the event to transform.
 *@return Transformed event.
 */
const std::vector<float>& PndMvaVarPCATransform::Transform(const std::vector<float>& evt) const
{
  const size_t nvar = evt.size();
  std::vector<float>* p = new std::vector<float>(nvar, 0.0);
  
  for (size_t i = 0; i < nvar; i++) {
    double pv = 0;
    for (size_t j = 0; j < nvar; j++)
      pv += (static_cast<double>(evt.at(j)) - (*m_MeanValues)(j)) * (*m_EigenVectors)(j,i);
    (*p)[i] = pv;
  }
  return *p;
}

// Given a list of n-dimensional data points, Computes PCA for the current dataset.
void PndMvaVarPCATransform::ComputePrincipalComponents(const vector<pair<string, vector<float>*> >& dat)
{
  cout << "<INFO> Computing PCA for the current dataset."
       << endl;
  size_t nvar = (dat[0].second)->size();
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
