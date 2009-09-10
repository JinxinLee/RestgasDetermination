/* **********************************************
 * MVA classifiers interface.                   *
 * Author: M.Babai@rug.nl                       *
 * LICENSE:                                     *
 * Version: 0.1 beta1.                          *
 * License:                                     *
 * **********************************************
 */

//#pragma once
#ifndef PndGPIDCLASSIFIER_H
#define PndGPIDCLASSIFIER_H

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

class PndGpidClassifier
{
 public:
  //! Constructor
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
  virtual void GetMvaValues(std::vector<float> EvtData,
			    std::map<std::string,float>& result) = 0;
  
  virtual const std::string& Classify(std::vector<float> EvtData)const = 0;

 protected:
  //! Data set. Holds event Weights
  PndMvaDataSet m_dataSets;
 
 private:
  PndGpidClassifier(const PndGpidClassifier& other);
  PndGpidClassifier& operator=(const PndGpidClassifier& other);
};
#endif //End of class interface
