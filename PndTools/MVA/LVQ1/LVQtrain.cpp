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

// *************  DEBUG ONLY **********
void printProto(const std::vector< std::pair<std::string, std::vector<float>*> >& dat)
{
  std::cout << "==========================\n";
  for(size_t i = 0; i < dat.size(); i++)
  {
    std::cout << dat[i].first << " => ";
    std::vector<float>* example = dat[i].second;
    std::cout << " ( ";
    for(size_t j = 0; j < example->size(); j++)
    {
      std::cout << example->at(j) << "  ";
    }
    std::cout << ").\n";
  }
  std::cout << "==========================\n";
}
// *************  DEBUG ONLY **********

int main(int argc, char** argv)
{
  // Labels.
  std::vector<std::string> clas;
  // Variables.
  std::vector<std::string> nam;
  
  if(argc < 5)
  {
    std::cerr << "\t<ERROR> Usage:\n"
              <<'\t' << argv[0] 
	      << " <algNum> <NumProtoTypes> <numSweep>"
	      <<" <InputTrainEventFeatureFile> <OutFile>"
              << std::endl;
    return 1;
  }
  
  int algNum = atoi(argv[1]);
  std::cout << "Using algoritme " << algNum << '\n';

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
  
  std::string OutErr = "Err" + ot;
  std::cout << " ErrorFile = " << OutErr << '\n';
  
  // Class names
  clas.push_back("electron");
  clas.push_back("pion");
  
  //clas.push_back("kaon");
  //clas.push_back("muon");
  //clas.push_back("proton");
  
  // Variable names 
  //nam.push_back("p");
  nam.push_back("emc");
  nam.push_back("lat");
  nam.push_back("z20");
  nam.push_back("z53");
  
  //nam.push_back("thetaC");
  //nam.push_back("mvd");
  //nam.push_back("tof");
  //nam.push_back("stt"); 
  
  // Create trainer object.
  PndLVQTrain tr(ip, clas, nam, true);

  float initC  = 0.8;
  float ethaZ  = 0.1;
  float ethaF  = 0.001;
  
  tr.SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
  
  // Use for symm. initialization.
  tr.SetNumberOfProto(numProto);

  // Use for asymm. init.
  /*
    std::map <std::string, unsigned int> numProtoMap;
    numProtoMap["electron"] = 70;
    numProtoMap["pion"] = 150;
    tr.SetNumberOfProto(numProtoMap);
  */

  tr.SetTetsSetSize(50);//10% (DEFAULT)

  tr.SetErrorStepSize(100);//100 (DEFALUT)
  
  //VARX, MINMAX, MEDIAN, NONORM(DEFAULT)
  //tr.NormalizeData(VARX);

  // Perform PCA on input events.
  //tr.PCATransForm();
  
  // Specify the file containing pre-initialized prototypes.
  //tr.SetInitProtoFileName("InitialProto.root");
  
  // FILE_PR, KMEANS_PR, RANDOM_PR, RAND_FROM_DATA (DEFAULT)
  tr.setProtoInitType(RAND_FROM_DATA);
  
  // Set outPut file name to store weights.
  tr.SetOutPutFile(ot);

  tr.Initialize();
  
  switch(algNum)
  {
  case 1:
    tr.Train();
    break;
  case 2:
    tr.Train21();
    break;
  case 3:
    printProto( tr.train1sec() );
    break;
  case 4:
    printProto( tr.train2sec() );
  default:
    std::cerr << "No algorithm selected"
	      << std::endl;
    break;
  }
  // Write out the error info.
  tr.WriteErroVect(OutErr);
  return 0;
}
