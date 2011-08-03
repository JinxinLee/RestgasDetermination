/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid classifier                    *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/
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
  PndKnnTrain(const std::string& InputFile, 
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames,
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
  PndKnnTrain(const PndKnnTrain& other);
  PndKnnTrain& operator=(const PndKnnTrain& other);
};
//end of interface definition
#endif
