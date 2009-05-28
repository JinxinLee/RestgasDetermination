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

  if(argc < 4){
    std::cerr << "\t<ERROR> Usage\n"
              <<"\t./train <InputTrainEventFeatureFile> <NumProtoTypes> <OutFile>"
              << std::endl;
    return 1;
  }
  
  std::string ip = argv[1];// InputFile
  const char* InputFile = ip.c_str();

  int numProto = 0;
  std::string numstr = argv[2];// Number of proto's
  std::istringstream buff(numstr);
  buff >> numProto;

  std::string ot   = argv[3];// OutPutFile
  const char* OutFile = ot.c_str();

  //Class names
  clas.push_back("Elect"); clas.push_back("Pion");
  clas.push_back("Kaon");  clas.push_back("Muon");
  clas.push_back("Proton"); //clas.push_back("Gamma");
  
  //Variable names 
  nam.push_back("p");  nam.push_back("tof");  nam.push_back("emc");
  nam.push_back("stt");  nam.push_back("mvd");  nam.push_back("thetaC");
  
  PndLVQTrain tr (InputFile, clas, nam);
  float initC  = 0.3;
  float ethaZ  = 0.1;
  float ethaF  = 0.00001;
  int numSweep = 1;

  tr.SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
  
  NormType ty = MINMAX;
  tr.SelNormMethod(ty);

  tr.Train  (numProto, OutFile);
  //tr.TrainSec(numProto, OutFile);
  
  //tr.Train21(numProto, OutFile);
  //tr.Train21Sec(numProto, OutFile);
  return 0;
}
/*
  std::string bla = "haphap.root";
  std::cout <<" str is " << bla << std::endl;
  std::string::size_type loc = bla.find( ".root", 0 );
  if( loc != std::string::npos ){
  std::cout << "Found Omega at " << loc << std::endl;
  }else{
  std::cout << "Didn't find Omega" << std::endl;
  }
  bla.erase(loc, bla.size());
  std::cout <<" str is " << bla << std::endl;
*/
