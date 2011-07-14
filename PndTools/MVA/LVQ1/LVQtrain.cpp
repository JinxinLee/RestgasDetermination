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
#include "TRandom3.h"

#include "PndLVQTrain.h"
#include "PndMvaTools.h"

//____________________________________
#define DEBUGVQ_TRAIN_EXAMPLE 0

#define CROSS_VALIDATE 1
//____________________________________

#if DEBUGVQ_TRAIN_EXAMPLE
// *************  DEBUG ONLY **********
void printProto(std::vector< std::pair<std::string, std::vector<float>*> > const& dat)
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
#endif

int main(int argc, char** argv)
{
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
  std::cout << "Using algoritme " << algNum
	    << '\n';

  size_t numProto = 0;
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
  std::cout << " ErrorFile = " << OutErr
	    << '\n';
  
  // Labels.
  std::vector<std::string> labels;
  // Variables.
  std::vector<std::string> vars;
  
  // Labelss names
  labels.push_back("electron");
  labels.push_back("pion");
  
  //labels.push_back("muon");
  //labels.push_back("kaon");
  //labels.push_back("proton");
  
  // Variable names 
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  
  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("tof");
  //vars.push_back("stt"); 
  
#if (CROSS_VALIDATE == 0)
  std::cout << "<-I-> Init classifier using Tree data.\n";
  // Create trainer object.
  PndLVQTrain tr(ip, labels, vars, true);

#else
  std::cout << "<-I-> Init classifier using PreInitialized"
	    << " vector of events.\n";
  std::vector<std::pair<std::string, std::vector<float>*> > events;

  std::map<std::string, size_t>* counts = readEvents (ip.c_str(), vars, labels, events);

  // Create trainer object.
  PndLVQTrain tr(events, labels, vars, true);

  // Clean-up un-needed stuff
  for(size_t ev = 0; ev < events.size(); ++ev)
  {
    delete events[ev].second;
  }
  events.clear();
  counts->clear();
  delete counts;
#endif

  float initC  = 0.8;
  float ethaZ  = 0.1;
  float ethaF  = 0.0001;
  
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
  
  // Set the size of the test set in (%)
  tr.SetTetsSetSize(0);

  tr.SetErrorStepSize(0);//1000 (DEFALUT)
  
  //VARX, MINMAX, MEDIAN, NONORM(DEFAULT)
  //tr.NormalizeData(VARX);

  // Perform PCA on input events.
  //tr.PCATransForm();
  
  // Specify the file containing pre-initialized prototypes.
  //tr.SetInitProtoFileName("InitialProto.root");
  
  // FILE_PR, KMEANS_PR, CCM_PR, RAND_FROM_DATA (DEFAULT)
  //tr.setProtoInitType(RAND_FROM_DATA);
  
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

#if DEBUGVQ_TRAIN_EXAMPLE
  case 3:
    printProto( tr.train1sec() );
    break;
  case 4:
    printProto( tr.train2sec() );
    break;
#endif

  default:
    std::cerr << "No algorithm selected"
	      << std::endl;
    break;
  }

  // Write out the error info.
  tr.WriteErroVect(OutErr);
  return 0;
}
