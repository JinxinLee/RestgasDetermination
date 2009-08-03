#pragma once
#ifndef PndKnnTrain_H
#define PndKnnTrain_H

#include "PndMvaTrainer.h"

class PndKnnTrain : public PndMvaTrainer
{
 public:
  /**
   * Constructor.
   * @param OutPut: File name to hold the weight values.
   * @param ClassNames: Class names for wich the classifier is
   * trained.
   * @param VarNames: Variable names creating the feature vector.
   */
  PndKnnTrain(const std::string& InputFile, 
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames,
	      bool trim);
  /**
   * Class destructor.
   */
  virtual ~PndKnnTrain();
  
  /**
   * Train the classifier.
   */
  void Train();
  
 private:
  PndKnnTrain(const PndKnnTrain& other);
  PndKnnTrain& operator=(const PndKnnTrain& other);
};
#endif//end of interface definition
