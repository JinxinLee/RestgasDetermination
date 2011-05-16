/* ***************************************
 * MultiClass BDT Training example       *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
/*
 * Note: This is just an interface to the original TMVA
 * implementation. To find out the available options, please read TMVA
 * manuals. In case of errors or wrong outputs produced by TMVA
 * classifiers, try to read their mailing list and send your questions
 * to the same list.
 ******* VERY IMORTANT ****
 * You NEED TMVA version > 4.1.X before this works.
 */
#include <iostream>

#include "PndMultiClassBdtTrain.h"

int main(int argc, char** argv)
{
  if(argc < 2)
  {
    std::cerr << "<ERROR>\n\t<USAGE>: " << argv[0]
	      << " <Events inFile> \n";
    exit(10);
  }
  
  std::string EventsFile = argv[1];

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

  PndMultiClassBdtTrain mltr(EventsFile, labels, variables);
  mltr.SetEvaluation(true);
  mltr.Initialize();
  mltr.Train();

  return 0;
}
