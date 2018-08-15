/* ***************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */

#include "PndLVQTrain.h"

/**
 * Constructor:
 * @param InputEvtsParam Input events vector.
 * @param ClassNames class names.
 * @param VarNames variable names of the features.
 */
PndLVQTrain::PndLVQTrain(std::vector< std::pair<std::string, std::vector<float>*> > const& Input,
			 std::vector<std::string> const& ClassNames, 
			 std::vector<std::string> const& VarNames,
			 bool trim)
  : PndMvaTrainer(Input, ClassNames, VarNames, trim),
    m_initConst(0.8),
    m_ethaZero(0.1),
    m_ethaFinal(0.01),
    m_WindowSize(0.3),
    m_NumSweep(10),
    m_proto_init(RAND_FROM_DATA),
    m_initProtoFile(""),
    m_ErrorStep(1000),
    m_ProgStep(1000),
    m_PerEpoch(false)
{}

/**
 * Constructor:
 * @param InPut: Input file name.
 * @param ClassNames: class names.
 * @param VarNames: variable names of the features.
 */
PndLVQTrain::PndLVQTrain(std::string const& inputFile,
			 std::vector<std::string> const& ClassNames, 
			 std::vector<std::string> const& VarNames,
			 bool trim)
  : PndMvaTrainer(inputFile, ClassNames, VarNames, trim),
    m_initConst(0.8),
    m_ethaZero(0.1),
    m_ethaFinal(0.01),
    m_WindowSize(0.3),
    m_NumSweep(10),
    m_proto_init(RAND_FROM_DATA),
    m_initProtoFile(""),
    m_ErrorStep(1000),
    m_ProgStep(1000),
    m_PerEpoch(false)
{}

/**
 * Destructor
 */
PndLVQTrain::~PndLVQTrain()
{
  std::cout << "<INFO> Cleaning all initialized objects, LVQ.\n"; 

  // Clean m_LVQProtos
  for(size_t i = 0; i < m_LVQProtos.size(); i++)
  {
    delete m_LVQProtos[i].second;
  }
  m_LVQProtos.clear();

  // Clean m_distances
  m_distances.clear();

  m_numProtoPerClass.clear();
}

/**
 * Train the classifier accourding to LVQ1 algorithm.
 */
void PndLVQTrain::Train()
{
  TRandom3 trand(m_RND_seed);
  
  // Init Proto types
  InitProtoTypes();
  
  // Fetch available event examples
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();
  
  std::set <size_t>::const_iterator testSetIter;
  
  // Compute learning rate constant "a"
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.0001;
  unsigned int numSweep  = m_NumSweep;//1000;
  unsigned int tFinal = numSweep * ( events.size() );
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal) );
  
  m_ProgStep = (tFinal / 100);
  
  std::cout << "<INFO> Each . equals " << m_ProgStep 
	    << " learning steps and\n\teach +/- means one re-initialization.\n";

  // We need to solve or catch this
  if( a <= 0.00 )
  {//Underflow
    std::cerr << "<ERROR> Too small value for parameter a .\n"
	      << "\tWe will proceed using std::numeric_limits<long double>::min()."
	      << "\n\n\n\n"
	      << std::endl;
    a = std::numeric_limits<long double>::min();
  }
  
  if( (tFinal + static_cast<unsigned int>(1)) <= tFinal )
  {// OverFlow
    std::cerr << "<ERROR> tFinal Overflow.\n"
	      << "\tWe will proceed using std::numeric_limits<unsigned int>::max()."
	      << "\n\n\n\n"
	      << std::endl;
    tFinal = std::numeric_limits<unsigned int>::max();
  }
  // We need to solve or catch this

  // Print some information.
  std::cout << "<INFO>: Performing LVQ1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst <<", ethaZero = " 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= " << tFinal 
	    <<", learn coeff. = " << a << '\n';
  
  // Start the training
  std::cout << "Starting to train (LVQ1) .....\n";
  
  for(unsigned int time = 0; time < tFinal; time++)
  {
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
    // ethaT can become very small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min()))
    {
      ethaT  = std::numeric_limits<double>::min();
      std::cerr <<"\tVery small ethaT" << std::endl;
    }

    // Show progress.
    if( (time % m_ProgStep) == 0)
    {
      std::cerr << ". " ;
    }

    // Evaluate classifier per specified number of steps.
    if( (!m_PerEpoch) &&
        (m_ErrorStep != 0) &&
        ( (time % m_ErrorStep) == 0 )
      )
    {
      EvalClassifierError(time);
    }
    else if( (m_PerEpoch) && // Evaluate classifier per Epoch.
             ( (time % events.size()) == 0 )
           )
    {
      // EvalClassifierError(time);
      EvalClassifierError( (time / events.size()) );
    }
    // select a random example
    // int index = static_cast<int>(trand.Uniform(0.0, (events.size() - 1) ) );
    int index = static_cast<int>( (trand.Uniform(0.0, (events.size() - 1)) ) + 0.5);

    testSetIter = m_testSet_indices.find(index);

    // Selected event NOT in the test set. If the iterator ==
    // m_testSet_indices.end() the the selected index is not a member
    // of the test set.
    while( testSetIter != m_testSet_indices.end())
    {
      // index = static_cast<int>( trand.Uniform(0.0, (events.size() - 1) ));
      index = static_cast<int>( (trand.Uniform(0.0, (events.size() - 1) ) ) + 0.5);
      testSetIter = m_testSet_indices.find(index);
    }
    
    size_t protoIndex      = 0;
    float distance         = 0.0;
    float minProtoDistance = std::numeric_limits<float>::max();

    // Compute the distance to all available LVQ proto-types
    for(size_t ix = 0; ix < m_LVQProtos.size(); ix++)
    {
      distance = ComputeDist( *(events[index].second), *(m_LVQProtos[ix].second) );
      
      if(distance < minProtoDistance)
      {
	minProtoDistance = distance;//minimum distance
	protoIndex  = ix;//index of the prototype with min dist
      }
    }
    
    // We need to update the (winner) prototype
    int delta = 0;
    
    // determine delta
    if( events[index].first == m_LVQProtos[protoIndex].first )
    {// Equal labels
      delta = 1;
    }
    else
    {// Diff. Labels
      delta = -1;
    }// delta is calculated
    
    // Update the LVQ prototype
    UpdateProto( *(events[index].second), *(m_LVQProtos[protoIndex].second), delta, ethaT);
  }

  // Last evaluation after the very last learning step.
  if( m_PerEpoch ) {
    EvalClassifierError( (tFinal / events.size()) );
  } else {
    EvalClassifierError( tFinal );
  }

  std::cerr << '\n';
  std::cout << "<INFO> Finished training.\n";
  //WriteToWeightFile(m_LVQProtos);
}

/**
 * Train the classifier accourding to LVQ2.1 algorithm.
 */
void PndLVQTrain::Train21()
{
  TRandom3 trand(m_RND_seed);

  // Init proto types
  InitProtoTypes();
  
  // Initialize distance container.
  if(m_distances.size() == 0)
  {
    std::cout << "<INFO> Init Distances Container.\n";
    for(size_t i = 0; i < m_LVQProtos.size(); ++i)
    {
      m_distances.push_back(PndMvaDistObj());
    }
  }

  // Fetch available event examples.
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();

  // Test set iterator
  std::set <size_t>::const_iterator testSetIter;

  // Compute learning rate constant "a"
  float windowSize = m_WindowSize;// A value between 0.2 & 0.3 is recommended.
  float s = (1 - windowSize)/(1 + windowSize);//Define the surrounding.
  
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.001;
  unsigned int numSweep  = m_NumSweep;//100;
  unsigned int tFinal = numSweep * ( events.size());
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal));
  
  m_ProgStep = (tFinal / 100);

  std::cout << "<INFO> Each . equals " << m_ProgStep 
	    << " learning steps and\n\teach +/- means one re-initialization.\n";

  // We need to solve or catch this
  if( a <= 0.00 )
  {//Underflow
    std::cerr << "<ERROR> Too small value for parameter a .\n"
	      << "\tWe will proceed using std::numeric_limits<long double>::min()."
	      << "\n\n\n\n"
	      << std::endl;
    a = std::numeric_limits<long double>::min();
  }
  
  if( (tFinal + static_cast<unsigned int>(1)) <= tFinal )
  {// OverFlow
    std::cerr << "<ERROR> tFinal Overflow.\n"
	      << "\tWe will proceed using std::numeric_limits<unsigned int>::max()."
	      << "\n\n\n\n"
	      << std::endl;
    tFinal = std::numeric_limits<unsigned int>::max();
  }
  // We need to solve or catch this

  // Print some INFO.
  std::cout << "<INFO>: Performing LVQ2.1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst << ", ethaZero = " 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= "<< tFinal 
	    <<", learn coeff. = " << a << ", Window = " << windowSize 
	    <<", surroun. = "<< s << '\n';
  
  // Start learning
  std::cout << "Starting to train (LVQ2.1) .....\n";
  for(unsigned int time = 0; time < tFinal; time++)
  {
    // Show progress.
    if( (time % m_ProgStep) == 0)
    {
      std::cerr << " ." ;
    }
    
    // Evaluate classifier per specified number of steps.
    if( ( (!m_PerEpoch) && 
          (m_ErrorStep != 0) &&
	  (time % m_ErrorStep) == 0
        )
      )
    {
      EvalClassifierError(time);
    }
    else if( (m_PerEpoch) && // Evaluate classifier per Epoch.
             ( (time % events.size()) == 0 )
           )
    {
      //EvalClassifierError(time);
      EvalClassifierError( (time / events.size()) );
    }

    double distance = 0.0;
    double ethaT    = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
    // ethaT can become very small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min()))
    {
      ethaT  = std::numeric_limits<double>::min();
      std::cerr <<"Very small ethaT" << std::endl;
    }
    
    // select a random example
    int index = static_cast<int>(trand.Uniform( 0, events.size() - 1));
    
    testSetIter = m_testSet_indices.find(index);

    // Selected event NOT in the test set
    while( testSetIter != m_testSet_indices.end())
    {
      index = static_cast<int>(trand.Uniform(0.0, events.size() - 1));
      testSetIter = m_testSet_indices.find(index);
    }

    // Compute the distance to all available LVQ proto-types
    for(size_t ix = 0; ix < m_LVQProtos.size(); ix++)
    {
      distance = ComputeDist( *(events[index].second), *(m_LVQProtos[ix].second) );
      
      // Store distance.
      (m_distances[ix]).m_idx  = ix;
      (m_distances[ix]).m_dist = distance;
      (m_distances[ix]).m_cls  = m_LVQProtos[ix].first;
    }// All distances are determined.
    
    // Sort the distances.
    std::sort( m_distances.begin(), m_distances.end() );
    
    /*
     * We need to Select the two nearest codebooks and update
     * them. Per definition the first one has index zero (0), thus we
     * need to find the second one.
     */
    int idxSame = 0; int idx2d = 0;
    if( events[index].first == (m_distances[idxSame]).m_cls )
    {//Equal labels
      idx2d = 1;
    }
    else
    {//Diff. labels
      idxSame = 1;
    }
    if(idxSame == 0)
    {//Find one with a diff. label
      while(events[index].first == (m_distances[idx2d]).m_cls)
      {
	idx2d++;
      }
    }
    else
    {//Find one with the same label.
      while(events[index].first != (m_distances[idxSame]).m_cls)
      {
	idxSame++;
      }
    }

    // Found two prototypes, one with the same lablel and one with a diff. one
    if( minFunct( (m_distances[idxSame]).m_dist / (m_distances[idx2d]).m_dist ,
		  (m_distances[idx2d]).m_dist   / (m_distances[idxSame]).m_dist ) > s )
    {
      // Update the LVQ prototype
      int deltaEqCls = 1; int deltaNonEqCls = -1;      

      // Update equal label prototype.
      UpdateProto( *(events[index].second), *(m_LVQProtos[idxSame].second), deltaEqCls, ethaT);
      
      // Update different label prototype.
      UpdateProto( *(events[index].second), *(m_LVQProtos[idx2d].second), deltaNonEqCls, ethaT);
    }
  }

  // Last evaluation
  if( m_PerEpoch ) {
    EvalClassifierError( (tFinal / events.size()) );
  }
  else {
    EvalClassifierError( tFinal );
  }

  std::cerr << std::endl;
  std::cout << "<INFO> Finished training.\n";
  //WriteToWeightFile(m_LVQProtos);
}

// ==================== Private functions =======================

/**
 * Initialize LVQ prototypes (Code books).
 */
void PndLVQTrain::InitProtoTypes()
{
  // number of proto = 0 makes No sence.
  bool nonZeroProto = true;
  for(std::map<std::string, size_t>::const_iterator iter = m_numProtoPerClass.begin(); 
      iter != m_numProtoPerClass.end(); iter++)
  {
    nonZeroProto = nonZeroProto && (iter->second != 0);
  }
  if( !nonZeroProto )
  {
    std::cerr << "<ERROR> Undefined number of prototypes for one or more classes."
	      << std::endl;
    assert(nonZeroProto);
  }
  
  // Number of proto larger dan number of events!
  // Fetch labels.
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  for(size_t i = 0; i < classes.size(); i++)
  {
    if( classes[i].NExamples < m_numProtoPerClass[(classes[i]).Name] )
    {
      std::cerr << "<ERROR> Requested number of prototypes larger than "
		<< "the number of available examples for class: "
		<< (classes[i]).Name << '\n' 
		<< std::endl;
      exit(EXIT_FAILURE);
    }
  }
  
  // Clear protypes list
  cleanProtoList();
  
  // Select initialization method & initialize.
  switch(m_proto_init)
  {
  case FILE_PR:// Read from file
    if(m_initProtoFile == "")
    {
      std::cerr << "<ERROR> Empty file name.\n"
		<<"\tYou need to specify the file "
		<<"containing the initial code books." 
		<< std::endl;
      assert(m_initProtoFile != "");
    }
    // Read Code Books (protoTypes) from file
    ReadProtoFromFile();
    break;
    
  case KMEANS_PR:// Kmeans_clustering
    InitProtoK_Means();
    break;
    
  case CCM_PR:
    //Initialize using CCM.
    InitProtoRand();
    break;
    
  case RAND_FROM_DATA:
  default:
    //Select random examples from data.
    InitRandProtoFromData();
    break;
  }
}

/**
 * Initialize LVQ prototypes (Code books) using K-Means clustering.
 */
void PndLVQTrain::InitProtoK_Means()
{
  std::cout << "<INFO> Initializing LVQ prototypes using K_Means clustering.\n";

  // Fetch labels.
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();

  // Print number of proto for each class.
  std::cout << "<INFO> Number of protoTypes per class:\n\t";
  for(size_t i = 0; i < classes.size(); i++)
  {
    std::cout << classes[i].Name << " = "
	      << m_numProtoPerClass[classes[i].Name]
	      << ", ";
  }
  std::cout << '\n';

  // Get available data points.
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();
  
  // Init temporary container. Store clustering output.
  std::map<std::string, DataPoints*> ProtoVector;//(classes.size());
  
  //======== Class loop
  int cls = 0;
  int numberOfClasses = static_cast<int> (classes.size());
  
#ifdef _OPENMP
#pragma omp parallel for  schedule(dynamic)
#endif  
  for( cls = 0; cls < numberOfClasses; cls++)
  {
    DataPoints clusteringInput;
    std::string clsName = (classes[cls]).Name;
    size_t numProto = m_numProtoPerClass[clsName];

    // Example loop
    for(size_t evt = 0; evt < events.size(); evt++)
    {
      if(events[evt].first == clsName)
      {
	clusteringInput.push_back(events[evt]);
      }
    }// ExampleLoop
    
    // We have seen all available examples for the current class
    // (lable).
    std::cout << "Number of examples for " << clsName 
	      << " = " <<  clusteringInput.size()
	      << '\n';
    
    // Create clusters from current data points.
    PndMvaCluster clust (clusteringInput, numProto);
    DataPoints* clustOut = clust.Cluster();

#ifdef _OPENMP
#pragma omp critical (AddToProtoListMap)
    {
#endif
      // Store computed means in the prototypes container.
      ProtoVector[clsName] = clustOut;
#ifdef _OPENMP
    }
#endif
    
  }// END ClassLoop
  
  // Copy cluster centers (CMs) to LVQ prototypes (code books)
  for(size_t i = 0 ; i < classes.size(); i++)
  {
    // Select current label
    std::string label = classes[i].Name;

    // Select centroids for the current label
    DataPoints* tdat = ProtoVector[label];

    // Copy coordinates to prototypes vector.
    for(size_t j = 0; j < tdat->size(); ++j)
    {
      std::pair<std::string, std::vector<float>*>& TMP = tdat->at(j);
      std::vector<float>* lvpr = new std::vector<float>( *(TMP.second) );
      m_LVQProtos.push_back(std::make_pair(label, lvpr));
    }
  }// End of classes loop
  
  // We are done. Clean-up
  for(size_t i = 0 ; i < classes.size(); i++)
  {
    std::string label = classes[i].Name;

    DataPoints* tdat = ProtoVector[label];
    
    for(size_t j = 0; j < tdat->size(); ++j)
    {
      std::pair<std::string, std::vector<float>*>& TMP = tdat->at(j);
      delete TMP.second;
    }
    tdat->clear();
    delete ProtoVector[label];
  }// END of cleaning
  ProtoVector.clear();
}

/**
 * Initialize LVQ prototypes (Code books) using class conditional
 * means (CCM) vectors.
 */
void PndLVQTrain::InitProtoRand()
{
  std::cout << "<INFO> Initializing LVQ prototypes based on CCM.\n";

  // Initialize LVQ-prototypes.
  double c = m_initConst;//0.8;
  TRandom3 trand(m_RND_seed);

  // Fetch labels.
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();

  // Print number of proto for each class.
  std::cout << "<INFO> Number of protoTypes per class:\n\t";
  for(size_t i = 0; i < classes.size(); i++)
  {
    std::cout << classes[i].Name << " = "
	      << m_numProtoPerClass[classes[i].Name]
	      << ", ";
  }
  std::cout << '\n';

  // Fetch variables.
  std::vector<PndMvaVariable> const& variables = m_dataSets.GetVars();
  
  // Fetch examples.
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();
  
  // Fetch class conditional means.
  std::map< std::string, std::vector<float>* > const& ClsCondMeans = m_dataSets.GetClassCondMeans();

  for(size_t cl = 0; cl < classes.size(); cl++)
  {
    int minIdx = classes[cl].StartIdx;
    int maxIdx = classes[cl].EndIdx;
    std::string curClsName = classes[cl].Name;
    size_t numProto = m_numProtoPerClass[curClsName];
    
    for(size_t i = 0; i < numProto; i++)
    {
      // select a random example
      if(minIdx == 0)
      {
	minIdx = 1;
      }
      int index = static_cast<int>(trand.Uniform(minIdx, maxIdx));
      
      if(index < minIdx)
      {
	index += minIdx - index;
      }
      if(index > maxIdx)
      {
	std::cerr << "\n\n======================================\n"
		  << "\t<ERROR> Index out of bound "
		  << index <<" and cls = "<< cl
		  << "\n=========================================\n"
		  << std::endl;
	assert(!(index > maxIdx));
      }
      
      // We have found a random event.
      std::map< std::string, std::vector<float>* >::const_iterator it = ClsCondMeans.find(curClsName);
      std::vector<float>* clsmean = it->second;
      
      // Init protoType vector
      std::vector<float>* proto   = new std::vector<float>(variables.size(), 0.0);
      std::vector<float>* evtData = events[index].second;
      
      // May not happen, DEBUG DEBUG DEBUG
      if(curClsName != events[index].first)
      {
	std::cerr << "\n Name collision cls is " << classes[cl].Name
		  << " index is " << index << " minindex " << minIdx
		  <<" current class name is " << events[index].first 
		  << std::endl;
	assert(curClsName == events[index].first);
      }
      
      for(size_t k = 0; k < evtData->size(); k++)
      {
	proto->at(k) = (clsmean->at(k) * c) + ( (1.0 - c) * evtData->at(k) );
      }
      // Add ProtoType to the container
      m_LVQProtos.push_back(std::make_pair(events[index].first, proto));
    }//END for(i = 0) 
  }// END for(cl = 0)
}

/**
 * Initialize LVQ prototypes (Code books) using Randomly selected
 * vectors from the original data set.
 */
void PndLVQTrain::InitRandProtoFromData()
{
  std::cout << "<INFO> Initializing LVQ prototypes"
	    << " using random data vectors.\n";

  TRandom3 trand(m_RND_seed);

  // Fetch labels.
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();
  
  // Fetch variables.
  std::vector<PndMvaVariable> const& variables = m_dataSets.GetVars();
  
  // Fetch examples.
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();

  // Print number of proto for each class.
  std::cout << "<INFO> Number of protoTypes per class:\n\t";
  for(size_t i = 0; i < classes.size(); i++)
  {
    std::cout << classes[i].Name << " = "
	      << m_numProtoPerClass[classes[i].Name]
	      << ", ";
  }
  std::cout << '\n';
  // Loop (labels)
  for(size_t cl = 0; cl < classes.size(); cl++)
  {
    int minIdx = classes[cl].StartIdx;
    int maxIdx = classes[cl].EndIdx;
    std::string curClsName = classes[cl].Name;
    size_t numProto = m_numProtoPerClass[curClsName];
    
    for(size_t i = 0; i < numProto; ++i)
    {
      // Select a random example.
      int index = static_cast<int>(trand.Uniform(minIdx, maxIdx));

      // Init protoType vector
      std::vector<float>* proto   = new std::vector<float>(variables.size(), 0.0);
      std::vector<float>* evtData = events[index].second;

      // Copy values and add to the container.
      for(size_t k = 0; k < evtData->size(); k++)
      {
	proto->at(k) = evtData->at(k);
      }
      
      // Add ProtoType to the container
      m_LVQProtos.push_back(std::make_pair(events[index].first, proto));
    }// Num proto
  }//Labels
}

/**
 * Clean prototype container.
 */
void PndLVQTrain::cleanProtoList()
{
  std::cout << "<INFO> Cleaning the prototype list.\n"; 

  // Clean up the container for proto-types
  for(size_t k = 0; k < m_LVQProtos.size(); k++)
  {
    delete m_LVQProtos[k].second;
  }
  m_LVQProtos.clear();
}

/**
 * Updates the LVQ prototypes.
 */
void PndLVQTrain::UpdateProto(std::vector<float> const& EvtData, 
			      std::vector<float>& proto, 
			      int const delta, double const ethaT)
{
  for(size_t i = 0; i < proto.size(); i++)
  {
    proto[i] = proto[i] + ( ethaT * static_cast<double>(delta) * (EvtData[i] - proto[i]) );
  }

  // Validate
  ValidateProtoUpdate(proto);
}

/**
 * Check if the current update creates an invalid codebook (If the
 * vectore is placed outside the extrema). If after the update the 
 * codebook is out of boundary: reinitialize. Else: do nothing.
 *@param p The to be validated prototype.
 */
void PndLVQTrain::ValidateProtoUpdate(std::vector<float>& p)
{
  // Either reinit or not.
  bool reinit = false;

  // Fetch available variables
  std::vector<PndMvaVariable> const& variables = m_dataSets.GetVars();

  /*
   * Optimization NOTE: This can be more efficint by using a while
   * statement; but as we have only a small number of parameters ( 3
   * or 4), it is good enough to use a for statement.
   */
  for(size_t var = 0; var < variables.size(); ++var)
  {
    // If we need to reinitialize
    if( ( p[var] < variables[var].Min) || ( p[var] > variables[var].Max ) )
    {
      reinit = true;
    }
  }

  /*
   * Optimization NOTE: It is more efficent to remember the indices of
   * variables which are out of bound. In that case the number of
   * calls to get a random number will decrease. But for now, it is
   * good enough. :).
   */
  // We need to reinitialize the codebook.  For now we init using
  // random numbers in the range of var.Min and ar.Max
  if(reinit)
  {
    TRandom3 rnd(m_RND_seed);
    for(size_t idx = 0; idx < variables.size(); ++idx)
    {
      p[idx] = static_cast<float>(rnd.Uniform(variables[idx].Min, variables[idx].Max));
    }
    // Indicate a re-init on std error.
#if (DEBUG_LVQ_TRAIN > 0)
    std::cerr << '^';
#endif
  }
}

/**
 * Evaluate the train and test error for the current classifier.
 */
void PndLVQTrain::EvalClassifierError(unsigned int stp)
{
  // Structure to hold the number of mis-classified events per label.
  std::map <std::string, float> perLabelMisClsTest;
  std::map <std::string, float> perLabelMisClsTrain;
  
  // Fetch labels.
  std::vector<PndMvaClass> const& labels = m_dataSets.GetClasses();
  // Init containers.
  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    perLabelMisClsTest [labels[lb].Name] = 0.00;
    perLabelMisClsTrain[labels[lb].Name] = 0.00;
  }

  // Get Examples
  std::vector<std::pair<std::string, std::vector<float>*> > const& events = m_dataSets.GetData();
  
  // Test-set iterator.
  std::set <size_t>::const_iterator iter;

  size_t TrError = 0;// Train error
  size_t TsError = 0;// Test  error

  //========== Classify Test Set
  for(iter = m_testSet_indices.begin(); iter != m_testSet_indices.end(); ++iter)
  {
    std::string WinClassName;
    float dist    = 0.0; // Current distance
    float minDist = std::numeric_limits<float>::max();// Winner dist
    size_t idx = *iter;

    std::vector<float>* EvtVect = (events.at(idx)).second;

    for(size_t i = 0; i < m_LVQProtos.size(); i++)
    {
      dist = ComputeDist(*EvtVect, *(m_LVQProtos[i].second));

      if(dist < minDist)
      { 
	minDist = dist;
	WinClassName = m_LVQProtos[i].first;
      }
    }
    if(WinClassName != (events.at(idx)).first)
    {// Wrong (Labels are not equal), misclassified
      TsError++;
      perLabelMisClsTest[(events.at(idx)).first] += 1.00;
    }
  }

  //========== Classify train set
  // Event loop
  for(size_t evt = 0; evt < events.size(); evt++)
  {
    std::string WinClassName;
    float dist    = 0.0; // Current distance
    float minDist = std::numeric_limits<float>::max();// Winner dist

    if(m_testSet_indices.find(evt) == m_testSet_indices.end())
    {// Event not in the Test Set

      std::vector<float>* EvtVect = events[evt].second;

      // Proto loop
      for(size_t k = 0; k < m_LVQProtos.size(); k++)
      {
	dist = ComputeDist(*EvtVect, *(m_LVQProtos[k].second));
	
	if(dist < minDist)
	{
	  minDist = dist;
	  WinClassName = m_LVQProtos[k].first;
	}
      }
      if(WinClassName != events[evt].first)
      {// Wrong (misclassified, labels are not equal).
	TrError++;
	perLabelMisClsTrain[events[evt].first] += 1.00;
      }
    }
  }

  // Number of events in each subset (test and train)
  size_t NumTsTrEvt = m_testSet_indices.size();
  NumTsTrEvt = (NumTsTrEvt > 0)? NumTsTrEvt : -1;

  // Test error in %.
  float tsEr = (TsError * 100.00) / static_cast<float>(NumTsTrEvt);
 
  // Number of train examples
  NumTsTrEvt = ( events.size() - m_testSet_indices.size() );
  NumTsTrEvt = (NumTsTrEvt > 0)? NumTsTrEvt : -1;

  // Train error in %.
  float trEr = (TrError * 100.00) / static_cast<float>(NumTsTrEvt);

  // Train and test per class micls in %
  for(size_t lb = 0; lb < labels.size(); ++lb)
  {
    // Current label
    std::string const& lbn = labels[lb].Name;
    // Test set error
    perLabelMisClsTest[lbn] = (perLabelMisClsTest[lbn] * 100.00)/
      static_cast<float>(labels[lb].NExamples - labels[lb].NTrainEx);

    // Train set error
    perLabelMisClsTrain[lbn] = (perLabelMisClsTrain[lbn] * 100.00)/
      static_cast<float>(labels[lb].NTrainEx);

#if DEBUG_LVQ_TRAIN > 0
    std::cout << "\nLabel is " << lbn
	      << "\n perLabelMisClsTest[" << lbn << "] = " << perLabelMisClsTest [lbn]
	      << "\n perLabelMisClsTrain["<< lbn << "] = " << perLabelMisClsTrain[lbn]
	      << '\n';
#endif // DEBUG DEBUG
  }
  // Create object and Add to the container.
  StepError StpEr (stp, trEr, tsEr, perLabelMisClsTest, perLabelMisClsTrain);
  m_StepErro.push_back(StpEr);
  
  // Reset counters.
  //TrError = TsError = 0;
}

/**
 * Read pre-initialized code books from file and store the vectors
 * in LVQ prototype container.
 */
void PndLVQTrain::ReadProtoFromFile()
{
  // Fetch the available labels.
  std::vector<PndMvaClass> const& classes = m_dataSets.GetClasses();

  // Get parameters.
  std::vector<PndMvaVariable> const& variables = m_dataSets.GetVars();
  
  std::cout << "<INFO> Reading data from  "<< m_initProtoFile
	    << '\n';
  
  // Open the input file for reading event data.
  TFile InPutFile(m_initProtoFile.c_str(), "READ");
  
  // Fetch the class trees and read the event data.
  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    // Tree name
    char const* name = classes[cls].Name.c_str();
    std::cout << "<INFO> Reading events for "
	      <<  classes[cls].Name << '\n';
    
    // Get the tree object
    TTree *t = (TTree*) InPutFile.Get(name);
    if(!t)
    {
      std::cerr<< "\t<ERROR> Could not find data tree " << name 
	       << std::endl;
      assert (t);
    }
    std::cout << "<INFO> There are "<< t->GetEntriesFast()
	      << " vectors available for the current class."
	      << '\n';
    
    if( static_cast<size_t>(t->GetEntriesFast()) != m_numProtoPerClass[classes[cls].Name])
    {
      std::cerr << "<ERROR> Number of prototypes and the"
		<<" number of available examples do not match."
		<< std::endl;
      assert(static_cast<size_t>(t->GetEntriesFast()) == m_numProtoPerClass[classes[cls].Name]);
    }
    // Init a container to bind to the tree branches
    std::vector<float> ev (variables.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < variables.size(); j++)
    {
      char const* branchName = variables[j].Name.c_str();
      //Binding the branches
      t->SetBranchAddress(branchName, &(ev[j]));
    }// Tree parameters are bounded
    
    // Fetch and store the variables to per class variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++)
    {
      t->GetEntry(k);
      
      // Container to store the vent data read from the input tree
      std::vector<float>* eventData = new std::vector<float>();
      
      for(size_t idx = 0; idx < variables.size(); idx++)
      {
        eventData->push_back(ev[idx]);
      }
      
      // Store the event and its class name
      m_LVQProtos.push_back(make_pair(classes[cls].Name, eventData));
    }
    
    // We are done and can delete the tree pointer
    delete t;
  }// End of for(cls) loop for all classes
}

/**
 * Set the number of protoTypes to be used for training.
 *@param numProto  Number of prototypes.
 */  
void PndLVQTrain::SetNumberOfProto(size_t const numProto)
{
  // Fetch labels.
  const std::vector < PndMvaClass >& classes = m_dataSets.GetClasses();
  for(size_t cl = 0; cl < classes.size(); cl++)
  {
    m_numProtoPerClass[ classes[cl].Name ] = numProto;
  }
}

/**
 * Set the number of protoTypes to be used for training.
 *@param labelMap  Map containing number of prototypes 
 * for each class (label).
 */  
void PndLVQTrain::SetNumberOfProto(std::map<std::string, size_t> const& labelMap)
{
  // Fetch labels.
  std::vector < PndMvaClass > const& classes = m_dataSets.GetClasses();
  // Init map iterator.
  std::map < std::string, size_t>::const_iterator iter;
  for(size_t cl = 0; cl < classes.size(); cl++)
  {
    std::string curLabel = classes[cl].Name;
    // Check if the current is specified.
    iter = labelMap.find(curLabel);
    // Num proto defined.
    if( iter != labelMap.end() )
    {
      m_numProtoPerClass [curLabel] = iter->second;
    }
    else{// Num Proto not defined.
      m_numProtoPerClass [curLabel] = 0;
      std::cerr << "<ERROR> Number of prototypes for curLabel was not defined."
		<< "        Program is halted."
		<< std::endl;
      exit(EXIT_FAILURE);      
    }
  }
}

/**
 * Classifier evaluation.
 */
void PndLVQTrain::EvalClassifierError()
{
  std::cout << "\t<-I-> Evaluating LVQ.\n";
  EvalClassifierError(std::numeric_limits<unsigned int>::max());
}

/**
 * Store weights in the output File. If output file name is not
 * specified, then write nothing.
 */
void PndLVQTrain::storeWeights()
{
  WriteToWeightFile(m_LVQProtos);
}

/// ======== DEBUG FUNCTIONS
#if DEBUG_LVQ_TRAIN > 0
/**
 * Evaluate the classifier, using the given test events.
 *@param TestEvts The set which is used as test set.
 * Note: tstEx is not normalized or modified. If it is needed, the
 * user has to perform modifications before invoking this method.
 */
float PndLVQTrain::EvalClassifierError( std::vector< std::pair< std::string, std::vector<float>* > > const& TestEvts) const
{ 
  //Classifier error
  float error = 0.0;
  // Distance between curren event and prototype
  float dist = 0.0;
  // Winner dist
  float minDist = std::numeric_limits<float>::max();
  // Winner label
  std::string WinClassName;
  
  // Events loop
  for(size_t ev = 0; ev < TestEvts.size(); ++ev)
  {
    // Current event.
    std::vector<float> const* teEvt = (TestEvts.at(ev)).second;
    
    // Reset minDistance
    minDist = std::numeric_limits<float>::max();
    
    // Prototype loop
    for(size_t pr = 0; pr < m_LVQProtos.size(); ++pr)
    {
      // Current prototype
      std::vector<float> const* prot = m_LVQProtos[pr].second;
      
      // compute distance
      dist = ComputeDist( *teEvt, *prot );
      
      if(dist < minDist)
      { 
	minDist = dist;
	WinClassName = m_LVQProtos[pr].first;
      }
    }// Proto loop
    
    // If the classification was corect
    if( WinClassName != (TestEvts.at(ev)).first )
    {// Wrong (Labels are not equal), misclassified
      error++;
    }
  }// Events Loop
  
  error = (error * 100.00) / static_cast<float>(TestEvts.size());
  return error;
}
#endif// DEBUG FUNCTION
