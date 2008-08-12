#ifndef PndLVQTrain_H
#define PndLVQTrain_H

// Standard C++ libraries
#include <iostream>
#include <vector>
#include <map>

//OPENMP header
//#include <omp.h>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"
#include "TStopwatch.h"

class PndLVQTrain{
 public:
  PndLVQTrain(const char *InPut,
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames);

  virtual ~PndLVQTrain();
  
  void Train(int numProto, const char* OutPut);

  // +++++++++++++ =======================  
  /// TEST ZOOOOIIII MAG WEG 
  void PrintIndex(){
    for(unsigned int id = 0; id < m_ClassIndex.size(); id++){
      std::cout << "min index = " << m_ClassIndex[id].first
		<< " max index = "<< m_ClassIndex[id].second
		<< std::endl;
    }
    std::cout << "Total length is " << m_EventsData.size() << std::endl;
    std::cout << "The length of proto is " << m_LVQProtos.size() << std::endl;
  }
  /// TEST ZOOOOIIII MAG WEG 
  // +++++++++++++ =======================  

 // Protected functions and variables
 protected:
  /* 
   * Computes the Euclidean distance between two given vectors of
   * event features.
   */
  float ComputeDist(std::vector<float> &EvtData, 
		    std::vector<float> &Example);
  
  void UpdateProto( std::vector<float> &EvtData, std::vector<float> &proto, int delta, double ethaT);

 // Private functions and variables
 private:
  /* 
   * Compute the class conditional mean for a given class and store
   * that in the class conditional means container
   */
  void CompClsCondMean(std::string clsName);
  
  void WriteToFile(const char* outFile);

  // Class names
  std::vector<std::string> m_ClassNames;
  
  // Variable Names
  std::vector<std::string> m_VarNames; 
  
  //Container to keep  the Class Conditional means
  std::vector<std::pair< std::string, std::vector<float>*> > m_ClassCondMeans;
  
  //Container to keep  the Event data feature vectors
  std::vector< std::pair<std::string, std::vector<float>*> > m_EventsData;
  
  // Holds the number of available examples per class
  std::map<std::string, int> m_perClsExamples;
  
  //Holds the start and the end index of events from each class
  std::vector< std::pair<int,int> > m_ClassIndex;

  //Holds the LVQ proto-types
  std::vector< std::pair<std::string, std::vector<float>*> > m_LVQProtos;
};
#endif
//end of interface definition
