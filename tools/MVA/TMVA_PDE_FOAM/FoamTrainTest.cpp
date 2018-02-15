#include "PndPDEFoamTrain.h"


int main(int argc, char** argv)
{
  if(argc < 3){
    std::cerr << "\t<ERROR> Usage\n"
              <<"\t./FoamTrainTest <InputTrainEventFeatureFile> <Output foam file>"
              << std::endl;
    return 1;
  }

  std::vector<std::string> vars,clas;

  // ==== Class names
  //clas.push_back("electron"); clas.push_back("pion");
  // clas.push_back("kaon"); clas.push_back("muon");
  //clas.push_back("proton");   //clas.push_back("gamma");

  clas.push_back("Elect"); clas.push_back("Pion");
  //clas.push_back("Kaon");  clas.push_back("Muon");
  //clas.push_back("Proton"); clas.push_back("Gamma");

  // ==== Variable names 
  vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("thetaC");
  vars.push_back("mvd");
  vars.push_back("tof");
  //vars.push_back("stt"); 

  std::string inFile = argv[1];
  std::string OutFile = argv[2];

  PndPDEFoamTrain foam (inFile, clas, vars);

  foam.splitTetsSet(0);//10 % (DEFAULT)

  //VARX, MINMAX, MEDIAN, NONE(DEFAULT)
  foam.NormalizeData(VARX);

  foam.SetOutPutFile(OutFile);

  //foam.Train();
  foam.TrainPar();
  //  foam.TestFoam();


  return 0;
}
