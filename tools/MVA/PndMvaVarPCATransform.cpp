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

/**
 * Constructor.
 */
PndMvaVarPCATransform::PndMvaVarPCATransform()
  : m_MeanValues(0),
    m_EigenVectors(0)
{}

/**
 * Destructor.
 */
PndMvaVarPCATransform::~PndMvaVarPCATransform()
{
  // Delete Mean value vector.
  if(m_MeanValues)
  {
    delete m_MeanValues;
  }

  // Delete Eigenvectors matrix.
  if(m_EigenVectors)
  {
    delete m_EigenVectors;
  }
}

/**
 * Prepare Transformation for the given dataset events.
 *@param dat Collection of the event feature vectors.
 */
bool PndMvaVarPCATransform::InitPCATranformation(std::vector<std::pair<std::string, std::vector<float>*> > const &dat)
{
  if( dat.size() <= 0 )
  {
    std::cerr << "<ERROR> No data available in the given data container.\n"
	      << "Could not perform PCA." << std::endl;
    exit(EXIT_FAILURE);
  }
  std::cout << "<INFO> Initializing PCA object and"
	    <<" computing PCA transformation parameters.\n";
  
  ComputePrincipalComponents(dat);

  if( m_MeanValues && m_EigenVectors )
  {
    return true;
  }
  return false;
}

/**
 * Transforms the current event variables
 *@param evd Vector containing the event to transform.
 *@return Transformed event.
 */
std::vector<float>* PndMvaVarPCATransform::Transform(std::vector<float> const &evt) const
{
  if( !(m_MeanValues && m_EigenVectors) )
  {
    std::cerr << "<ERROR> Eigen vectors and mean values are not initialized. "
	      << "PCA transformation can not be done."
	      << std::endl;
    abort();
  }
  size_t const nvar = evt.size();

  // Allocate memory and initialize.
  std::vector<float>* p = new std::vector<float>(nvar, 0.0);
  
  for (size_t i = 0; i < nvar; i++)
  {
    double pv = 0.00;
    for (size_t j = 0; j < nvar; j++)
    {
      pv += (static_cast<double>(evt.at(j)) - (*m_MeanValues)(j)) * (*m_EigenVectors)(j,i);
    }
    (*p)[i] = pv;
  }
  return p;
}

/**
 * Given a list of n-dimensional data points, Computes PCA for the
 * current dataset.
 */
void PndMvaVarPCATransform::ComputePrincipalComponents(std::vector< std::pair< std::string, std::vector<float>* > > const &dat)
{
  std::cout << "<INFO> Computing PCA for the current dataset.\n";

  size_t nvar = (dat[0].second)->size();

  if(nvar <= 1)
  {
    std::cerr << "<ERROR> Cannot perform PCA transformation for "
	      << nvar << std::endl;
    abort();
  }
  // Temporary to store event parameters.
  double *dvec = new double[nvar];
  
  /*
   * Options are:
   * N       Normalize the covariance matrix (default)
   * D       Store input data (default)
   */
  TPrincipal pca(nvar, "N");

  // Loop through the dataset members.
  for(size_t ev = 0; ev < dat.size(); ev++)
  {
    // Fetch parameters of the current event.
    std::vector<float>* curEv = dat[ev].second;

    // Copy values.
    for(size_t i = 0; i < curEv->size(); i++)
    {
      dvec[i] = curEv->at(i);
    }

    // Add array to the Tprincipal object.
    pca.AddRow( dvec );
  }
  
  // Perform actual principal component analysis.
  pca.MakePrincipals();

  /*
   * Retrieve mean values, eigenvectors. Need to copy ownership.
   */
  m_MeanValues   = new TVectorD( *(pca.GetMeanValues()) );
  
  m_EigenVectors = new TMatrixD( *(pca.GetEigenVectors()) );
  
  const TVectorD* eigenVals = pca.GetEigenValues();
  
  // Print Some info to screen
  std::cout << "<INFO> Eigen values:\n";
  eigenVals->Print();

  std::cout << "<INFO> Mean values:\n";
  m_MeanValues->Print();

  std::cout << "<INFO> Eigen vectors:\n";
  m_EigenVectors->Print();
  
  // Free memory.
  delete [] dvec;
}

// Set mean values vector.
void PndMvaVarPCATransform::SetMeanVector(const TVectorD& vect)
{
  m_MeanValues = new TVectorD(vect);
}

// Set Eigenvectors matrix.
void PndMvaVarPCATransform::SetEigenVectors(const TMatrixD& mat)
{
 m_EigenVectors = new TMatrixD(mat);
}
