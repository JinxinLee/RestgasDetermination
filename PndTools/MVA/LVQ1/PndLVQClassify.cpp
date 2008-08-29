#include "PndLVQClassify.h"

PndLVQClassify::PndLVQClassify(const char* InPut,
			       const std::vector<std::string>& ClassNames, 
			       const std::vector<std::string>& VarNames)
{
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;

  // Open the input file for reading the protypes data.
  TFile* InPutFile = new TFile(InPut,"READ");
  
  //Read the proto types and store. 
  for(unsigned int cls; cls < m_ClassNames.size(); cls++){
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

/*
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
  
/* 
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

/*
 *@param EvtData. Event data to be classified.
 *@param result.  Classification results.
 */
void PndLVQClassify::Classify(std::vector<float> &EvtData, 
			      std::map<std::string,float>& result)
{
  float dist = 0.0;
  result.clear();
  // Initialize results
  for(unsigned int id = 0; id < m_ClassNames.size(); id++){
    result.insert( make_pair( m_ClassNames[id], 0.0 ) );
  }
  
  //Loop trough the prototypes list and compute the distances
  for(unsigned int i = 0; i < m_protoContainer.size(); i++){
    std::vector<float>* ev = (m_protoContainer[i]).second;
    dist = ComputeDist(*ev, EvtData);
  }//FIXME FIXME
}

// Just for testing. You may want to remove this before use.
int main(int argc, char** argv){
  TStopwatch timer;
  timer.Start();
  //================================
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  
  // Classes
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  // Variables
  nam.push_back("ep");  nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f"); nam.push_back("d");
  nam.push_back("a");  nam.push_back("b"); nam.push_back("c");

  PndLVQClassify bla("OutTestPut.root",clas,nam);

  std::vector<float> evt;
  evt.push_back(5.0);
  evt.push_back(5.0);
  evt.push_back(5.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(15.0);
  evt.push_back(5.0);

  std::map<std::string,float> res;

  //timer.Start();
  bla.Classify(evt,res);

  //================================
  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;

  return 0;
}
