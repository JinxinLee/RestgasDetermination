/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * Edited: E.A.Dijck@student.rug.nl      *
 * LICENSE:                              *
 * Version:                              *
 * License:                              *
 * ***************************************
 */
#ifndef PND_LVQ_CLASSIFY_H
#define PND_LVQ_CLASSIFY_H

// Standard C++ libraries
#include <limits>

// Local includes
#include "PndGpidClassifier.h"

//! Interface definition of the LVQ classifier.
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
  /**
   * Given a feature vector describing the pattern. Classifies the pattern.
   *@param EvtData Input vector describing the pattern.
   *@return The name of the class to which the current pattern is assigned.
   */
  std::string* Classify(std::vector<float> EvtData);

 private:
  //! To avoid mistakes
  PndLVQClassify(const PndLVQClassify& other);
  PndLVQClassify& operator=(const PndLVQClassify& other);
};
#endif//end of interface definition
