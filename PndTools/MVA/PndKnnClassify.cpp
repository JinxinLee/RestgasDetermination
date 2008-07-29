/* ***************************************
 * KNN based classifier                  *
 * Author: M.Babai@rug.nl                *
 * Version 1.0 beta1.                    *
 * ***************************************
 */
#include "PndKnnClassify.h"

/* Function used for sorting the distances container */
bool LessFunct(DistObject* p1, DistObject* p2)
{  
  if(p1->m_dist < p2->m_dist)
  {
    return true;
  }
  return false;
}

/*
 * @param InputPutFile: The name of the file that holds the weits
 * @param ClassNames: The names of classes to which an event might be
 * assigned to.
 * @param VarNames: Variable names from which the feature vector is
 * built.
 */
PndKnnClassify::PndKnnClassify(const char *InputPutFile,
			       const std::vector<std::string>& ClassNames, 
			       const std::vector<std::string>& VarNames)
{
  // Initialize the input file pointer
  m_InPutF = new TFile(InputPutFile,"READ");
  
  //Copy the class and var names.
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;
  
  // Fetch the class trees and read the event data.
  for(unsigned int i = 0; i < m_ClassNames.size();i++){
    const char *name = m_ClassNames[i].c_str();
    
    TTree *t = (TTree*) m_InPutF->Get(name);

    // Init a container to bind to the tree branches
    std::vector<float> ev (m_VarNames.size(),0.0);
    
    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < m_VarNames.size(); j++){
      const char *bname = m_VarNames[j].c_str();

      t->SetBranchAddress(bname, &(ev[j]));
    }// Tree parameters are bounded
    
    //Container to store the vent data read from the input tree
    std::vector<float>* EvtDat = new std::vector<float>();
    
    // Fetch and store the variables to per class variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++){
      t->GetEntry(k);
      
      for(unsigned int idx = 0; idx < m_VarNames.size(); idx++){
	EvtDat->push_back(ev[idx]);
      }
    }
    //Add class name and event data to the container
    m_EventVarCont.push_back(std::make_pair(m_ClassNames[i], EvtDat));

    //We are done and can delete the tree pointer
    delete t;
  } // Values from the trees are inserted into the container
  
  /*
    std::pair< std::string, std::vector<float>* > bla = m_EventVarCont[0];
    std::cout << "Class name is " << bla.first
    << " and the length is " << (bla.second)->size()
    << std::endl;
    std::cout << m_EventVarCont.size() << std::endl;
  */

  // Close the open file.
  m_InPutF->Close();
  delete m_InPutF;
}

PndKnnClassify::~PndKnnClassify()
{
  m_ClassNames.clear();
  m_VarNames.clear();
  
  for(unsigned int i = 0; i < m_EventVarCont.size(); i++){
    delete (m_EventVarCont[i]).second;
  }
  m_EventVarCont.clear();

  for(unsigned int i = 0; i < m_dists.size(); i++){
    delete m_dists[i];
  }
  m_dists.clear();
}

/* 
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float PndKnnClassify::ComputeDist(std::vector<float> &EvtData, 
				  std::vector<float> &Ex)
{
  float dist = 0.0;
  
  for(unsigned int i = 0; i< Ex.size(); i++){
    dist += (EvtData[i] - Ex[i]) * (EvtData[i] - Ex[i]);
  }
  return dist;
}

/*
 * @param InputPutFile: The name of the file that holds the weits
 * @param ClassNames: The names of classes to which an event might be
 * assigned to.
 * @param VarNames: Variable names from which the feature vector is
 * built.
 */
void PndKnnClassify::Classify(std::vector<float> &EvtData, 
			      unsigned int Neighbours,
			      std::map<std::string, float>& result)
{
  result.clear();
  // Holds the number of available examples per class
  std::vector<int> perClsExamples;
  
  // Initialize perClsExamples
  for(unsigned int i = 0; i < m_ClassNames.size(); i++){
    int numExamples = ((m_EventVarCont[i]).second)->size()/m_VarNames.size();
    perClsExamples.push_back(numExamples);
  }

  //// Initialize results
  for(unsigned int id = 0; id < m_ClassNames.size(); id++){
    result.insert( make_pair( m_ClassNames[id], 0.0 ) ); 
  }
  
  // Now we need to compute distances and store the results in order
  // to do classification
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    
    std::vector<float>* evtVals = (m_EventVarCont[cls]).second;
    
    // We need to read in chunks of m_VarNames.size()
    unsigned int begin = 0; 
    int chunk = m_VarNames.size();
    std::vector<float> vals;
    
    while(begin < evtVals->size()){
      vals.clear();
      
      for(unsigned int k = begin; k < (begin + chunk); k++){
	vals.push_back(evtVals->at(k));
      }
      //std::cout << "length is " << vals.size() << std::endl;  
      begin += chunk;
      
      float dist = ComputeDist(EvtData,vals);
      DistObject* ds = new DistObject(dist,m_ClassNames[cls]);
      m_dists.push_back(ds);
    }// WHILE
  }//For CLS
  
  //All distances are determined, now we can classify
  sort(m_dists.begin(), m_dists.end(), LessFunct);
  
  if (Neighbours > m_dists.size()){
    std::cout << "<ERROR> Requested number of Neighbours is too large"
	      << std::endl;
    return;
  }//IF
  
  for(unsigned int id = 0; id < Neighbours; id++){
    DistObject* dOb = m_dists[id];//take the distance object
    std::string clas = dOb->m_cls;//Find the Object class
    //Increment the number of objects found for a certain class
    result[clas] += 1.0;
  }
  
  //Normalizing the results
  float Psum = 0.0;
  for(unsigned int icl = 0; icl < result.size(); icl++){
    int num = perClsExamples[icl];
    std::string className = m_ClassNames[icl];
    result[className] = result[className]/num;
    Psum += result[className];
  }

  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++){
    std::string className = m_ClassNames[cl];
    result[className] = result[className]/Psum;
  }
  
  std::cout << Psum << std::endl;
  for( std::map<std::string,float>::iterator ii=result.begin(); 
       ii != result.end(); ++ii){
    std::cout << (*ii).first << ": " << (*ii).second << std::endl;
    
  }
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */
int main(int argc, char** argv)
{
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  // Classes
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");

  // Variables
  nam.push_back("ep");  nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f"); nam.push_back("d");
  nam.push_back("a");  nam.push_back("b"); nam.push_back("c");

  PndKnnClassify cls ("Test.root",clas,nam);
  
  //std::vector<float> evt(9,5.0);
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
  
  cls.Classify(evt,1500000,res);
  return 0;
}
