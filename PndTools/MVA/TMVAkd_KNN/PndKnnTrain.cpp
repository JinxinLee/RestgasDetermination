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
{
  // Write to output file
  // WriteDataSetToOutFile();

  // Fetch available data.
  std::vector< std::pair<std::string, std::vector<float>*> > const &w = m_dataSets.GetData();
  // Write
  WriteToWeightFile(w);
}
