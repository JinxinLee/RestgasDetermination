/* ***************************************
 * LVQ  Classifier                       *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version 1.0 beta1.                    *
 * ***************************************
 */
#include "PndLVQClassify.h"

/**
 * Constructor:
 * @param InPut, Input file name (Weights).
 * @param ClassNames, class names.
 * @param VarNames, variable names of the features.
 */
PndLVQClassify::PndLVQClassify(const char* InPut,
			       const std::vector<std::string>& ClassNames, 
			       const std::vector<std::string>& VarNames)
{
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;

  // Open the input file for reading the protypes data.
  TFile* InPutFile = new TFile(InPut,"READ");
  
  //Read the proto types and store. 
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    // Tree name
    const char *name = m_ClassNames[cls].c_str();

    //Get the tree object
    TTree *t = (TTree*) InPutFile->Get(name);
    
    // Init a container to bind to the tree branches
    std::vector<float> proto (m_VarNames.size(),0.0);
    
    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < m_VarNames.size(); j++){
      const char* branchName = m_VarNames[j].c_str();
      
      //Binding the branches
      t->SetBranchAddress(branchName, &(proto[j]));
    }// Tree parameters are bounded
    
    // Fetch and store the variables to variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++){
      t->GetEntry(k);
      
      // Container to store the vent data read from the input tree
      std::vector<float>* EvtDat = new std::vector<float>();
      
      for(unsigned int idx = 0; idx < m_VarNames.size(); idx++){
	EvtDat->push_back(proto[idx]);
      }
      
      //Store the event and its class name
      m_protoContainer.push_back(std::make_pair(m_ClassNames[cls], EvtDat));
    }//End of tree loop

    delete t;
  }// End of for cls

  //Close the open file and delete the file pointer
  InPutFile->Close();
  delete InPutFile;
}//End of constructor

/**
 * Destructor
 */
PndLVQClassify::~PndLVQClassify()
{
  for(unsigned int i = 0; i < m_protoContainer.size(); i++){
    delete (m_protoContainer[i]).second;
  }
  m_protoContainer.clear();
  m_ClassNames.clear();
  m_VarNames.clear();
}
  
/**
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float PndLVQClassify::ComputeDist(std::vector<float> &EvtData, 
				  std::vector<float> &proto)
{
  float dist = 0.0;
  
  for(unsigned int i = 0; i< proto.size(); i++){
    dist += (EvtData[i] - proto[i]) * (EvtData[i] - proto[i]);
  }
  return dist;
}

/**
 * @param EvtData. Event data to be classified.
 * @param result.  Classification results. Currently the shortest
 *     distance for each class is stored in result.
 */
void PndLVQClassify::Classify(std::vector<float> &EvtData, 
			      std::map<std::string,float>& result)
{
  float dist = 0.0;
  result.clear();
  // Initialize results
  for(unsigned int id = 0; id < m_ClassNames.size(); id++){
    result.insert( make_pair( m_ClassNames[id], std::numeric_limits<float>::max()) );
  }
  
  // Loop trough the prototypes list and compute the distances
  for(unsigned int i = 0; i < m_protoContainer.size(); i++){
    std::string clsName = m_protoContainer[i].first;
    std::vector<float>* ev = (m_protoContainer[i]).second;
   
    dist = ComputeDist(*ev, EvtData);
    
    if(dist < result[clsName]){
      result[clsName] = dist;
    }
  }
}
