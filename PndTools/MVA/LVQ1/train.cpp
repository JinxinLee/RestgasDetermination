/* ***************************************
 * Author: M.Babai@rug.nl                *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight file to be used by the KNN based
 * classifier.
 */
#include <sstream>
#include "PndLVQTrain.h"
#include "TRandom3.h"

int main(int argc, char** argv)
{
  std::vector<std::string> clas;
  std::vector<std::string> nam;

  if(argc < 3){
    std::cerr << "\t<ERROR> Usage\n"
              <<"\t./train <NumProtoTypes> <OutFile>"
              << std::endl;
    return 1;
  }
  
  int numProto = 0;
  std::string numstr = argv[1];
  std::istringstream buff(numstr);
  buff >> numProto;

  std::string ot   = argv[2];
  const char* OutFile = ot.c_str();

  //Class names
  clas.push_back("Elect"); clas.push_back("Pion");
  clas.push_back("Kaon");  clas.push_back("Muon");
  clas.push_back("Proton"); //clas.push_back("Gamma");
  
  //Variable names 
  nam.push_back("p");  nam.push_back("tof");  nam.push_back("emc");
  nam.push_back("stt");  nam.push_back("mvd");  nam.push_back("thetaC");
  
  PndLVQTrain tr ("../Gpid_files/EventFeaturesTrain.root", clas, nam);
  //tr.SetLearnPrameters(0.3, 0.1, 0.00001, 1000);
  tr.SetLearnPrameters(0.3, 0.1, 0.00001, 10);
  
  tr.Train(numProto,OutFile);
  //tr.Train21(numProto,OutFile);
  return 0;
}
