/* **********************************************
 * MVA classifiers interface.                   *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version:                                     *
 * License:                                     *
 * **********************************************
 */
#include "PndGpidClassifier.h"

/**
 *@param InPut Input file name containing weights.
 *@ClassNames Names of available (signal) classes.
 *@VarNames Names of the used variables(features).
 */
PndGpidClassifier::PndGpidClassifier(const std::string& InPut,
				     const std::vector<std::string>& ClassNames, 
				     const std::vector<std::string>& VarNames)
  : m_dataSets(InPut, ClassNames, VarNames)
{}

//! Destructor.
PndGpidClassifier::~PndGpidClassifier()
{}

// Normalize the given event vector.
void PndGpidClassifier::NormalizeEvent(std::vector<float>& event) const
{
  // Fetch variables
  const std::vector <PndMvaVariable>& vars = m_dataSets.GetVars();

  // Normalize current Event
  for(size_t k = 0; k < vars.size(); ++k)
  {
    assert(vars[k].NormFactor != 0);
    event[k] -= vars[k].Mean;
    event[k] /= vars[k].NormFactor;
  }

  // If the input was PCA_transformed.
  if( m_dataSets.Used_PCA() )
  {
    PndMvaVarPCATransform const &pca = m_dataSets.Get_PCA();
    std::vector<float> *trsEvt = pca.Transform(event);
    
    // Copy back to input event.
    event = std::vector<float>(*trsEvt);

    // Free allocated memory.
    delete trsEvt;
  }
}
