#include "PndMultiClassMlpTrain.h"

PndMultiClassMlpTrain::PndMultiClassMlpTrain(const std::string& InPut,
					     const std::vector<std::string>& ClassNames, 
					     const std::vector<std::string>& VarNames,
					     bool trim)
  : PndMvaTrainer(InPut, ClassNames, VarNames, trim)
{}

PndMultiClassMlpTrain::~PndMultiClassMlpTrain()
{}
  
void PndMultiClassMlpTrain::Train()
{}
