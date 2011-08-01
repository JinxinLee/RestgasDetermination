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

// 0 = no crossvalidation, 1 = crossvalidation
#define CROSS_VALIDATE 1

// k-Fold CV
#define NUMBER_OF_FOLDS 10
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
  
  std::string OutErr = "EValuation" + ot;
  std::cout << " EValuation File = " << OutErr
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
  std::cout << "<-I-> Init classifier using data from root Trees.\n";
  // Create trainer object.
  PndLVQTrain tr(ip, labels, vars, true);

#else
  std::cout << "<-I-> Init classifier using PreInitialized"
	    << " vector of events.\n";
  // Store events
  std::vector<std::pair<std::string, std::vector<float>*> > events;
  
  // Read event features and store.
  std::map<std::string, size_t>* counts = readEvents (ip.c_str(), vars, labels, events);
  
  /*
   * We want to do cross-validation. So first create k-disjoint subsets
   * of the data point indices and use this to set the test set for
   * each of k-classifiers.
   */
  size_t subsetSize = events.size() / NUMBER_OF_FOLDS;
  std::cerr << "Each subset containes " << subsetSize
	    << " Events.\n";

  std::vector< std::set <size_t> > TestSets;

  for(size_t ts = 0; ts < NUMBER_OF_FOLDS; ++ts)
  {
    TestSets.size();
  }

  // Create trainer object.
  PndLVQTrain tr(events, labels, vars, true);
  
  //_________ Clean-up un-needed stuff __________
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
  
#if (CROSS_VALIDATE == 0)
  // Set the size of the test set in (%)
  tr.SetTetsSetSize(20);
#else
  // We specify the test set ourselves.
  tr.SetTetsSetSize(20);
  std::set <size_t> const& bla = tr.GetTestEvetIdx();
  tr.SetTestSet(bla);
#endif

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

#if (CROSS_VALIDATE == 0)
  // Write out the error info.
  tr.WriteErroVect(OutErr);

#else
  //tr.EvalClassifierError();
  std::vector <StepError> const& Error = tr.GetErrorValues();
  for(size_t i = 0; i < Error.size(); ++i)
  {
    std::cout << " Index = "    << i
	      << "\n\tm_step  = " << Error[i].m_step
	      << "\n\tm_trErr = " << Error[i].m_trErr
	      << "\n\tm_tsErr = " << Error[i].m_tsErr
	      << '\n';
  }
#endif

  return 0;
}
