/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * License:                              *
 * ***************************************
 */

//#pragma once
#ifndef PndLVQClassify_H
#define PndLVQClassify_H

// Standard C++ libraries
#include <limits>

// Local includes
#include "PndGpidClassifier.h"

class PndLVQClassify: public PndGpidClassifier
{
 public:
  /**
   * Constructor:
   * @param inputFile: Input file name (Weights).
   * @param classNames: class names.
   * @param varNames: variable names of the features.
   */
  PndLVQClassify(const std::string& inputFile,
		 const std::vector<std::string>& classNames, 
		 const std::vector<std::string>& varNames);
  /**
   * Destructor
   */
  virtual ~PndLVQClassify();
  
  /**
   * @param eventData: Event data to be classified.
   *
   * @param result:  Classification results. Currently the shortest
   *  distance for each class is stored in result.
   */
  void GetMvaValues(std::vector<float> eventData, 
		    std::map<std::string, float>& result);

  const std::string& Classify(std::vector<float> EvtData)const;

 private:
  PndLVQClassify(const PndLVQClassify& other);
  PndLVQClassify& operator=(const PndLVQClassify& other);
};
#endif//end of interface definition
