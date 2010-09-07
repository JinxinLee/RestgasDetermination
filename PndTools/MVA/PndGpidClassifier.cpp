/* **********************************************
 * MVA classifiers interface.                   *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version:                                     *
 * License:                                     *
 * **********************************************
 */
#include "PndGpidClassifier.h"

// ===========================  public:
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
