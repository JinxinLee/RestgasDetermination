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
#ifdef _OPENMP
#include <omp.h>
#endif

// ROOT
#include "TRandom3.h"

// LOCAL includes
#include "PndLVQTrain.h"
#include "PndMvaTools.h"

//____________________________________
/*
 * 0 = no crossvalidation, Normal init or read from pre init vector
 * 1 = crossvalidation using root trees
 */
#define CROSS_VALIDATE 0

#if (CROSS_VALIDATE != 0)
#define NUMBER_OF_FOLDS 10 // k-Fold CV
#endif

// Print DEBUG information
#define DEBUGVQ_TRAIN_EXAMPLE 0
//____________________________________

#if (DEBUGVQ_TRAIN_EXAMPLE > 0)
// *************  DEBUG ONLY **********
// Prints the given prototypes list.
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

// Prints Error vector.
void printErrors(PndLVQTrain const& tr)
{
  // Note: Can produce large output on STDOUT.
  std::map<std::string, float>::const_iterator it;
  std::vector <StepError> const& Error = tr.GetErrorValues();
  for(size_t i = 0; i < Error.size(); ++i)
  {
    std::cout << " Index = "      << i
	      << "\n\tm_step  = " << Error[i].m_step
	      << "\n\tm_trErr = " << Error[i].m_trErr
	      << "\n\tm_tsErr = " << Error[i].m_tsErr
	      << "\n\tm_MisClsTest: ";
    for(it = (Error[i].m_MisClsTest).begin(); it != (Error[i].m_MisClsTest).end(); ++it)
    {
      std::cout << it->first << ", " << it->second << ' ';
    }
    std::cout << "\n\tm_MisClsTrain: ";
    for(it = (Error[i].m_MisClsTrain).begin(); it != (Error[i].m_MisClsTrain).end(); ++it)
    {
      std::cout << it->first << ", " << it->second << ' ';
    }
    std::cout << '\n';
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
  int algNum = str2int(argv[1]);
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
  std::string OutErr = "EValuation_log" + ot;
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

  // vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  vars.push_back("E9E25");
  // vars.push_back("E1");
  // vars.push_back("E9");
  // vars.push_back("E25");
  // vars.push_back("E1E9");

  // Use for asymm. init.
  std::map <std::string, size_t> numProtoMap;
  numProtoMap["electron"] = 40;
  numProtoMap["pion"]     = 80;
  
  // Learning parameters.
  float initC  = 0.8;// Only used with CCM protos
  float ethaZ  = 0.1;// Start learn param.
  float ethaF  = 0.0001;// Final learn param.

  //========= Normal training =====================
#if (CROSS_VALIDATE == 0)
  std::cout << "<-I-> Init classifier using data from root Trees.\n";

  // Create trainer object.
  PndLVQTrain tr(ip, labels, vars, true);

  tr.SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
  
  // Symm. initialization.
  tr.SetNumberOfProto(numProto);

  // Asymm. init.
  // tr.SetNumberOfProto(numProtoMap);
  
  // Set the size of the test set in (%)
  tr.SetTestSetSize(10);
  
  tr.SetErrorStepSize(10000);//1000 (DEFALUT)
  
  // Set per Epoch evaluation.
  tr.SetPerEpochEval(true);

  //VARX, MINMAX, MEDIAN, NONORM(DEFAULT)
  // tr.NormalizeData(VARX);
  
  // Perform PCA on input events.
  // tr.PCATransForm();
  
  // Specify the file containing pre-initialized prototypes.
  // tr.SetInitProtoFileName("InitialProto.root");
  
  // FILE_PR, KMEANS_PR, CCM_PR, RAND_FROM_DATA (DEFAULT)
  tr.setProtoInitType(RAND_FROM_DATA);
  
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
    
#if (DEBUGVQ_TRAIN_EXAMPLE > 0)
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
  
#else //__________ CROSS_VALIDATE != 0 (Do Cross-Validate) _______
  /*
   * We want to do cross-validation. So first create k-disjoint subsets
   * of the data point indices and use this to set the test set for
   * each of k-classifiers.
   */
  PndMvaDataSet* dat = new PndMvaDataSet(ip, labels, vars, TRAIN);

  // Trim data set.
  dat->SetTrim(true);
  
  // Normalize dataset (VARX, MINMAX, MEDIAN)
  // dat->SetNormType(VARX);
  
  // Perform PCA.
  // dat->Use_PCA(true);

  dat->Initialize();

  // Get available event data.
  std::vector< std::pair<std::string, std::vector<float>* > > const& evts = dat->GetData();
  std::cout << "<-In-> Total number of available events = "
	    << evts.size() << '\n';

  // Get list of labels and their attributes..
  std::vector<PndMvaClass> const& clsList = dat->GetClasses();

#if (DEBUGVQ_TRAIN_EXAMPLE > 0)
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

  // Determine how many events per set, per label
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

  //___ TEst sets are ready, Clean.
  nameIdx.clear();

  //========= DEBUG INFO.
#if (DEBUGVQ_TRAIN_EXAMPLE > 0)
  std::cout << "________ " << testSets.size()
	    << " == "      << NUMBER_OF_FOLDS
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

#if (DEBUGVQ_TRAIN_EXAMPLE > 1)
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
  if(disj)
  { std::cout << "All Subsets are Disjoint\n"; }
  else
  { std::cout << "NOT ALL subsets are Disjoint\n"; }
#endif// DEBUG

  // We need K classifiers.
  std::vector<PndLVQTrain*> trainerList;
  
  // Create trainer objects
  for(size_t i = 0; i < NUMBER_OF_FOLDS; ++i)
  {
    // Create trainers
    PndLVQTrain* t = new PndLVQTrain(evts, labels, vars, true);

    // Set learning parameters
    t->SetLearnPrameters(initC, ethaZ, ethaF, numSweep);
    
    // Symm. number of proto.
    t->SetNumberOfProto(numProto);

    // Use for asymm. init.
    //t->SetNumberOfProto(numProtoMap);
    
    // FILE_PR, KMEANS_PR, CCM_PR, RAND_FROM_DATA (DEFAULT)
    t->setProtoInitType(RAND_FROM_DATA);

    // Do NOT split test set.
    t->SetTestSetSize(0);

    // Assign test set indices.
    t->SetTestSet(testSets[i]);
    
    // Eval. rate.(DEFALUT = 1000) During CV we do just need the very
    // last evaluation. Note: If define anything else than zero here,
    // then you will need to adapt your computations.
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
   * NOTE: Number of started threads equals the number of available
   * cores. You can set the number of threads by function call or by
   * the shell variable.
   */
#if ( __GNUC__ >= 4 && __GNUC_MINOR__ >= 3)
  // GCC older that 4.4 and OpenMP produce a warning when size_t is
  // used; seems to be harmless.
#ifdef _OPENMP
#pragma omp parallel for schedule(dynamic, 1)
#endif// OPENMP
#endif//GCC
  for(size_t tr = 0; tr < trainerList.size(); ++tr)
  {
    // Perform training
    (trainerList[tr])->Train();

    // Write Weights.
#ifdef _OPENMP
#pragma omp critical (StoreProtoTypes)
#endif
    (trainerList[tr])->storeWeights();

    // Write Evaluation.
#ifdef _OPENMP
#pragma omp critical (StoreEvalData)
#endif
    (trainerList[tr])->WriteErroVect( (int2str(tr)+"_"+OutErr) );
  }// END FOR(tr)

  // ========= Fetch evaluation data for processing ====
  std::vector <StepError> EvalData;
  for(size_t i = 0; i < trainerList.size(); ++i)
  {
    // Get Eval vector.
    std::vector <StepError> const& err = (trainerList[i])->GetErrorValues();

    //! NOTE: Each classifier should return a single "StepError". In
    // other words a vector with size EQUAL to 1.
    assert (err.size() == 1);

    // Copy to container.
    for(size_t k = 0; k < err.size(); ++k)
    {
      EvalData.push_back(err[k]);
    }//err.size()
  }// Trainers loop

  // Print the evaluation results.
#if (DEBUGVQ_TRAIN_EXAMPLE > 1)
  for(size_t i = 0; i < trainerList.size(); ++i)
  {
    std::cout << "Trainer index = " << i << '\t';
    printErrors( *(trainerList[i]) );
  }
#endif// DEBUG
 
  // ====== Clean trainers list (we dont need them) =====
  for(size_t j = 0; j < trainerList.size(); ++j)
  {
    delete trainerList[j];
  }
  //_____________________________

  // Process Evaluation data
  float mean_tr, mean_ts;
  float sigma_tr, sigma_ts;
  mean_tr = mean_ts = sigma_tr = sigma_ts = 0.00;

  std::map <std::string, float> perClsMean_tr, perClsMean_ts;
  std::map <std::string, float> perClsSigm_tr, perClsSigm_ts;

  // Init Maps
  for(size_t i = 0; i < labels.size(); ++i)
  {
    std::string const& name = labels[i];
    
    perClsMean_tr[name] = 0.00;
    perClsMean_ts[name] = 0.00;

    perClsSigm_tr[name] = 0.00;
    perClsSigm_ts[name] = 0.00;
  }

  //assert (EvalData.size() == NUMBER_OF_FOLDS);

  // Loop through Eval Data points.
  for(size_t k = 0; k < EvalData.size(); ++k)
  {
    mean_tr += EvalData[k].m_trErr;
    mean_ts += EvalData[k].m_tsErr;
    std::map <std::string, float>& misTest  = EvalData[k].m_MisClsTest;
    std::map <std::string, float>& misTrain = EvalData[k].m_MisClsTrain;

    for(size_t i = 0; i < labels.size(); ++i)
    {
      perClsMean_ts [labels[i]] += misTest  [labels[i]];
      perClsMean_tr [labels[i]] += misTrain [labels[i]];
    }
  }
  // Compute mean_s (divide by (N))
  mean_tr /= static_cast<float>(EvalData.size());
  mean_ts /= static_cast<float>(EvalData.size());
  
  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    perClsMean_ts [labels[lb]] /= static_cast<float>(EvalData.size());
    perClsMean_tr [labels[lb]] /= static_cast<float>(EvalData.size());
  }
  
  // Compute variance
  for(size_t k = 0; k < EvalData.size(); ++k)
  {
    sigma_tr += (EvalData[k].m_trErr - mean_tr ) * (EvalData[k].m_trErr - mean_tr);
    sigma_ts += (EvalData[k].m_tsErr - mean_ts ) * (EvalData[k].m_tsErr - mean_ts);

    std::map <std::string, float>& misTest  = EvalData[k].m_MisClsTest;
    std::map <std::string, float>& misTrain = EvalData[k].m_MisClsTrain;

    for(size_t i = 0; i < labels.size(); ++i)
    {
      perClsSigm_ts[labels[i]] += (misTest[labels[i]] - perClsMean_ts [labels[i]]) * 
	(misTest[labels[i]] - perClsMean_ts [labels[i]]);

      perClsSigm_tr[labels[i]] += (misTrain[labels[i]] - perClsMean_tr [labels[i]]) *
	(misTrain[labels[i]] - perClsMean_tr [labels[i]]);
    }
  }
  // divide by (N-1)
  sigma_tr /= static_cast<float>(EvalData.size() - 1);
  sigma_ts /= static_cast<float>(EvalData.size() - 1);
  
  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    perClsSigm_ts [labels[lb]] /= static_cast<float>( EvalData.size() - 1);
    perClsSigm_tr [labels[lb]] /= static_cast<float>( EvalData.size() - 1);
  }

  // Print Overal mean, sigma and the same per label.
  std::cout << "\n=====================================\n"
	    << "# Cross_validation Evaluation Data.\n"
	    << "# "<< NUMBER_OF_FOLDS << "-fold.\n"
	    << " Number of proto = " << numProto
	    << " Num Sweep = " << numSweep
	    << "\n Mean_tr = " << mean_tr
	    << ", Sigma_tr = " << sigma_tr <<'\n'
	    << " Mean_ts = " << mean_ts
	    << ", Sigma_ts = " << sigma_ts <<'\n';

  std::cout << "\nPer label information.\n\n";
  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    std::cout << " Mean test "    << labels[lb] << " = " << perClsMean_ts[labels[lb]]
	      << ", Sigm test "   << labels[lb] << " = " << perClsSigm_ts [labels[lb]]
      	      << "\n Mean train " << labels[lb] << " = " << perClsMean_tr[labels[lb]]
	      << ", sigm train "  << labels[lb] << " = " << perClsSigm_tr [labels[lb]]
	      <<'\n';
  }
  //+++++++++++++
  std::cout << "\n# Num Sweep = " << numSweep
	    << "\n#+++++++++++++++++++++++++++++++++++++++++++++++\n"
	    << numProto << " "
	    << mean_tr  << " "
	    << sigma_tr << " "
	    << mean_ts  << " "
	    << sigma_ts << " ";
  
  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    std::cout << perClsMean_ts[labels[lb]]  << " "
	      << perClsSigm_ts [labels[lb]] << " "
      	      << perClsMean_tr[labels[lb]]  << " "
	      << perClsSigm_tr [labels[lb]] << " ";
  }
  std::cout << "\n#+++++++++++++++++++++++++++++++++++++++++++++++\n";
  //+++++++++++++++
  std::cout << "\n=====================================\n";
#endif// CROSS_VALIDATE != 0
  return 0;
}
