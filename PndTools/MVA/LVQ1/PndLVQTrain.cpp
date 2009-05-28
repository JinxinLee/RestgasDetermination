/* ***************************************
 * LVQ  Training functions               *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version 1.0 beta1.                    *
 * ***************************************
 */

#include "PndLVQTrain.h"

/**
 * Constructor:
 * @param InPut: Input file name.
 * @param ClassNames: class names.
 * @param VarNames: variable names of the features.
 */
PndLVQTrain::PndLVQTrain(const char* InPut,
			 const std::vector<std::string>& ClassNames, 
			 const std::vector<std::string>& VarNames)
{
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;
  
  /* Read event data */
  readInput(InPut);

  // Set learning constants.
  m_initConst = 0.8; m_ethaZero = 0.1; 
  m_ethaFinal = 0.0001; m_NumSweep = 900;
  
}// End of constructor

/**
 * Destructor
 */
PndLVQTrain::~PndLVQTrain()
{
  std::cout << "Cleaning all initialized objects." << std::endl;
  // Clean up the container for class Conditional means
  std::map< std::string, std::vector<float>* >::iterator it;
  for(it = m_ClassCondMeans.begin(); it != m_ClassCondMeans.end(); ++it)
  {
    delete it->second;
  }
  m_ClassCondMeans.clear();
  
  // Clean up the container for event data
  for(unsigned int i = 0; i < m_EventsData.size(); i++)
  {
    delete m_EventsData[i].second;
  }
  m_EventsData.clear();
  
  // Clean up the container for proto-types
  for(unsigned int k = 0; k < m_LVQProtos.size(); k++)
  {
    delete m_LVQProtos[k].second;
  }
  m_LVQProtos.clear();

  m_perClsExamples.clear();
  m_ClassNames.clear();
  m_VarNames.clear();
  m_ClassIndex.clear();
  m_normFact.clear();
}

/**
 * @param numProto: number of LVQ1 prototypes. Current implementation
 *  considers an equal number of prototypes for each class with equal
 *  weights.
 * @param OutPut: Out-put file, where the weights are stored.
 */
void PndLVQTrain::Train(const int numProto, const char* outPut)
{
  TRandom3 trand(435775);
  
  // Init LVQ protoTypes.
  if(numProto <= 0)
  {
    std::cerr << "<ERROR> The number of prototypes\n" 
	      << " MUST be greater than zero (0)."<< std::endl;
    return;
  }

  if(!outPut)
  {
    std::cerr << "You need to specify the output file" << std::endl;
    return;
  }
  // Init Proto types

  //InitProtoTypes(numProto);
  InitProtoTypesWithClsMean(numProto);

  // All protypes are initialized. We can perform the training
  // Compute learning rate constant "a"
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.0001;
  int    numSweep     = m_NumSweep;//1000;
  unsigned int tFinal = numSweep * ( m_EventsData.size() );
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal) );
  
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
  std::cout << "\t<INFO>: Performing LVQ1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst <<", ethaZero = " 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= " << tFinal 
	    <<", learn coeff. = " << a << "\nPrototypes will be stored in "
	    << outPut << std::endl;
  
  // Start the training
  std::cout << "Starting to train (LVQ1)....." << std::endl;
  for(unsigned int time = 0; time < tFinal; time++)
  {
    if( (time % 100000) == 0)
    {
      std::cerr << " ." ;
    }

    int    protoIndex       = 0;
    double distance         = 0.0;
    double minProtoDistance = std::numeric_limits<float>::max();//1000000.0;
    
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
    // ethaT can become very small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min()))
    {
      ethaT  = std::numeric_limits<double>::min();
      std::cout <<"Very small ethaT" << std::endl;
    }
    
    // select a random example
    int index = (int) trand.Poisson( (time + 2) * 10000) % (m_EventsData.size() - 1);
    
    // Compute the distance to all available LVQ proto-types
    for(unsigned int ix = 0; ix < m_LVQProtos.size(); ix++)
    {
      distance = ComputeDist( *(m_EventsData[index].second), *(m_LVQProtos[ix].second) );
      
      if(distance < minProtoDistance)
      {
	minProtoDistance = distance;//minimum distance
	protoIndex  = ix;//index of the prototype with min dist
      }
    }
    
    // We need to update the (winner) prototype
    int delta = 0;
    // determine delta
    if( m_EventsData[index].first == m_LVQProtos[protoIndex].first )
    {
      // Equal labels
      delta = 1;
    }
    else
    {
      // Diff. Labels
      delta = -1;
    }// delta is calculated
    
    // Update the LVQ prototype
    UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[protoIndex].second), delta, ethaT);
  }
  std::cerr << std::endl;
  
  // Write the coordinates of the prototypes to the file
  WriteToProtoFile(outPut);
}

/**
 * Train the classifier accourding to LVQ2.1 algorithm.
 * @param numProto: number of LVQ2 prototypes. Current
 * implementation considers an equal number of prototypes for each
 * class with equal weights.
 * @param OutPut: Out-put file, where the weights are stored.
 */
void PndLVQTrain::Train21(const int numProto, const char* outPut)
{
  TRandom3 trand(435573);
  
  // Container to store distances.
  std::vector <PndLVQDistObj*> distances;
  
  // Init LVQ protoTypes.
  if(numProto <= 0)
  {
    std::cerr << "\t<ERROR:> The number of prototypes MUST\n"
	      <<"be greater than zero" << std::endl;
    return;
  }
  
  if(!outPut)
  {
    std::cerr << "You need to specify the output file." << std::endl;
    return;
  }
  
  // Init proto types
  //InitProtoTypes(numProto);
  InitProtoTypesWithClsMean(numProto);
  
  // Initialize distance container.
  for(unsigned int i = 0; i < m_LVQProtos.size(); i++)
  {
    PndLVQDistObj* dd = new PndLVQDistObj();
    distances.push_back(dd);
  }
  
  // All protypes are initialized. We can perform the training
  // Compute learning rate constant "a"
  float windowSize = 0.2;// A value between0.2 & 0.3 is recommended.
  float s = (1 - windowSize)/(1 + windowSize);//Define the surrounding.
  
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.001;
  int    numSweep     = m_NumSweep;//100;
  unsigned int tFinal = numSweep * ( m_EventsData.size() );
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
	    <<", surroun. = "<< s << "\nPrototypes will be stored in "
	    << outPut <<std::endl;
  
  //Start learning
  std::cout << "Starting to train (LVQ2.1)....." << std::endl;
  for(unsigned int time = 0; time < tFinal; time++)
  {
    if( (time % 100000) == 0)
    {
      std::cerr << " ." ;
    }
    double distance         = 0.0;
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    // ethaT can become very small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min()))
    {
      ethaT  = std::numeric_limits<double>::min();
      std::cout <<"Very small ethaT" << std::endl;
    }
    
    // select a random example
    int index = (int) trand.Poisson( (time + 2) * 10000) % (m_EventsData.size() - 1);
    
    // Compute the distance to all available LVQ proto-types
    for(unsigned int ix = 0; ix < m_LVQProtos.size(); ix++)
    {
      distance = ComputeDist( *(m_EventsData[index].second), *(m_LVQProtos[ix].second) );
      
      // Store distance.
      (distances[ix])->m_idx   = ix;
      (distances[ix])->m_dist = distance;
      (distances[ix])->m_cls  = m_LVQProtos[ix].first;
    }// All distances are determined.
    
    // Sort the distances.
    sort(distances.begin(), distances.end());
    
    /*
     * We need to Select the two nearest codebooks and update
     * them. Per definition the first one has index zero (0), thus we
     * need to find the second one.
     */
    int idxSame = 0; int idx2d = 0;
    if( m_EventsData[index].first == (distances[idxSame])->m_cls )
    {
      //Equal labels
      idx2d = 1;
    }
    else
    {
      //Diff. labels
      idxSame = 1;
    }
    if(idxSame == 0)
    {
      //Find one with a diff. label
      while(m_EventsData[index].first == (distances[idx2d])->m_cls)
      {
	idx2d++;
      }
    }
    else
    {
      //Find one with the same label.
      while(m_EventsData[index].first != (distances[idxSame])->m_cls)
      {
	idxSame++;
      }
    }
    
    //Found two prototypes, one with the same lablel and one with a diff. one
    if(minFunct( (distances[idxSame])->m_dist / (distances[idx2d])->m_dist ,
		 (distances[idx2d])->m_dist   / (distances[idxSame])->m_dist ) > s)
    {
      // Update the LVQ prototype
      int deltaEqCls = 1; int deltaNonEqCls = -1;
      
      // Update equal label prototype.
      UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[idxSame].second), deltaEqCls, ethaT);
      
      // Update different label prototype.
      UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[idx2d].second), deltaNonEqCls, ethaT);
    }
  }// (for time = 0;)Training is finished
  
  std::cerr << std::endl;
  
  // Write the coordinates of the prototypes (Codebook) to a file.
  WriteToProtoFile(outPut);
  
  // We are done. Clean distances.
  for(unsigned int i = 0; i < distances.size(); i++){
    delete distances[i];
  }
  distances.clear();
}

////////////////////////// Protected functions ///////////
/**
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float PndLVQTrain::ComputeDist(const std::vector<float> &EvtData, 
			       const std::vector<float> &Ex)
{
  float dist = 0.0;
  
  for(unsigned int i = 0; i< Ex.size(); i++)
  {
    dist += (EvtData[i] - Ex[i]) * (EvtData[i] - Ex[i]);
  }
  return sqrt(dist);
}

///////////////////////////// Private functions /////////////////
/**
 * Initialize LVQ prototypes (Code books).
 */
void PndLVQTrain::InitProtoTypes(const int numProto)
{
  cleanProtoList();
  
  std::cout << "Initializing " << numProto 
	    <<" LVQ prototypes."<< std::endl;
  
  // Initialize LVQ-prototypes.
  double c = m_initConst;//0.8;
  TRandom3 trand(435375);
  
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++)
  {
    int minIdx = m_ClassIndex[cl].first;
    int maxIdx = m_ClassIndex[cl].second;
    
    for(int i = 0; i < numProto; i++)
    {
      // select a random example
      if(minIdx == 0){minIdx = 1;}
      int index = (int) (trand.Poisson( (float)((maxIdx + minIdx)/2))) % (maxIdx);
      
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
	return;
      }
      
      // We have found a random event.
      std::vector<float>* proto   = new std::vector<float>(m_VarNames.size(),0.0);
      std::vector<float>* evtData = m_EventsData[index].second;
      
      // May not happen, DEBUG DEBUG DEBUG
      if(m_ClassNames[cl] != m_EventsData[index].first)
      {
	std::cout << "\n Name collision cls is " << m_ClassNames[cl]
		  << " index is " << index << " minindex " << minIdx
		  << std::endl;
      }
      
      for(unsigned int k = 0; k < evtData->size(); k++)
      {
	proto->at(k) = evtData->at(k) * c + (1.0 - c) * evtData->at(k);
      }
      
      // proto type is initialized, add to the container
      m_LVQProtos.push_back(std::make_pair(m_EventsData[index].first, proto));
      
    }//END for(i = 0; i < numProto)
  }// END for(cl = 0)
}

/**
 * Updates the LVQ1 prototypes 
 */
void PndLVQTrain::UpdateProto( const std::vector<float> &EvtData, std::vector<float> &proto, 
			       const int delta, const double ethaT )
{
  for(unsigned int i = 0; i < proto.size(); i++)
  {
    //proto[i] = proto[i] + ethaT * (1.0 - 2.0 * static_cast<double>(delta)) * (EvtData[i] - proto[i]);
    proto[i] = proto[i] + ( ethaT * static_cast<double>(delta) * (EvtData[i] - proto[i]) );
  }
}

/**
 * Compute the class conditional mean for a given class and store
 * that in the class conditional means container
 */
void PndLVQTrain::CompClsCondMean(const std::string clsName)
{
  std::cout << "\t<INFO> Determining class conditional mean for "
	    << "\n\t\t class " << clsName << "." << std::endl;

  std::vector <float>* vec = new std::vector <float> (m_VarNames.size(),0.0);
  for(unsigned int i = 0; i < m_EventsData.size(); i++)
  {
    if( clsName == m_EventsData[i].first )
    {
      for(unsigned int j = 0; j < m_VarNames.size(); j++ )
      {
	vec->at(j) += (m_EventsData[i].second)->at(j);
      }
    }
  }// All available events are visited
  
  /* 
   * Normalizing by dividing by the total number of available examples
   * for the current class
   */
  int numExam = m_perClsExamples[clsName];
  for(unsigned int k = 0; k < vec->size(); k++)
  {
    vec->at(k) = (vec->at(k)/(static_cast<float>(numExam)));
  }
  // Add to the Class Conditional Means container
  m_ClassCondMeans.insert(std::make_pair(clsName,vec));
}

// Clean-up the prototypes list
void PndLVQTrain::cleanProtoList()
{
  std::cout << "\t<INFO> Cleaning the prototype list." 
	    << std::endl;
  
  // Clean up the container for proto-types
  for(unsigned int k = 0; k < m_LVQProtos.size(); k++)
  {
    delete m_LVQProtos[k].second;
  }
  m_LVQProtos.clear();
}

/**
 * Write the prototypes to the out-put file 
 */
void PndLVQTrain::WriteToProtoFile(const char* outPut)
{
  std::cerr << "\t<INFO> Writing Trained prototypes to "
	    << outPut << std::endl;
  /* Open out put file and write coordinates of the prototypes */
  TFile out (outPut,"RECREATE");

  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++)
  {
    std::vector<float> vars(m_VarNames.size(), 0.0);
    std::string treename = m_ClassNames[cls];
    std::string treedesc = "Description Of " + treename;

    // Create a tree
    TTree sig (treename.c_str(), treedesc.c_str());

    // Create branches and bind the variables
    for(unsigned int j = 0; j < m_VarNames.size(); j++)
    {
      std::string vname = m_VarNames[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();

      // Bind the parameters to the tree elements.
      sig.Branch(bname, &vars[j], lname);
    }
    // Fill The prototypes tree
    for(unsigned int i = 0; i< m_LVQProtos.size(); i++)
    {
      if(m_LVQProtos[i].first == treename)
      {
	for(unsigned int k = 0; k < vars.size(); k++){
	  vars[k] = (m_LVQProtos[i].second)->at(k);
	}
	sig.Fill();
      }
    }  
  
    // Write the created tree
    sig.Write();
  }// End for cls = 0;

  // Write normFactors 
  std::vector<float> vars(m_VarNames.size(), 0.0);
  std::string name = "NormFact";
  std::string desc = "desc of " + name;
  TTree fact(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(unsigned int j = 0; j < m_VarNames.size(); j++)
  {
    std::string vname = m_VarNames[j];
    std::string leaf  = vname + "/F" ;
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    fact.Branch  (bname, &vars[j], lname);
  }
  
  // Fill the trees.
  for(unsigned int i = 0; i < m_normFact.size(); i++)
  {
    std::string n =  m_VarNames[i];
    vars[i] = m_normFact[n];
  }
  fact.Fill();
  fact.Write();

  //Close open file
  out.Close();
}

/**
 * Will be deleted SOON.
 * Write the normalized DataSet to the out-put file.
 * @param  outFile  File name to write to
 */
void PndLVQTrain::WriteDataToFile(const char* outPut)
{
  std::cerr << "\t<INFO> Writing normalized data to "
	    << outPut << std::endl;
  /* Open out put file and write coordinates of the prototypes */
  TFile* out = new TFile(outPut,"RECREATE");
  
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++)
  {
    std::vector<float> vars(m_VarNames.size(),0.0);
    
    std::string name = m_ClassNames[cls];
    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();
    
    // Create a tree
    TTree sig (treeName, treeDesc);
    
    // Create branches and bind the variables
    for(unsigned int j = 0; j < m_VarNames.size(); j++)
    {
      std::string vname = m_VarNames[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      // Bind the parameters to the tree elements.
      sig.Branch(bname,&vars[j],lname);
    }
    // Fill The tree
    for(unsigned int i = 0; i< m_EventsData.size(); i++)
    {
      if(m_EventsData[i].first == name)
      {
	for(unsigned int k = 0; k < vars.size(); k++){
	  vars[k] = (m_EventsData[i].second)->at(k);
	}
	sig.Fill();
      }
    }
    // Write the created tree
    sig.Write();
  }
  // We are done. We can close the open file and delete the pointer
  out->Close();
  delete out;
}

/**
 * Determines the median for parameters of the loaded DataSet.
 */
void PndLVQTrain::DetermineMediaan()
{
  std::cout << "\t<INFO> Determining median for each parameter." 
	    << std::endl;
  
  float median = 0.00; float Fquartil = 0.00;
  std::vector <float> varVect(m_EventsData.size(), 0.00);    

  // We need to determine the median and inter-quartile distances per
  // feature.
  
  // Loop through the Variables
  for(unsigned int i = 0; i < m_VarNames.size(); i++)
  {
    // Loop through the Examples
    for(unsigned int ev = 0; ev < m_EventsData.size(); ev++)
    {
      varVect[ev] = (m_EventsData[ev].second)->at(i);
    }
    // Sort the variables
    std::sort(varVect.begin(), varVect.end());

    if(varVect.size() % 2 == 0)
    {
      median = ( varVect[(varVect.size() / 2) - 1] + varVect[(varVect.size() / 2)]) / static_cast<float>(2);
      Fquartil = varVect[( (varVect.size() + 2)/4) - 1 ];
    }
    else
    {
      median = varVect[(varVect.size() / 2)];
      Fquartil = varVect[( (varVect.size() + 1)/4) - 1 ];
    }
    // Store values
    m_normFact.insert(std::make_pair(m_VarNames[i], Fquartil));
    std::cout << m_VarNames[i] << "\tmedian = " << median 
	      << "\t IntQuartDist = " << Fquartil << std::endl;
  }
}

/**
 * Determine the Min Max distance.
 */
void PndLVQTrain::MinMaxDiff()
{
  std::vector <float> vec (m_EventsData.size(), 0.0);
  // Variables Loop
  for(unsigned int i = 0; i < m_VarNames.size(); i++)
  {
    // Event loop
    for(unsigned int j = 0; j < m_EventsData.size(); j++)
    {
      vec[j] = (m_EventsData[j].second)->at(i);
    }
    // Sort variables
    std::sort(vec.begin(),vec.end());

    std::cout << m_VarNames[i] << "\tmin = " << vec[0] 
	      << "\t\t max = " << vec[vec.size() - 1 ];

    float diff = vec[ vec.size() - 1 ] - vec[0];
    // Store values
    m_normFact.insert(std::make_pair(m_VarNames[i], diff));
    std::cout << "\t\t diff = " << diff << std::endl;
  }
}

/**
 * Computes Variance (unbiased estimator) for each parameter in the
 * feature list.
 * @param clsName The name of the class of events for with we want
 * to compute Var(X).
 */
void PndLVQTrain::ComputeVariance()
{
  std::vector <float> vec(m_EventsData.size(), 0.0);
  float mean = 0.0;
  float variance = 0.0;

  //Variable Loop
  for(unsigned int i = 0; i < m_VarNames.size(); i++ )
  {
    // Event Loop
    for(unsigned int ev = 0; ev < m_EventsData.size(); ev++)
    {
      vec[ev] = (m_EventsData[ev].second)->at(i);
      mean   += (m_EventsData[ev].second)->at(i);
    }
    // Compute variable mean and variance
    mean /= static_cast<float>(m_EventsData.size());
    
    for(unsigned int ev = 0; ev < m_EventsData.size(); ev++)
    {
      variance += (vec[ev] - mean) * (vec[ev] - mean);
    }
    variance = variance / static_cast<float>(m_EventsData.size() - 1 );
    variance = sqrt(variance);
    
    // Insert
    std::string varName = m_VarNames[i];
    m_normFact.insert(std::make_pair(varName, variance));
    std::cout << varName << "\t mean = " << mean
	      << "\t\tVar(X) = " << variance << std::endl;
    // Reset
    mean = 0.0;
    variance = 0.0;
  }
}

void PndLVQTrain::NormalizeDataSet(const NormType t)
{
  m_normFact.clear();
  
  switch(t){
  case VARX:
    std::cout << "\t<INFO> Normalizing the dataset "
	      << "using samle Variance. "
	      << std::endl;
    ComputeVariance();
    break;
  case MINMAX:
    MinMaxDiff();
    std::cout << "<INFO>\tNormalizing dataset using Min Max spread." 
	      <<std::endl;
    break;
  case MEDIAN:
    std::cout << "\t<INFO> Normalizing the dataset "
	      << "using Median and Inter Quartile Distance."
	      << std::endl;
    DetermineMediaan();
    break;
  default:
    std::cout << "<INFO> No normalization scheme was selected." 
	      << std::endl;
  }
  
  if(m_normFact.size() != 0)
  {
    // Event Loop
    for(unsigned int ev = 0; ev < m_EventsData.size(); ev++)
    {
      // Parameters Loop
      for(unsigned int i = 0; i < m_VarNames.size(); i++)
      {
	std::string varName = m_VarNames[i];
	(m_EventsData[ev].second)->at(i) = (m_EventsData[ev].second)->at(i) / (m_normFact[varName]);
      }
    }
  }
  //WriteDataToFile("InputVarianceNormalized.root");
  //WriteDataToFile("InputMinMaxNormalized.root");
  //WriteDataToFile("InputMedianNormalized.root");
}

void PndLVQTrain::readInput(const char *InPut)
{
  std::cout << "<INFO> Reading data from  "<< InPut 
	    << std::endl;
  // Open the input file for reading the event data.
  TFile* m_InPutFile = new TFile(InPut,"READ");
  
  int minIdx,maxIdx;
  minIdx = 0; maxIdx = -1;
  
  // Fetch the class trees and read the event data.
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++)
  {
    // Tree name
    const char *name = m_ClassNames[cls].c_str();
    
    // Get the tree object
    TTree *t = (TTree*) m_InPutFile->Get(name);
    
    // Init a container to bind to the tree branches
    std::vector<float> ev (m_VarNames.size(),0.0);
    
    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < m_VarNames.size(); j++)
    {
      const char* branchName = m_VarNames[j].c_str();
      //Binding the branches
      t->SetBranchAddress(branchName, &(ev[j]));
    }// Tree parameters are bounded
    
    // Fetch the number of examples available for the current class
    m_perClsExamples.insert(std::make_pair(m_ClassNames[cls],
					   t->GetEntriesFast()));
    // Find-out the min and max indices  per class
    maxIdx = maxIdx + t->GetEntriesFast();
    m_ClassIndex.push_back(std::make_pair(minIdx,maxIdx));
    minIdx = maxIdx + 1;
    
    // Fetch and store the variables to per class variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++)
    {
      t->GetEntry(k);
      
      // Container to store the vent data read from the input tree
      std::vector<float>* EvtDat = new std::vector<float>();
      
      for(unsigned int idx = 0; idx < m_VarNames.size(); idx++)
      {
	EvtDat->push_back(ev[idx]);
      }
      
      // Store the event and its class name
      m_EventsData.push_back(std::make_pair(m_ClassNames[cls], EvtDat));
    }
    
    // We are done and can delete the tree pointer
    delete t;
  }// End of for(cls) loop for all classes
  
  // Close the open file and delete the file pointer
  m_InPutFile->Close();
  delete m_InPutFile;
}
// End of class implementation.

//////////////////////=========== Test Functions ///////////////////
/**
 * Initialize LVQ prototypes (Code books) on the mean position between
 * the randomly selected proto type and the class conditional means
 * vector.
 */

//=========================== FIXME HIER BEN JE BEZIG. =============//
void PndLVQTrain::InitProtoTypesWithClsMean(const int numProto)
{
  // Clear protypes list
  cleanProtoList();

  //Init Class Conditional means
  for(unsigned int id = 0; id < m_ClassNames.size(); id++){
    CompClsCondMean(m_ClassNames[id]);
  }

  std::cout << "Initializing " << numProto 
	    <<" LVQ prototypes."<< std::endl;
  
  // Initialize LVQ-prototypes.
  double c = m_initConst;//0.8;
  TRandom3 trand(435375);
  
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++)
  {
    
    std::vector<float>* clsMean = m_ClassCondMeans[m_ClassNames[cl]];
    
    // Start and end indices
    int minIdx = m_ClassIndex[cl].first;
    int maxIdx = m_ClassIndex[cl].second;
    
    for(int i = 0; i < numProto; i++)
    {
      // select a random example
      if(minIdx == 0){minIdx = 1;}
      int index = (int) (trand.Poisson( (float)((maxIdx + minIdx)/2))) % (maxIdx);
      
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
	return;
      }
      
      // We have found a random event.
      std::vector<float>* evtData = m_EventsData[index].second;

      // May not happen, DEBUG DEBUG DEBUG
      if(m_ClassNames[cl] != m_EventsData[index].first)
      {
	std::cout << "\n Name collision cls is " << m_ClassNames[cl]
		  << " index is " << index << " minindex " << minIdx
		  << std::endl;
      }

      // New Proto type
      std::vector<float>* proto   = new std::vector<float>(m_VarNames.size(),0.0);
      
      for(unsigned int k = 0; k < evtData->size(); k++)
      {
	c = trand.Gaus(0.0, 1.0);
	//proto->at(k) = evtData->at(k) * c + (1.0 - c) * evtData->at(k);
	proto->at(k) = ( clsMean->at(k) + evtData->at(k) ) / 2.00;
      }
      
      // proto type is initialized, add to the container
      m_LVQProtos.push_back(std::make_pair(m_EventsData[index].first, proto));

    }//END for(i = 0; i < numProto)
  }//End for(cls = 0
}

void PndLVQTrain::TrainSec(const int numProto, const char* outPut)
{
  TRandom3 trand(435775);
  std::vector<unsigned int> indices(m_ClassNames.size(),0);
  
  // Init LVQ protoTypes.
  if(numProto <= 0){
    std::cerr << "<ERROR> The number of prototypes\n"
	      << " MUST be greater than zero (0)."<< std::endl;
    return;
  }
  
  //InitProtoTypes(numProto);
  InitProtoTypesWithClsMean(numProto);
  
  if(!outPut){
    std::cerr << "You need to specify the output file" << std::endl;
    return;
  }

  // All protypes are initialized. We can perform the training
  // Compute learning rate constant "a"
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.0001;
  int    numSweep     = m_NumSweep;//1000;
  unsigned int tFinal = numSweep * ( m_EventsData.size() );
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal) );
  
  if(a < 0.00){//Underflow
    std::cout << "Too small value for a." << std::endl;
    a = std::numeric_limits<double>::min();
  }

  if(tFinal <= static_cast<unsigned>(0)){// OverFlow
    std::cout << "tFinal Overflow." << std::endl;
    tFinal = std::numeric_limits<unsigned>::max();
  }

  // Print some information.
  std::cout << "\t<INFO>: Performing LVQ1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst <<", ethaZero = "
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= " << tFinal
	    <<", learn coeff. = " << a << "\nPrototypes will be stored in "
	    << outPut << std::endl;
  
  // Start the training
  std::cout << "\t<INFO> Starting to train Per Class Example (LVQ1)....." 
	    << std::endl;
  
  for(unsigned int time = 0; time < tFinal; time++){
    //Write progress to std::cerr
    if( (time % 100000) == 0){
      std::cerr << " ." ;
    }

    int    protoIndex       = 0;
    double distance         = 0.0;
    double minProtoDistance = std::numeric_limits<float>::max();//1000000.0;

    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    // ethaT can become too small
    if( ethaT <= (1.50 * std::numeric_limits<double>::min())){
      ethaT  = std::numeric_limits<double>::min();
      std::cout <<"Very small ethaT" << std::endl;
    }

    // select number of classes, random examples
    for(unsigned int exa = 0; exa < m_ClassIndex.size(); exa++){
      indices[exa] = (int) trand.Uniform(m_ClassIndex[exa].first, m_ClassIndex[exa].second);
    }

    for(unsigned int k = 0; k < indices.size(); k++){
      int index = indices[k];
      
      // Compute the distance to all available LVQ proto-types
      for(unsigned int ix = 0; ix < m_LVQProtos.size(); ix++){
	distance = ComputeDist( *(m_EventsData[index].second), *(m_LVQProtos[ix].second) );
	
	if(distance < minProtoDistance){
	  minProtoDistance = distance;//minimum distance
	  protoIndex  = ix;//index of the prototype with min dist
	}
      }// END for(ix)
      
      // We need to update the (winner) prototype
      int delta = 0;

      // determine delta
      if( m_EventsData[index].first == m_LVQProtos[protoIndex].first ){
	// Equal labels
	delta = 1;
      }
      else{
	// Diff. Labels
	delta = -1;
      }// delta is calculated
    
      // Update the LVQ prototype
      UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[protoIndex].second), delta, ethaT);
    }//End for(k=0)
  }//End for(times)
  std::cerr << std::endl;
  // Write the coordinates of the prototypes to the file
  WriteToProtoFile(outPut);
}

void PndLVQTrain::Train21Sec(const int numProto, const char* outPut)
{
  /////////////////////////////////
  TRandom3 trand(435573);
  std::vector<unsigned int> indices(m_ClassNames.size(),0);
  
  // Container to store distances.
  std::vector <PndLVQDistObj*> distances;
  
  // Init LVQ protoTypes.
  if(numProto <= 0){
    std::cerr << "\t<ERROR:> The number of prototypes MUST\n"
	      <<"be greater than zero" << std::endl;
    return;
  }

  if(!outPut){
    std::cerr << "You need to specify the output file." << std::endl;
    return;
  }

  //InitProtoTypes(numProto);
  InitProtoTypesWithClsMean(numProto);
    
  // Initialize distance container.
  for(unsigned int i = 0; i < m_LVQProtos.size(); i++)
  {
    PndLVQDistObj* dd = new PndLVQDistObj();
    distances.push_back(dd);
  }
  
  // All protypes are initialized. We can perform the training
  // Compute learning rate constant "a"
  float windowSize = 0.2;// A value between0.2 & 0.3 is recommended.
  float s = (1 - windowSize)/(1 + windowSize);//Define the surrounding.
  
  double ethaZero     = m_ethaZero;//0.1;
  double ethaFinal    = m_ethaFinal;//0.001;
  int    numSweep     = m_NumSweep;//100;
  unsigned int tFinal = numSweep * ( m_EventsData.size() );
  long double a       = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal));
  
  if(a < 0.00){//Underflow
    std::cout << "Too small value for a." << std::endl;
    a = std::numeric_limits<double>::min();
  }
  
  if(tFinal <= static_cast<unsigned>(0)){// OverFlow
    std::cout << "tFinal Overflow." << std::endl;
    tFinal = std::numeric_limits<unsigned>::max();
  }
  
  // Print some information.
  std::cout << "\t<INFO>: Performing LVQ2.1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst << ", ethaZero =" 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= "<< tFinal 
	    <<", learn coeff. = " << a << ", Window = " << windowSize 
	    <<", surroun. = "<< s << "\nPrototypes will be stored in "
	    << outPut <<std::endl;
  
  //Start learning
  std::cout << "Starting to train (LVQ2.1)....." << std::endl;
  for(unsigned int time = 0; time < tFinal; time++){
    if( (time % 100000) == 0){
      std::cerr << " ." ;
    }
    double distance         = 0.0;
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
    if( ethaT <= (1.50 * std::numeric_limits<double>::min())){
      ethaT  = std::numeric_limits<double>::min();
      std::cout <<"Very small ethaT" << std::endl;
    }

    // select number of classes, random examples
    for(unsigned int exa = 0; exa < m_ClassIndex.size(); exa++){
      indices[exa] = (int) trand.Uniform(m_ClassIndex[exa].first, m_ClassIndex[exa].second);
    }
    
    for(unsigned int k = 0; k < indices.size(); k++){
      int index = indices[k];

      // Compute the distance to all available LVQ proto-types
      for(unsigned int ix = 0; ix < m_LVQProtos.size(); ix++){
	distance = ComputeDist( *(m_EventsData[index].second), *(m_LVQProtos[ix].second) );
	
	// Store distance.
	(distances[ix])->m_idx   = ix;
	(distances[ix])->m_dist = distance;
	(distances[ix])->m_cls  = m_LVQProtos[ix].first;
      }// All distances are determined.
      
      // Sort the distances.
      sort(distances.begin(), distances.end());
      
      /*
       * We need to Select the two nearest codebooks and update
       * them. Per definition the first one has index zero (0), thus we
       * need to find the second one.
       */
      int idxSame = 0; int idx2d = 0;
      if( m_EventsData[index].first == (distances[idxSame])->m_cls ){
	//Same labels
	idx2d = 1;
      }
      else{//Diff. labels
	idxSame = 1;
      }
      if(idxSame == 0){
	//Find one with a diff. label
	while(m_EventsData[index].first == (distances[idx2d])->m_cls){
	  idx2d++;
	}
      }
    else{//Find one with the same label.
      while(m_EventsData[index].first != (distances[idxSame])->m_cls){
	idxSame++;
      }
    }
      
      //Found two prototypes, one with the same lablel and one with a diff. one
      if(minFunct( (distances[idxSame])->m_dist / (distances[idx2d])->m_dist ,
		   (distances[idx2d])->m_dist   / (distances[idxSame])->m_dist ) > s){
	
	// Update the LVQ prototype
	int deltaEqCls = 1; int deltaNonEqCls = -1;
	
	// Update equal label prototype.
	UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[idxSame].second), deltaEqCls, ethaT);
	
	// Update different label prototype.
	UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[idx2d].second), deltaNonEqCls, ethaT);
      }
    }//END for(k=0)
  }// (for time = 0;)Training is finished
  
  std::cerr << std::endl;
  // Write the coordinates of the prototypes (Codebook) to a file.
  WriteToProtoFile(outPut);
  
  // We are done. Clean distances.
  for(unsigned int i = 0; i < distances.size(); i++){
    delete distances[i];
  }
  distances.clear();  
  //////////////////////////////////
}
