/* ************************************
 *  Author: M. Babai (M.Babai@rug.nl) *
 *                                    *
 *  pid Trainer                       *
 *                                    *
 * Modified:                          *
 *                                    *
 * ************************************/

#include "PndKnnTrain.h"

/**
 * @param InputFile: Example event input File name.
 * @param ClassNames: Class Names.
 * @param VarNames: Variable names creating the feature vector.
 */
PndKnnTrain::PndKnnTrain(const std::string& inputFile, 
			 const std::vector<std::string>& ClassNames, 
			 const std::vector<std::string>& VarNames,
			 bool trim)
  : PndMvaTrainer(inputFile, ClassNames, VarNames, trim)
{
  if(trim)
  {
    std::cout << "Trim." << '\n';
  }
}

/**
 * Class Destructor.
 */
PndKnnTrain::~PndKnnTrain()
{}

/**
 * Fill the Signal trees and train the classifier.
 */
void PndKnnTrain::Train()
{
  // Write to output file
  WriteDataSetToOutFile();
}
