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

// C++ && C
//#include <sstream>
#ifdef _OPENMP
#include <omp.h>
#endif

// ROOT
#include "TRandom3.h"

// LOCAL includes
#include "PndLVQTrain.h"
#include "PndMvaTools.h"

//____________________________________
#define DEBUGVQ_TRAIN_EXAMPLE 2

/*
 * 0 = no crossvalidation, Normal init or read from pre init vector
 * 1 = crossvalidation using root trees
 */
#define CROSS_VALIDATE 1

// k-Fold CV
#define NUMBER_OF_FOLDS 10
//____________________________________

#if DEBUGVQ_TRAIN_EXAMPLE > 0
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

void printErrors(PndLVQTrain const& tr)
{
  // Note: Can produce large output on STDOUT.
  std::vector <StepError> const& Error = tr.GetErrorValues();
  for(size_t i = 0; i < Error.size(); ++i)
  {
    std::cout << " Index = "      << i
	      << "\n\tm_step  = " << Error[i].m_step
	      << "\n\tm_trErr = " << Error[i].m_trErr
	      << "\n\tm_tsErr = " << Error[i].m_tsErr
	      << '\n';
  }
}
// *************  DEBUG ONLY **********
#endif// DEBUG

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
  // Select alg.  
  int algNum = atoi(argv[1]);
  std::cout << "Using algoritme " << algNum
	    << '\n';

  // Number of proto's
  size_t numProto = str2Uint(argv[2]);
  std::cout << "\tnumProto = " << numProto << ',';

  // Number of sweeps.
  int numSweep = str2int(argv[3]);
  std::cout << " numSweep = " << numSweep << ',';
  
  // Input file name.
  std::string ip = argv[4];
  std::cout << " TrainFile = " << ip << ',';
  
  // OutPutFile
  std::string ot = argv[5];
  std::cout << " outPut = " << ot << ',';
  
  // Evaluation output file name.
  std::string OutErr = "EValuation" + ot;
  std::cout << " EValuation File = " << OutErr
	    << '\n';
  
  //// Labelss names
  std::vector<std::string> labels;

  labels.push_back("electron");
  labels.push_back("pion");
  //labels.push_back("muon");
  //labels.push_back("kaon");
  //labels.push_back("proton");

  //// Variable names
  std::vector<std::string> vars;

  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  
  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("tof");
  //vars.push_back("stt"); 

  // Use for asymm. init.
  /*
    std::map <std::string, unsigned int> numProtoMap;
    numProtoMap["electron"] = 70;
    numProtoMap["pion"] = 150;
  */
  float initC  = 0.8;
  float ethaZ  = 0.1;
  float ethaF  = 0.0001;

#if CROSS_VALIDATE == 0
  //========= Normal training =====================
  std::cout << "<-I-> Init classifier using data from root Trees.\n";

  // Create trainer object.
  PndLVQTrain tr(ip, labels, vars, true);

  tr.SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
  
  // Use for symm. initialization.
  tr.SetNumberOfProto(numProto);

  // Use for asymm. init.
  // tr.SetNumberOfProto(numProtoMap);
  
  // Set the size of the test set in (%)
  tr.SetTetsSetSize(20);
  
  tr.SetErrorStepSize(0);//1000 (DEFALUT)
  
  //VARX, MINMAX, MEDIAN, NONORM(DEFAULT)
  // tr.NormalizeData(VARX);
  
  // Perform PCA on input events.
  // tr.PCATransForm();
  
  // Specify the file containing pre-initialized prototypes.
  // tr.SetInitProtoFileName("InitialProto.root");
  
  // FILE_PR, KMEANS_PR, CCM_PR, RAND_FROM_DATA (DEFAULT)
  // tr.setProtoInitType(RAND_FROM_DATA);
  
  // Set outPut file name to store weights.
  tr.SetOutPutFile(ot);
  
  // Init trainer
  tr.Initialize();
  
  // Select algorithm.
  switch(algNum)
  {
  case 1:
    tr.Train();
    break;
  case 2:
    tr.Train21();
    break;
    
#if DEBUGVQ_TRAIN_EXAMPLE > 0
  case 3:
    printProto( tr.train1sec() );
    break;
  case 4:
    printProto( tr.train2sec() );
    break;
#endif//DEBUG
    
  default:
    std::cerr << "No algorithm selected"
	      << std::endl;
    break;
  }
  
  // Store weights
  tr.storeWeights();
  
  // Write out the error info.
  tr.WriteErroVect(OutErr);
  
  // Print the evaluation results.
#if DEBUGVQ_TRAIN_EXAMPLE > 0
  printErrors(tr);
#endif//DEBUG
  
#else// CROSS_VALIDATE != 0
  /*
   * We want to do cross-validation. So first create k-disjoint subsets
   * of the data point indices and use this to set the test set for
   * each of k-classifiers.
   */
  PndMvaDataSet* dat = new PndMvaDataSet(ip, labels, vars, TRAIN);
  dat->SetTrim(true);
  dat->Initialize();

  // Get available event data.
  std::vector< std::pair<std::string, std::vector<float>* > > const& evts = dat->GetData();
  std::cout << "<-In-> Total number of available events = "
	    << evts.size() << '\n';

  // Get list of labels and their attributes..
  std::vector<PndMvaClass> const& clsList = dat->GetClasses();

#if DEBUGVQ_TRAIN_EXAMPLE > 0
  for(size_t cl = 0; cl < clsList.size(); ++cl)
  {
    std::cout << "Name = "       << clsList[cl].Name
	      << " NExamples = " << clsList[cl].NExamples
	      << " StartIdx  = " << clsList[cl].StartIdx
	      << " EndIdx    = " << clsList[cl].EndIdx
	      << " NTrainEx  = " << clsList[cl].NTrainEx
	      <<'\n';
  }
#endif//DEBUG

  // Determine how many events per set per label
  size_t subsetSize = evts.size() / NUMBER_OF_FOLDS;
  size_t perClassNumbers = subsetSize / clsList.size();

  std::cerr << "<-In-> Each subset (test set) containes " << subsetSize
	    << " Events, and " << perClassNumbers
	    << " events per available label.\n";
  
  //============= Create k- test set indices.
  // Name to index map Per label.
  std::map<std::string, size_t> nameIdx;
  for(size_t cl = 0; cl < clsList.size(); ++cl)
  {
    nameIdx[clsList[cl].Name] = clsList[cl].StartIdx;
  }

  // Container to hold test set indices.
  std::vector< std::set <size_t> > testSets;

  // Create test sets and add to the container.
  for(size_t f = 0; f < NUMBER_OF_FOLDS; ++f)
  {
    std::set <size_t> curTsSet;
    // Labels loop
    for(size_t cl = 0; cl < clsList.size(); ++cl)
    {
      std::string curClsName = clsList[cl].Name;
      // Select events indices for each label
      for(size_t ex = 0; ex < perClassNumbers; ++ex)
      {
	curTsSet.insert(nameIdx[curClsName]);
	nameIdx[curClsName] += 1;
      }// EVENTS
    }// LABELS
    testSets.push_back(curTsSet);
  }// FOLDS

  //___ TEst sets are ready.
  nameIdx.clear();

  //========= DEBUG DEBUG DEBUG .
#if DEBUGVQ_TRAIN_EXAMPLE > 0
  std::cout << "________ " << testSets.size() << " == "
	    << NUMBER_OF_FOLDS
	    << '\n';
  
  size_t sum = 0;
  for(size_t ts = 0; ts < testSets.size(); ++ts)
  {
    std::cout << testSets[ts].size() << " ";
    sum += testSets[ts].size();
  }

  // Total number of elements in all subsets.
  std::cout << "\nSum = " << sum
	    << '\n';

#if DEBUGVQ_TRAIN_EXAMPLE > 1
  // Check if disjoint using stl functions.
  std::vector<size_t> v( (2 * perClassNumbers) );
  std::vector<size_t>::iterator it;
  
  it = std::set_intersection (testSets[0].begin(), testSets[0].end(),
			      testSets[1].begin(), testSets[1].end(),
			      v.begin());
  std::cout << " STL:: intersection has " << int(it - v.begin())
	    << " elements.\n";
  v.clear();
#endif//DEBUG > 1

  bool disj = true;
  for(size_t st = 0; st < (testSets.size() -1); ++st)
  {
    disj = disj && is_disjoint( testSets[st], testSets[st+1] );
  }
  if(disj){std::cout << "All Disjoint\n";}
  else{std::cout << "NOT ALL Disjoint\n";}
#endif// DEBUG

  // We need K classifiers.
  std::vector<PndLVQTrain*> trainerList;
  
  // Create trainer objects
  for(size_t i = 0; i < NUMBER_OF_FOLDS; ++i)
  {
    // Create and init trainers
    PndLVQTrain* t = new PndLVQTrain(evts, labels, vars, true);

    // Set parameters
    t->SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
    
    // Symm. number of proto.
    t->SetNumberOfProto(numProto);
    
    // Set testSet size or indices
    t->SetTetsSetSize(0);
    t->SetTestSet(testSets[i]);
    
    // Eval. rate.
    t->SetErrorStepSize(0);
    
    // OutFile names.
    std::string prefix = int2str(i);
    t->SetOutPutFile( (prefix + "_" + ot));
    
    // Initialize
    t->Initialize();

    // Add to container.
    trainerList.push_back(t);
  }
  // We have created the trainers, thus we can clean-up memory before
  // performing the training.
  delete dat;
  testSets.clear();

  /*
   * Perform training for all K-classifiers.
   *
   * NOTE: Number of started threads equal to the number of available
   * cores on the system. If you do not want this, then you should set
   * the number of threads.
   */
#ifdef _OPENMP
#pragma omp parallel for  schedule(dynamic)
#endif// OPENMP
  for(size_t i = 0; i < trainerList.size(); ++i)
  {
    (trainerList[i])->Train();
  }
  
  // ========= Store weights
  for(size_t i = 0; i < trainerList.size(); ++i)
  {
    (trainerList[i])->storeWeights();
    // Write out the error info.
    (trainerList[i])->WriteErroVect( (int2str(i)+ "_" + OutErr) );
  }

  // Print the evaluation results.
#if DEBUGVQ_TRAIN_EXAMPLE > 0
  for(size_t i = 0; i < trainerList.size(); ++i)
  {
    printErrors( *(trainerList[i]) );
  }
#endif// DEBUG
  
  //======= Clean trainers list ======
  for(size_t j = 0; j < NUMBER_OF_FOLDS; ++j)
  {
    delete trainerList[j];
  }
  //_____________________________
#endif// CROSS_VALIDATE != 0
  
  return 0;
}
