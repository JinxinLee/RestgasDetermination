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
  
  /* 
   * Set the initial values for the learning constants. Note, One
   * needs to change these for better learning result.
   */
  m_initConst = 0.8; m_ethaZero = 0.1; 
  m_ethaFinal = 0.0001; m_NumSweep = 500;
  
  // Compute mean vector for each class
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++){
    std::string ClName = m_ClassNames[cl];
    CompClsCondMean(ClName);
  }
  // Compute Var(X)
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++){
    std::string ClName = m_ClassNames[cl];
    ComputeVariance(ClName);
  }

  // ====== Normalize the loaded dataSet
  //DetermineMediaan();
  //NormalizeWithMedian();
  NormalizeWithVariance();
  WriteDataToFile("InputNormalized.root");
}// End of constructor

/**
 * Destructor
 */
PndLVQTrain::~PndLVQTrain()
{
  // Clean up the container for class Conditional means
  std::map< std::string, std::vector<float>* >::iterator it;
  for(it = m_ClassCondMeans.begin(); it != m_ClassCondMeans.end(); ++it)
  {
    delete it->second;
  }
  m_ClassCondMeans.clear();

  // Clean up the container for VAR(X)
  for(it = m_ClassVarian.begin(); it != m_ClassVarian.end(); it++)
  {
    delete it->second;
  }
  m_ClassVarian.clear();
 
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
  
  for(unsigned int k = 0; k < m_ClsMedianQrtlDis.size(); k++)
  {
    delete (m_ClsMedianQrtlDis[k].second).first;
    delete (m_ClsMedianQrtlDis[k].second).second;
  }
  m_ClsMedianQrtlDis.clear();
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
  if(numProto > 0)
  {
    InitProtoTypes(numProto);
  }
  else
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
  // All protypes are initialized. We can perform the training
  // Compute learning rate constant "a"
  double ethaZero  = m_ethaZero;//0.1;
  double ethaFinal = m_ethaFinal;//0.0001;
  int    numSweep  = m_NumSweep;//1000;
  int    tFinal    = numSweep * ( m_EventsData.size() );
  double a         = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal) );
  
  // Print some information.
  std::cout << "\t<INFO>: Performing LVQ1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst <<", ethaZero = " 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= " << tFinal 
	    <<", learn coeff. = " << a << "\nPrototypes will be stored in "
	    << outPut << std::endl;
  
  // Start the training
  for(int time = 0; time < tFinal; time++)
  {
    int    protoIndex       = 0;
    double distance         = 0.0;
    double minProtoDistance = std::numeric_limits<float>::max();//1000000.0;
    
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
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
    {// Equal labels
      //delta = 0;
      delta = 1;
    }
    else
    {// Diff. Labels
      delta = -1;
      //delta = 0;
    }// delta is calculated
    
    // Update the LVQ prototype
    UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[protoIndex].second), delta, ethaT);
  }
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
  if(numProto > 0)
  {
    InitProtoTypes(numProto);
  }
  else
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
  
  double ethaZero  = m_ethaZero;//0.1;
  double ethaFinal = m_ethaFinal;//0.001;
  int    numSweep  = m_NumSweep;//100;
  int    tFinal    = numSweep * ( m_EventsData.size() );
  double a         = (ethaZero - ethaFinal)/(ethaFinal * static_cast<double>(tFinal));
  
  // Print some information.
  std::cout << "\t<INFO>: Performing LVQ2.1 learning with parameters:\n"
	    <<"Init constant = " << m_initConst << ", ethaZero =" 
	    << ethaZero << ", ethaFinal = " << ethaFinal
	    <<", numSweep = " << numSweep << ", tFinal= "<< tFinal 
	    <<", learn coeff. = " << a << ", Window = " << windowSize 
	    <<", surroun. = "<< s << "\nPrototypes will be stored in "
	    << outPut <<std::endl;
  
  //Start learning  
  for(int time = 0; time < tFinal; time++)
  {
    double distance         = 0.0;
    double ethaT = (ethaZero) / (1.0 + (a * static_cast<double>(time)));
    
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
    {//Same labels
      idx2d = 1;
    }
    else
    {//Diff. labels
      idxSame = 1;
    }
    if(idxSame == 0)
    {//Find one with a diff. label
      while(m_EventsData[index].first == (distances[idx2d])->m_cls)
      {
	idx2d++;
      }
    }
    else
    {//Find one with the same label.
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
      //int deltaEqCls = 1; int deltaNonEqCls = 0;
      int deltaEqCls = 1; int deltaNonEqCls = -1;
      
      // Update equal label prototype.
      UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[idxSame].second), deltaEqCls, ethaT);
      
      // Update different label prototype.
      UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[idx2d].second), deltaNonEqCls, ethaT);
    }
  }// (for time = 0;)Training is finished
  
  // Write the coordinates of the prototypes (Codebook) to a file.
  WriteToProtoFile(outPut);
  
  // We are done. Clean distances.
  for(unsigned int i = 0; i < distances.size(); i++)
  {
    delete distances[i];
  }
  distances.clear();
}

////////////////////////// Protected functions ///////////

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
  }
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
 * Compute the class conditional mean for a given class and store
 * that in the class conditional means container
 */
void PndLVQTrain::CompClsCondMean(const std::string clsName)
{
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

/**
 * Computes Variance (unbiased estimator) for each parameter in the
 * feature list.
 * @param clsName The name of the class of events for with we want
 * to compute Var(X).
 */
void PndLVQTrain::ComputeVariance(const std::string clsName)
{
  if(m_ClassCondMeans.size() == 0)
  {
    std::cerr << "<Warning>: Class means are not initialized."
	      << std::endl;
    return;// FIXME, we may re-init clas-means.
  }
  std::vector <float>* vec = new std::vector <float> (m_VarNames.size(), 0.0);
  for(unsigned int i = 0; i < m_EventsData.size(); i++)
  {
    if( clsName == m_EventsData[i].first )
    {
      for(unsigned int j = 0; j < m_VarNames.size(); j++ )
      {
	float mean  = (m_ClassCondMeans[clsName])->at(j);
	float val   = (m_EventsData[i].second)->at(j);
	vec->at(j) += (val - mean) * (val - mean);
      }
    }
  }
  
  // All available events are visited
  // Normalizing
  int numExam = m_perClsExamples[clsName] - 1;
  
  for(unsigned int k = 0; k < vec->size(); k++){
    vec->at(k) = sqrt( (vec->at(k)/(static_cast<float>(numExam))) );
  }
  m_ClassVarian.insert(std::make_pair(clsName,vec));
}

/**
 * Normalize the loaded DataSet using the computed Var(X).
 */
void PndLVQTrain::NormalizeWithVariance()
{
  std::cout << "\t<INFO> Normalizing the dataset "
	    << "using samle Variance. "
	    << std::endl;
  // Loop through the classes
  for(unsigned int ev = 0; ev < m_EventsData.size(); ev++)
  {
    std::string className = m_EventsData[ev].first;
    // Loop Through parameters
    for(unsigned int i = 0; i < m_VarNames.size(); i++)
      {//FIXME FIXME
      (m_EventsData[ev].second)->at(i) = (m_EventsData[ev].second)->at(i) / (100.0 * (m_ClassVarian[className])->at(i));
    }
  }
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
  /* Open out put file and write coordinates of the prototypes */
  TFile* out = new TFile(outPut,"RECREATE");
  
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++)
  {
    std::vector<float> vars(m_VarNames.size(), 0.0);
    std::vector<float> Variance(m_VarNames.size(), 0.0);

    std::string name = m_ClassNames[cls];
    std::string Variance_name = m_ClassNames[cls] + "Var";

    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* Variance_treeName = Variance_name.c_str();

    const char* treeDesc = desc.c_str();
    desc += "Var";
    const char* Var_Desc = desc.c_str();

    // Create a tree
    TTree sig (treeName, treeDesc);
    TTree PropTree (Variance_treeName, Var_Desc);
    
    // Create branches and bind the variables
    for(unsigned int j = 0; j < m_VarNames.size(); j++)
    {
      std::string vname = m_VarNames[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();

      // Bind the parameters to the tree elements.
      sig.Branch(bname, &vars[j], lname);
      PropTree.Branch(bname, &Variance[j], lname);
    }
    // Fill The prototypes tree
    for(unsigned int i = 0; i< m_LVQProtos.size(); i++)
    {
      if(m_LVQProtos[i].first == name)
      {
	for(unsigned int k = 0; k < vars.size(); k++)
	{
	  vars[k] = (m_LVQProtos[i].second)->at(k);
	}
	sig.Fill();
      }
    }
    // Fill the properties tree.
    for(unsigned int k = 0; k < (m_ClassVarian[name])->size(); k++)
    {
      Variance[k] = (m_ClassVarian[name])->at(k);
    }
    PropTree.Fill();
    // Write the created tree
    sig.Write();
    PropTree.Write();
  }
  // We are done. We can close the open file and delete the pointer
  out->Close();
  delete out;
}

/**
 * Write the normalized DataSet to the out-put file.
 * @param  outFile  File name to write to
 */
void PndLVQTrain::WriteDataToFile(const char* outPut)
{
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
  std::cout << "\t<INFO> Determining the features of the dataset." 
	    << std::endl;
  
  // Loop through the classes
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++)
  {
    int minIdx = m_ClassIndex[cl].first;
    int maxIdx = m_ClassIndex[cl].second;
    float median = 0.00; float Fquartil = 0.00;
    
    // We need to determine the median and inter-quartile distances
    // per feature and per class
    std::vector <float>* clsMedian  = new std::vector <float>();
    std::vector <float>* clsQrtlDist = new std::vector <float>();
    
    // Loop through the Variables
    for(unsigned int i = 0; i < m_VarNames.size(); i++)
    {
      std::vector <float> varVect;
      
      // Loop through the Examples
      for(int k = minIdx; k < maxIdx; k++)
      {
	varVect.push_back( (m_EventsData[k].second)->at(i) );
      }
      
      // Sort the variables
      std::sort(varVect.begin(),varVect.end());
      
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
      
      clsMedian->push_back(median);
      clsQrtlDist->push_back((median - Fquartil));
    }
    m_ClsMedianQrtlDis.push_back(std::make_pair(m_ClassNames[cl], 
						std::make_pair(clsMedian, clsQrtlDist)));
  }
}

/**
 * Normalize the loaded DataSet using Median and inter-quartile
 * distance.
 */
void PndLVQTrain::NormalizeWithMedian()
{
  std::cout << "\t<INFO> Normalizing the dataset "
	    << "using Median and Inter Quartile Distance."
	    << std::endl;
  
  // Loop through the classes
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++)
  {
    int minIdx = m_ClassIndex[cl].first;
    int maxIdx = m_ClassIndex[cl].second;
    
    // Check if the class names are equal.
    if(m_ClsMedianQrtlDis[cl].first != m_ClassNames[cl])
    {
      std::cerr << "Index out of bound. Unknown class name <Normalization LVQ>" 
		<< std::endl;
      abort();
    }
    std::vector <float>* Median   = (m_ClsMedianQrtlDis[cl].second).first;
    //std::vector <float>* QrtlDist = (m_ClsMedianQrtlDis[cl].second).second;
    
    // Loop through the Examples
    for(int k = minIdx; k < maxIdx; k++)
    {
      // Loop through the Variables
      for(unsigned int i = 0; i < m_VarNames.size(); i++)
      {
	//(m_EventsData[k].second)->at(i) = ( (m_EventsData[k].second)->at(i) - Median->at(i))/(QrtlDist->at(i));
	(m_EventsData[k].second)->at(i) = ( (m_EventsData[k].second)->at(i))/ (Median->at(i));
      }
    }// END Loop through the Examples
  }// End of loop through classes
}
