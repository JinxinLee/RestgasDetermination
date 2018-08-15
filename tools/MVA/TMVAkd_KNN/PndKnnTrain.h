/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/
//#pragma once
#ifndef PND_KNN_TRAIN_H
#define PND_KNN_TRAIN_H

#include "PndMvaTrainer.h"

class PndKnnTrain : public PndMvaTrainer
{
 public:
  /**
   * Constructor.
   * @param InputFile: Example event input File name.
   * @param ClassNames: Class names for wich the classifier is
   * trained.
   * @param VarNames: Variable names creating the feature vector.
   */
  explicit PndKnnTrain(std::string const& InputFile,
		       std::vector<std::string> const& ClassNames, 
		       std::vector<std::string> const& VarNames,
		       bool trim = true);
  /**
   * Class destructor.
   */
  virtual ~PndKnnTrain();
  
  /**
   * Train the classifier.
   */
  void Train();

  /**
   * Store weights in the output File. If output file name is not
   * specified, then write nothing.
   */
  void storeWeights();

 private:
  //! To avoid mistakes
  PndKnnTrain( PndKnnTrain const& other);
  PndKnnTrain& operator=( PndKnnTrain const& other);
};
//end of interface definition
#endif
