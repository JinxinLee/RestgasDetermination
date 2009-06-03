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

  if(argc < 5){
    std::cerr << "\t<ERROR> Usage\n"
              <<"\t./train <algNum> <NumProtoTypes> <InputTrainEventFeatureFile> <OutFile>"
              << std::endl;
    return 1;
  }
  
  int algNum = atoi(argv[1]);
  std::cout << "Using algoritme " << algNum << std::endl;

  int numProto = 0;
  std::string numstr = argv[2];// Number of proto's
  std::istringstream buff(numstr);
  buff >> numProto;

  std::string ip = argv[3];// InputFile
  const char* InputFile = ip.c_str();

  std::string ot      = argv[4];// OutPutFile


  //Class names
  /*
  clas.push_back("electron"); clas.push_back("pion");
  clas.push_back("kaon");  clas.push_back("muon");
  clas.push_back("proton"); //clas.push_back("gamma");
  */
  clas.push_back("Elect"); clas.push_back("Pion");
  clas.push_back("Kaon");  clas.push_back("Muon");
  clas.push_back("Proton"); //clas.push_back("gamma");

  //Variable names 
  nam.push_back("p");  nam.push_back("tof");  nam.push_back("emc");
  nam.push_back("stt");  nam.push_back("mvd");  nam.push_back("thetaC");
  
  PndLVQTrain tr(InputFile, clas, nam);
  float initC  = 0.9;
  float ethaZ  = 0.1;
  float ethaF  = 0.00001;
  int numSweep = 500;

  tr.SetLearnPrameters(initC, ethaZ, ethaF, numSweep);

  //NormType ty = MINMAX;
  //tr.SelNormMethod(ty);

  std::string OutFile = ot;

  switch(algNum)
  {
  case 1:
    tr.Train(numProto, OutFile.c_str());
    break;
  case 2:
    tr.TrainSec(numProto, OutFile.c_str());
    break;
  case 3:
      tr.Train21(numProto, OutFile.c_str());
      break;
  case 4:
      tr.Train21Sec(numProto, OutFile.c_str());
    break;
  default:
    std::cerr << "No algorithm selected" << std::endl;
  }
  return 0;
}
