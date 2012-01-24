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
PndKnnTrain::PndKnnTrain(std::string const& inputFile, 
			 std::vector<std::string> const& ClassNames, 
			 std::vector<std::string> const& VarNames,
			 bool trim)
  : PndMvaTrainer(inputFile, ClassNames, VarNames, trim)
{}

/**
 * Class Destructor.
 */
PndKnnTrain::~PndKnnTrain()
{}

/**
 * Fill the Signal trees and train the classifier.
 */
void PndKnnTrain::Train()
{}

/**
 * Store weights in the output File. If output file name is not
 * specified, then write nothing.
 */
void PndKnnTrain::storeWeights()
{
  // Fetch available data.
  std::vector< std::pair<std::string, std::vector<float>*> > const& w = m_dataSets.GetData();
  
  // Write to output file
  WriteToWeightFile(w);
}
