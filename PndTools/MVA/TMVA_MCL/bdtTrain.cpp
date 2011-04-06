#include <iostream>

#include "PndMultiClassBdtTrain.h"

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cerr << "<USAGE>: " << argv[0]
	      << " <inputFile> \n";
    exit(10);
  }
  
  std::string inFile = argv[1];

  // Labels.
  std::vector<std::string> labels;
  // Variables.
  std::vector<std::string> variables;

  // Class names
  labels.push_back("electron");
  labels.push_back("pion");
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");
  
  // Variable names 
  variables.push_back("emc");
  variables.push_back("lat");
  variables.push_back("z20");
  variables.push_back("z53");

  PndMultiClassBdtTrain mltr(inFile, labels, variables);
  mltr.SetEvaluation(true);
  mltr.Initialize();
  mltr.Train();

  return 0;
}
