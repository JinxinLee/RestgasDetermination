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
 * This directly based on the Cern Root implementation of PCA.
*/

#ifndef PND_MVA_VAR_PCA_TRANSFORM_h
#define PND_MVA_VAR_PCA_TRANSFORM_h

// C & C++ includes

// ROOT includes.
#include "TPrincipal.h"

class PndMvaVarPCATransform
{
 public:
  //! Constructor
  PndMvaVarPCATransform();

  //! Destructor.
  virtual ~PndMvaVarPCATransform();
  
 protected:
 private:
  // To avoid mistakes.
  PndMvaVarPCATransform(const PndMvaVarPCATransform& o);
  PndMvaVarPCATransform& operator=(const PndMvaVarPCATransform& o);

  void ComputePrincipalComponents(const std::vector< std::pair<std::string, std::vector<float>*> >& dat);
};
#endif// ENd of interface definition
