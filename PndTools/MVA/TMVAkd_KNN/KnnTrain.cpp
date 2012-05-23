/* ***************************************
 * Author: M.Babai@rug.nl                *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight file to be used by the KNN based
 * classifier.
 */
#include "PndKnnTrain.h"

int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cerr << "\t<ERROR> Usage\n"
	      <<"\t./train <InFile> <OutFile>"
	      << std::endl;
    return 1;
  }

  // Input file  
  std::string inpt   = argv[1];

  // Out put file name to write the weights.
  std::string oupt   = argv[2];

  // Labels.
  std::vector<std::string> label;

  // Variable names.
  std::vector<std::string> vars;
  
  // Class Names  
  label.push_back("electron");
  label.push_back("pion"); 

  //label.push_back("kaon");
  //label.push_back("muon");
  //label.push_back("proton");
  
  // Parameter Names
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  
  // Create classifier object.
  PndKnnTrain kNNtr ( inpt, label, vars, true);
  
  //NormType tt = VARX;
  //kNNtr.NormalizeData(VARX);
  //kNNtr.NormalizeData(MINMAX);
  //kNNtr.NormalizeData(MEDIAN);
  
  // Transform using PCA
  //kNNtr.PCATransForm();
  
  // Set the size of the test set.
  kNNtr.SetTestSetSize(0);

  // Set output file name
  kNNtr.SetOutPutFile(oupt);

  // Init and set data structures
  kNNtr.Initialize();
  
  // Perform training
  kNNtr.Train();// Currently it does nothing.

  // Store weights.
  kNNtr.storeWeights();

  return 0;
}
