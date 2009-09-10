/* ***************************************
 * Author: M. Babai                      *
 * M.Babai@rug.nl                        *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the train procedure, in
 * order to generate weight (Proto types) file to be used by the LVQ
 * based classifier.
 */
#include <sstream>
#include "PndLVQTrain.h"
#include "TRandom3.h"

int main(int argc, char** argv)
{
  std::vector<std::string> clas;
  std::vector<std::string> nam;

  if(argc < 6){
    std::cerr << "\t<ERROR> Usage\n"
              <<"\t./train <algNum> <NumProtoTypes> <numSweep> <InputTrainEventFeatureFile> <OutFile>"
	      << " <ErroFileName>"
              << std::endl;
    return 1;
  }
  
  int algNum = atoi(argv[1]);
  std::cout << "Using algoritme " << algNum << std::endl;

  unsigned int numProto = 0;
  std::string numstr = argv[2];// Number of proto's
  std::istringstream buff(numstr);
  buff >> numProto;
  std::cout << " numProto = " << numProto << " ";

  int numSweep = atoi(argv[3]);
  std::cout << " numSweep = " << numSweep << " ";

  std::string ip = argv[4];// InputFile
  std::cout << " TrainFile = " << ip << " ";

  std::string ot      = argv[5];// OutPutFile
  std::cout << " outPut = " << ot << " ";

  std::string OutErr = argv[6];
  std::cout << " ErrorFile = " << OutErr << std::endl;


  //Class names
    
  //clas.push_back("electron"); clas.push_back("pion");
  //clas.push_back("kaon");     clas.push_back("muon");
  //clas.push_back("proton");   //clas.push_back("gamma");


  clas.push_back("Elect"); clas.push_back("Pion");
  //clas.push_back("Kaon");  clas.push_back("Muon");
  //clas.push_back("Proton"); //clas.push_back("gamma");

  //Variable names 
  nam.push_back("p");
  nam.push_back("emc");
  nam.push_back("thetaC");
  nam.push_back("mvd");
  nam.push_back("tof");
  //nam.push_back("stt"); 

  PndLVQTrain tr(ip, clas, nam, true);
  float initC  = 0.9;
  float ethaZ  = 0.1;
  float ethaF  = 0.0001;

  tr.SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
  tr.SetNumberOfProto(numProto);
  tr.splitTetsSet(10);//10 % (DEFAULT)

  //VARX, MINMAX, MEDIAN, NONE(DEFAULT)
  tr.NormalizeData(VARX);
  
  std::string OutFile = ot;
  tr.SetOutPutFile(ot);

  switch(algNum)
  {
  case 1:
    tr.Train();
    break;
  case 2:
    tr.Train21();
    break;
  case 3:
    tr.TrainSec();
    break;
  case 4:
    tr.Train21Sec();
    break;
  default:
    std::cerr << "No algorithm selected" << std::endl;
  }

  tr.WriteErroVect(OutErr);
  return 0;
}
