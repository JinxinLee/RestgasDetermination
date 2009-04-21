#include "PndKnnClassify.h"

/**
 * @param InputPutFile: File that holds the weights
 * @param ClassNames: Class names.
 * @param VarNames: Variable names from which the feature vector is
 * built.
 */
PndKnnClassify::PndKnnClassify(const char *InputPutFile,
			       const std::vector<std::string>& ClassNames, 
			       const std::vector<std::string>& VarNames)
{
  m_InPutF = new TFile(InputPutFile,"READ");
  
  if(!InputPutFile){
    std::cerr << "<Error> Could not open inputfile."
	      << std::endl;
  }
  
  //Initialize TMVA KNN module (everything is done by this module.)
  m_module = new TMVA::kNN::ModulekNN();
  
  //Copy the class and variable names.
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;
  
  // Set the deafult value for scale factor
  m_ScaleFact = 0.8;
  Init();
}

PndKnnClassify::~PndKnnClassify()
{
  m_ClassNames.clear();
  m_VarNames.clear();
  
  m_perClassExamples.clear();
  
  if(m_InPutF){
    m_InPutF->Close();
    delete m_InPutF;
  }
  if(m_module){
    m_module->Clear();
    delete m_module;
  }
}

void PndKnnClassify::Init()
{
  std::cout << "<INFO> Initializing Multi class KNN." << std::endl;
  // Fetch the class trees and read the event data.
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    // Get tree
    const char *clsname = m_ClassNames[cls].c_str();
    TTree *t = (TTree*) m_InPutF->Get(clsname);
    
    // Init a container to bind to the tree branches
    std::vector<float> evtDat (m_VarNames.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < m_VarNames.size(); j++){
      const char *bname = m_VarNames[j].c_str();
      t->SetBranchAddress(bname, &(evtDat[j]));
    }// Tree parameters are bounded
    
    // Get number of available examples for the current class and
    // store the values.
    const unsigned int NumExamples = t->GetEntriesFast();
    
    std::pair<std::string, int>* ClNum = new std::pair<std::string, int> 
      (m_ClassNames[cls], cls);
    
    m_perClassExamples.insert(std::make_pair(ClNum, NumExamples));
    
    // Fetch and add the variables to the module
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++){
      t->GetEntry(k);
      
      // Create and add the current event. 
      TMVA::kNN::Event Evt(evtDat, m_weight, cls);
      m_module->Add(Evt);
    }
    //Done, delete tree ptr
    //delete t;
  }// Values from the (ROOT)trees are inserted
  
  std::cout << ".......... <INFO> "
	    << "Number of available classes is "
	    << m_ClassNames.size() << std::endl;
  
  // Fill module and optimize
  m_module->Fill( static_cast<unsigned int> (m_VarNames.size()),
  		  static_cast<unsigned int> (100.0 * m_ScaleFact),
  		  "");//"metric"
  
  /*
    m_module->Fill(static_cast<unsigned int> (12),
    static_cast<unsigned int> (0),
    "");
  */
  // Close the open file.
  m_InPutF->Close();
  //std::cout << "length is " << m_perClassExamples.size() << std::endl;
}

/**
 * @param InputPutFile: File that holds the weights
 * @param ClassNames: Class names.
 * @param VarNames: Variable names from which the feature vector is
 * built.
 */
void PndKnnClassify::Classify(const std::vector<float> &EvtData, 
			      const unsigned int Neighbours,
			      std::map<std::string, float>& result)
{
  
  result.clear();
  std::map<int, int> PerClsCount;
  
  // Initialize results
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    result.insert( std::make_pair( m_ClassNames[cls], 0.0 ) );
    PerClsCount.insert( std::make_pair(cls, 0));
  }
  
  // Create event and fetch Neighbours events from the tree
  TMVA::kNN::Event evt(EvtData, m_weight, 20);
  m_module->Find(evt, Neighbours);
  
  // Fetch the results list
  
  ResList lst = m_module->GetkNNList();
  
  ResList::iterator iter;
  for(iter  = lst.begin(); iter != lst.end(); ++iter){
    // Fetch the node
    const TMVA::kNN::Node<TMVA::kNN::Event> *node = (*iter).first;
    
    // Fetch the event from the Node
    TMVA::kNN::Event event = node->GetEvent();
    
    // Type corresponds with the class name
    int type = event.GetType();
    
    // Store per class counts in the search results
    PerClsCount[type] += 1;
  }
  
  // Map the results
  std::map< std::pair<std::string, int>*, int>::iterator it = 
    m_perClassExamples.begin();
  
  int cnt = 0;
  std::vector<int> counts(m_perClassExamples.size(), 0);
  
  while(it != m_perClassExamples.end()){
    std::pair<std::string, int>* pr = (*it).first;
    counts[cnt] = (*it).second;
    
    //
    // int clsNumber = pr->second;
    // std::string clName = pr->first;
    // result[clName] = PerClsCount[clsNumber];
    // 
    result[pr->first] = PerClsCount[pr->second];
    it++;
    cnt++;
  }
  
  // Normalizing the results
  float probSum = 0.0;
  for(unsigned int i = 0; i < result.size(); i++){
    std::string className = m_ClassNames[i];
    int num = counts[i];
    result[className] = result[className]/static_cast<float>(num);
    probSum += result[className];
  }
  
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++){
    std::string className = m_ClassNames[cl];
    result[className] = result[className]/probSum;
  }
  
  // CLean-up
  PerClsCount.clear();
  counts.clear();
}

// ===============  Protected functions ================

/*
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float PndKnnClassify::ComputeDist(const std::vector<float> &EvtData, 
				  const std::vector<float> &Ex)
{
  float dist = 0.0;
  
  for(unsigned int i = 0; i< Ex.size(); i++){
    dist += (EvtData[i] - Ex[i]) * (EvtData[i] - Ex[i]);
  }
  return sqrt(dist);
}

// ===============  Private functions ================
