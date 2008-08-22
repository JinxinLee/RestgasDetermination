#include "PndLVQTrain.h"

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
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    // Tree name
    const char *name = m_ClassNames[cls].c_str();
    
    //Get the tree object
    TTree *t = (TTree*) m_InPutFile->Get(name);
    
    // Init a container to bind to the tree branches
    std::vector<float> ev (m_VarNames.size(),0.0);
    
    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < m_VarNames.size(); j++){
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
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++){
      t->GetEntry(k);
      
      // Container to store the vent data read from the input tree
      std::vector<float>* EvtDat = new std::vector<float>();
      
      for(unsigned int idx = 0; idx < m_VarNames.size(); idx++){
	EvtDat->push_back(ev[idx]);
      }
      
      //Store the event and its class name
      m_EventsData.push_back(std::make_pair(m_ClassNames[cls], EvtDat));
    }
    
    //We are done and can delete the tree pointer
    delete t;
  }//End of for(cls) loop for all classes
  
  //Close the open file and delete the file pointer
  m_InPutFile->Close();
  delete m_InPutFile;
  
  // Compute the class conditional means for each class of signal
  for(unsigned int idx = 0; idx < m_ClassNames.size(); idx++){
    CompClsCondMean(m_ClassNames[idx]); 
  }//End of classconditional mean
  
}// End of constructor

PndLVQTrain::~PndLVQTrain()
{
  // Clean up the container for class Conditional means
  for(unsigned int i = 0; i < m_ClassCondMeans.size(); i++){
    delete m_ClassCondMeans[i].second;
  }
  m_ClassCondMeans.clear();
  
  // Clean up the container for event data
  for(unsigned int i = 0; i < m_EventsData.size(); i++){
    delete m_EventsData[i].second;
  }
  m_EventsData.clear();
  
  // Clean up the container for proto-types
  for(unsigned int k = 0; k < m_LVQProtos.size(); k++){
    delete m_LVQProtos[k].second;
  }
  m_LVQProtos.clear();
  
  m_perClsExamples.clear();
  m_ClassNames.clear();
  m_VarNames.clear();
  m_ClassIndex.clear();
}

void PndLVQTrain::CompClsCondMean(std::string clsName)
{
  std::vector <float>* vec = new std::vector <float> (m_VarNames.size(),0.0);
  for(unsigned int i = 0; i < m_EventsData.size(); i++){
    if( clsName == m_EventsData[i].first ){
      for(unsigned int j = 0; j < m_VarNames.size(); j++ ){
	vec->at(j) += (m_EventsData[i].second)->at(j);
      }
    }
  }// Here we have seen all available events
  
  /* 
   * Normalizing by dividing by the total number of available examples
   * for the current class
   */
  int numExam = m_perClsExamples[clsName];
  for(unsigned int k = 0; k < vec->size(); k++){
    vec->at(k) = (vec->at(k)/(float(numExam)));
  }
  // Add to the Class Conditional Means container
  m_ClassCondMeans.push_back(std::make_pair(clsName,vec));
}

void PndLVQTrain::Train(int numProto, const char* outPut)
{
  // Initialize LVQ-prototypes according to the classconditional means
  double c = 0.8;
  TRandom3 trand(4357);
  
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++){
    int minIdx = m_ClassIndex[cl].first;
    int maxIdx = m_ClassIndex[cl].second;
    
    for(int i = 0; i < numProto; i++){
      // select a random example
      if(minIdx == 0){minIdx = 1;}
      int index = (int) (trand.Poisson( (float)((maxIdx + minIdx)/2))) % (maxIdx);
      // std::cout << index << "\n";// DEBUG DEBUG DEBUG
      
      if(index < minIdx){
	index += minIdx - index;
	//std::cout << index << " \n";// DEBUG DEBUG DEBUG
      }
      if(index > maxIdx){
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
      
      // DEBUG DEBUG DEBUG
      if(m_ClassNames[cl] != m_EventsData[index].first){
	std::cout << "\n Name collision cls is " << m_ClassNames[cl]
		  << " index is " << index << " minindex " << minIdx
		  << std::endl;
      }
      
      for(unsigned int k = 0; k < evtData->size(); k++){
	proto->at(k) = evtData->at(k) * c + (1.0 - c) * evtData->at(k);
      }
      //proto type is initialized, add to the container
      m_LVQProtos.push_back(std::make_pair(m_EventsData[index].first, proto));
      // std::cout << m_EventsData[index].first << " " << std::endl;
    }
  }
  // All protypes are initialized. We can perform the training
  // Compute learning rate constant "a"
  double ethaZero  = 0.1;
  double ethaFinal = 0.001;
  int    numSweep  = 100;
  int    tFinal    = numSweep * ( m_EventsData.size() );
  double a         = (ethaZero - ethaFinal)/(ethaFinal * (double)tFinal);
  
  for(int time = 0; time < tFinal; time++){
    int    protoIndex       = 0;
    double distance         = 0.0;
    double minProtoDistance = 100000.0;
    
    double ethaT = (ethaZero) / (1.0 + (a * (double)time));
    // select a random example
    int index = (int) trand.Poisson( (time + 2) * 10000) % (m_EventsData.size() - 1);
    
    // Compute the distance to all available LVQ proto-types
    for(unsigned int ix = 0; ix < m_LVQProtos.size(); ix++){
      distance = ComputeDist( *(m_EventsData[index].second), *(m_LVQProtos[ix].second) );
      
      if(distance < minProtoDistance){
	minProtoDistance = distance;//minimum distance
	protoIndex  = ix;//index of the prototype with min dist
      }
    }
    //We need to update the (winner) prototype
    int delta = 0;
    //determine delta
    if( m_EventsData[index].first == m_LVQProtos[protoIndex].first ){
      delta = 0;
    }
    else{
      delta = 1;
    }//delta is calculated
    
    // Update the LVQ prototype
    UpdateProto( *(m_EventsData[index].second), *(m_LVQProtos[protoIndex].second), delta, ethaT);
  }
  //Write the coordinates of the prototypes to the file
  WriteToFile(outPut);
}

void PndLVQTrain::UpdateProto( std::vector<float> &EvtData, std::vector<float> &proto, 
			       int delta, double ethaT )
{
  for(unsigned int i = 0; i < proto.size(); i++){
    proto[i] = proto[i] + ethaT * (1.0 - 2.0 * (double)delta) * (EvtData[i] - proto[i]);
  }
}
/* 
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float PndLVQTrain::ComputeDist(std::vector<float> &EvtData, 
			       std::vector<float> &Ex)
{
  float dist = 0.0;
  
  for(unsigned int i = 0; i< Ex.size(); i++){
    dist += (EvtData[i] - Ex[i]) * (EvtData[i] - Ex[i]);
  }
  return dist;
}

void PndLVQTrain::WriteToFile(const char* outPut)
{
  /* Open out put file and write coordinates of the prototypes */
  TFile* out = new TFile(outPut,"RECREATE");

  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    std::vector<float> vars(m_VarNames.size(),0.0);

    std::string name = m_ClassNames[cls];
    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();

    //Create a tree
    TTree sig (treeName, treeDesc);
    
    //Create branches and bind the variables
    for(unsigned int j = 0; j < m_VarNames.size(); j++){
      std::string vname = m_VarNames[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      //Bind the parameters to the tree elements.
      sig.Branch(bname,&vars[j],lname);
    }
    //Fill The tree
    for(unsigned int i = 0; i< m_LVQProtos.size(); i++){
      if(m_LVQProtos[i].first == name){
	for(unsigned int k = 0; k < vars.size(); k++){
	  vars[k] = (m_LVQProtos[i].second)->at(k);
	}
	sig.Fill();
      }
    }
    //Write the created tree
    sig.Write();
  }  
  // We are done. We can close the open file and delete the pointer
  out->Close();
  delete out;
}

int main(int argc, char** argv)
{
  TStopwatch timer;
  timer.Start();
  
  // ==============================
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  
  // Classes
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  // Variables
  nam.push_back("ep");  nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f"); nam.push_back("d");
  nam.push_back("a");  nam.push_back("b"); nam.push_back("c");
  
  PndLVQTrain bla("TestInput10000.root",clas,nam);
  bla.Train(30,"OutTestPut.root");
  bla.PrintIndex();
  // =============================
  
  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;
  
  return 0;
}
