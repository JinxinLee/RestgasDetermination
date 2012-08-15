/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * LICENSE:                              *
 * Version:                              *
 * License:                              *
 * ***************************************
 */
#pragma once
#ifndef PND_LVQ_CLASSIFY_H
#define PND_LVQ_CLASSIFY_H

// Standard C++ libraries
#include <limits>

// Local includes
#include "PndMvaClassifier.h"

//! Interface definition of the LVQ classifier.
class PndLVQClassify: public PndMvaClassifier
{
 public:
  /**
   * Constructor:
   * @param inputFile: Input file name (Weights).
   * @param classNames: class names.
   * @param varNames: variable names of the features.
   */
  explicit PndLVQClassify(std::string const& inputFile,
			  std::vector<std::string> const& classNames, 
			  std::vector<std::string> const& varNames);
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
  /**
   * Given a feature vector describing the pattern. Classifies the pattern.
   *@param EvtData Input vector describing the pattern.
   *@return The name of the class to which the current pattern is assigned.
   */
  std::string* Classify(std::vector<float> EvtData);
  
 private:
  //! To avoid mistakes (No deep copy)
  PndLVQClassify(PndLVQClassify const& other);
  PndLVQClassify& operator=(PndLVQClassify const& other);
};
//end of interface definition
#endif
