/* ***************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * Version 0.1 beta1.                    *
 * LICENSE:                              *
 * ***************************************
 */
#define ProgStep  1000
#define ErrorStep 10

#include "PndLVQTrain.h"

/**
 * Constructor:
 * @param InPut: Input file name.
 * @param ClassNames: class names.
 * @param VarNames: variable names of the features.
 */
PndLVQTrain::PndLVQTrain(const std::string& inputFile,
			 const std::vector<std::string>& ClassNames, 
			 const std::vector<std::string>& VarNames,
			 bool trim)
  : PndMvaTrainer(inputFile, ClassNames, VarNames, trim),
    m_initConst(0.8), m_ethaZero(0.1),
    m_ethaFinal(0.0001), m_NumSweep(900), 
    m_numProto(0)
{}

/**
 * Destructor
 */
PndLVQTrain::~PndLVQTrain()
{
  std::cout << "\nCleaning all initialized objects." 
	    <<std::endl;
  //std::vector< std::pair<std::string, std::vector<float>*> > m_LVQProtos;
  for(size_t i = 0; i < m_LVQProtos.size(); i++)
  {
    // (m_LVQProtos[i].second)->clear();
    delete m_LVQProtos[i].second;
  }
  m_LVQProtos.clear();
}

void PndLVQTrain::Train()
{
  TRandom3 trand(m_RND_seed);
  //numProto may not be zero.
  assert(m_numProto != 0);
  
  // Init Proto types
  InitProtoTypes();

  const std::vector<std::pair<std::string, std::vector<float>*> >& events = m_dataSets.GetData();
  
  std::set <int>::const_iterator testSetIter;
  
  // Compute learning rate constant "a"
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.0001;
  int    numSweep     = m_NumSweep;//1000;
  unsigned int tFinal = numSweep * ( events.size() );
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal) );
  
  if(a < 0.00)
  {//Underflow
    std::cout << "\tToo small value for a." << std::endl;
    a = std::numeric_limits<double>::min();
  }
  
  if(tFinal <= static_cast<unsigned>(0))
  {// OverFlow
    std::cout << "\t tFinal Overflow." << std::endl;
    tFinal = std::numeric_limits<unsigned>::max();
  }
  
  // Print some information.
  std::cout << "\t<INFO>: Performing LVQ1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst <<", ethaZero = " 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= " << tFinal 
	    <<", learn coeff. = " << a << std::endl;
  
  // Start the training
  std::cout << "Starting to train (LVQ1)....." << std::endl;
  
  for(unsigned int time = 0; time < tFinal; time++)
  {
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
    // ethaT can become very small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min()))
    {
      ethaT  = std::numeric_limits<double>::min();
      std::cout <<"\tVery small ethaT" << std::endl;
    }
    
    if( (time % ProgStep) == 0)
    {
      std::cerr << ". " ;
    }

    if( (time % ErrorStep) == 0)
    {
      EvalClassifierError(time);
    }
    // select a random example
    int index = static_cast<int>(trand.Uniform(0.0, events.size() - 1));

    testSetIter = m_testSet_indices.find(index);

    // Selected event NOT in the test set
    while( testSetIter != m_testSet_indices.end())
    {
      index = static_cast<int>(trand.Uniform(0.0, events.size() - 1));
      testSetIter = m_testSet_indices.find(index);
    }
    
    unsigned int protoIndex = 0;
    float distance          = 0.0;
    float minProtoDistance  = std::numeric_limits<float>::max();

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
  std::cerr << std::endl;
  std::cerr << "<INFO> Finished training." 
	    << std::endl;
  WriteToWeightFile(m_LVQProtos);
}

void PndLVQTrain::Train21()
{
  TRandom3 trand(m_RND_seed);
  
  // Init LVQ protoTypes.
  if(m_numProto <= 0)
  {
    std::cerr << "\t<ERROR:> The number of prototypes MUST\n"
	      <<"be greater than zero" << std::endl;
    assert(m_numProto > 0);
  }

  // Init proto types
  InitProtoTypes();
  
  // Initialize distance container.
  if(m_distances.size() == 0)
  {
    std::cerr << "<INFO> Init Distances Container." << std::endl;
    for(unsigned int i = 0; i < m_LVQProtos.size(); i++)
    {
      m_distances.push_back(PndMvaDistObj());
    }
  }

  const std::vector<std::pair<std::string, std::vector<float>*> >& events = m_dataSets.GetData();
  std::set <int>::const_iterator testSetIter;

  // Compute learning rate constant "a"
  float windowSize = 0.3;// A value between0.2 & 0.3 is recommended.
  float s = (1 - windowSize)/(1 + windowSize);//Define the surrounding.
  
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.001;
  int    numSweep     = m_NumSweep;//100;
  unsigned int tFinal = numSweep * ( events.size());
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal));
  
  if(a < 0.00)
  {//Underflow
    std::cout << "Too small value for a." << std::endl;
    a = std::numeric_limits<double>::min();
  }
  
  if(tFinal <= static_cast<unsigned>(0))
  {// OverFlow
    std::cout << "tFinal Overflow." << std::endl;
    tFinal = std::numeric_limits<unsigned>::max();
  }
  
  // Print some information.
  std::cout << "\t<INFO>: Performing LVQ2.1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst << ", ethaZero =" 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= "<< tFinal 
	    <<", learn coeff. = " << a << ", Window = " << windowSize 
	    <<", surroun. = "<< s << std::endl;
  
  //Start learning
  std::cout << "Starting to train (LVQ2.1)....." << std::endl;
  for(unsigned int time = 0; time < tFinal; time++)
  {
    if( (time % ProgStep) == 0)
    {
      std::cerr << " ." ;
    }
    if( (time % ErrorStep) == 0)
    {
      EvalClassifierError(time);
    }

    double distance = 0.0;
    double ethaT    = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
    // ethaT can become very small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min()))
    {
      ethaT  = std::numeric_limits<double>::min();
      std::cout <<"Very small ethaT" << std::endl;
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
    std::sort(m_distances.begin(), m_distances.end());
    
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

    //Found two prototypes, one with the same lablel and one with a diff. one
    if(minFunct( (m_distances[idxSame]).m_dist / (m_distances[idx2d]).m_dist ,
		 (m_distances[idx2d]).m_dist   / (m_distances[idxSame]).m_dist ) > s)
    {
      // Update the LVQ prototype
      int deltaEqCls = 1; int deltaNonEqCls = -1;      

      // Update equal label prototype.
      UpdateProto( *(events[index].second), *(m_LVQProtos[idxSame].second), deltaEqCls, ethaT);
      
      // Update different label prototype.
      UpdateProto( *(events[index].second), *(m_LVQProtos[idx2d].second), deltaNonEqCls, ethaT);
    }
  }
  std::cerr << std::endl;
  std::cerr << "<INFO> Finished training." 
	    << std::endl;
  WriteToWeightFile(m_LVQProtos);
}

// FIX IMPLEMENT
void PndLVQTrain::TrainSec()
{}
void PndLVQTrain::Train21Sec()
{}



// ==================== Private functions =======================
/**
 * Initialize LVQ prototypes (Code books) using class conditional
 * means vectors.
 */
void PndLVQTrain::InitProtoTypes()
{
  // Clear protypes list
  cleanProtoList();
  std::cout << "<INFO> Initializing " << m_numProto 
            <<" LVQ prototypes."<< std::endl;

  // Initialize LVQ-prototypes.
  double c = m_initConst;//0.8;
  TRandom3 trand(m_RND_seed);

  const std::vector<PndMvaClass>& classes = m_dataSets.GetClasses();
  const std::vector<PndMvaVariable>& variables = m_dataSets.GetVars();
  const std::vector<std::pair<std::string, std::vector<float>*> >& events = m_dataSets.GetData();
  const std::map< std::string, std::vector<float>* >& ClsCondMeans = m_dataSets.GetClassCondMeans();

  for(size_t cl = 0; cl < classes.size(); cl++)
  {
    int minIdx = classes[cl].StartIdx;
    int maxIdx = classes[cl].EndIdx;
    std::string curClsName = classes[cl].Name;

    for(unsigned int i = 0; i < m_numProto; i++)
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
	std::cout << "\n\n======================================\n"
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
	std::cout << "\n Name collision cls is " << classes[cl].Name
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
 * Clean prototype container.
 */
void PndLVQTrain::cleanProtoList()
{
  std::cout << "<INFO> Cleaning the prototype list." 
            << std::endl;

  // Clean up the container for proto-types
  for(unsigned int k = 0; k < m_LVQProtos.size(); k++)
  {
    delete m_LVQProtos[k].second;
  }
  m_LVQProtos.clear();
}

/**
 * Updates the LVQ prototypes.
 */
void PndLVQTrain::UpdateProto(const std::vector<float>& EvtData, 
			      std::vector<float>& proto, 
			      const int delta, const double ethaT)
{
  for(size_t i = 0; i < proto.size(); i++)
  {
    proto[i] = proto[i] + ( ethaT * static_cast<double>(delta) * (EvtData[i] - proto[i]) );
  }
}
/**
 * Evaluate the train and test error for the current classifier.
 */
void PndLVQTrain::EvalClassifierError(unsigned int stp)
{
  const std::vector<std::pair<std::string, std::vector<float>*> >& events = m_dataSets.GetData();
  std::set <int>::const_iterator iter;

  int TrError, TsError;// Test and trai error
  TrError = TsError = 0;


  //========== Classify Test Set
  for(iter = m_testSet_indices.begin(); iter != m_testSet_indices.end(); ++iter)
  {
    std::string WinClassName;
    float dist    = 0.0; // Current distance
    float minDist = std::numeric_limits<float>::max();// Winner dist
    int idx = *iter;

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
    {// Wrong
      TsError++;
    }
  }

  //========== Classify train set
  // Event loop
  for(unsigned int evt = 0; evt < events.size(); evt++)
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
      {// Wrong
	TrError++;
      }
    }
  }
  float tsEr, trEr;
  tsEr = (TsError * 100.00) / static_cast<float>(m_testSet_indices.size());
  trEr = (TrError * 100.00) / static_cast<float>(events.size() - m_testSet_indices.size());
  // Add to container
  StepError StpEr (stp, trEr, tsEr);
  m_StepErro.push_back(StpEr);
  TrError = TsError = 0;
}
