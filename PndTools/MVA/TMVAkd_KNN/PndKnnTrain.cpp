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
  {std::cout << "Trim." << std::endl;}
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
  WriteDataSetToOutFile();
}
