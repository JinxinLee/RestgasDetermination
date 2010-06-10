/* **********************************************
 * MVA classifiers interface.                   *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version: 0.1 beta1.                          *
 * License:                                     *
 * **********************************************
 */
#ifndef PND_GPID_CLASSIFIER_H
#define PND_GPID_CLASSIFIER_H

// C++ includes
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cmath>
#include <cassert>

// PND PID includes.
#include "PndMvaDataSet.h"
#include "PndMvaUtil.h"

// PANDA And ROOT includes

/**
 * Main interface definition.
*/
class PndGpidClassifier
{
 public:
  //! Constructor
  /**
   *@param InPut Input file name containing weights.
   *@ClassNames Names of available (signal) classes.
   *@VarNames Names of the used variables(features).
   */
  PndGpidClassifier(const std::string& InPut,
		    const std::vector<std::string>& ClassNames,
		    const std::vector<std::string>& VarNames);
  //! Destructor
  virtual ~PndGpidClassifier();
  
  /**
   * @param EvtData: Event data to be classified.
   * @param result:  Classification results. Currently the shortest
   *  distance for each class is stored in result.
   */
  virtual void GetMvaValues( std::vector<float> EvtData,
			     std::map<std::string,float>& result ) = 0;
  /**
   *@param EvtData: Event, to be classified.
   *@return Name of the class with the best MVA value.
   */
  virtual const std::string& Classify( std::vector<float> EvtData )const = 0;

 protected:
  //! Data set. Holds event Weights
  PndMvaDataSet m_dataSets;
 
 private:
  // To avoid mistakes.
  PndGpidClassifier(const PndGpidClassifier& other);
  PndGpidClassifier& operator=(const PndGpidClassifier& other);
};
#endif //End of class interface
