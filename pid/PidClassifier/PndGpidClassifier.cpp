/* **********************************************
 * MVA classifiers interface.                   *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version: 0.1 beta1.                          *
 * License:                                     *
 * **********************************************
 */
#include "PndGpidClassifier.h"

// ===========================  public:
PndGpidClassifier::PndGpidClassifier(const std::string& InPut,
				     const std::vector<std::string>& ClassNames, 
				     const std::vector<std::string>& VarNames)
  : m_dataSets(InPut, ClassNames, VarNames)
{}

PndGpidClassifier::~PndGpidClassifier()
{}
