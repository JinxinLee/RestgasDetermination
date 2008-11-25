#ifndef PndKnnClassify_H
#define PndKnnClassify_H

// Standard C++ libraries
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

//OPENMP header
//#include <omp.h>

// Root includes
#include "TFile.h"
#include "TTree.h"
#include "TList.h"

/* 
 * Dummy object to hold the distances computed during classification
 */
class DistObject{
 public:
  //Constructors
 DistObject(): m_cls(""), m_dist(0.0){};
 DistObject(float d, std::string c): m_cls(c), m_dist(d){};
  
  //Destructor
  ~DistObject(){};
  
  //Operators
  inline bool operator < (const DistObject &other)const
  {
    return (m_dist < other.m_dist);
  }
  
  inline bool operator>(const DistObject &other)const
  {
    return (m_dist > other.m_dist);
  }
  
  //Local members. 
  std::string m_cls;// Class name
  float       m_dist;//Computed distance
};

/*
 * KNN based classification alg. implementation.
 */
class PndKnnClassify{
 public:
  /*
   * @param InputPutFile: The name of the file that holds the weits
   * @param ClassNames: The names of classes to which an event might be
   * assigned to.
   * @param VarNames: Variable names from which the feature vector is
   * built.
   */
  PndKnnClassify(const char *InputPutFile,
		 const std::vector<std::string>& ClassNames, 
		 const std::vector<std::string>& VarNames);
  
  virtual ~PndKnnClassify();
  /* 
   * Classification function.
   * @param EvtData: Feature vector of the current event which we want
   * to classify.
   * @param Neighbours: Number of Neighbours.
   * @param result: Holds the normalized results of classification
   * for every class of events.
   */
  void Classify(std::vector<float> &EvtData, 
		unsigned int Neighbours, 
		std::map<std::string,float>& result);
  
 protected:
  /* 
   * Computes the Euclidean distance between two given vectors of
   * event features.
   */
  float ComputeDist(std::vector<float> &EvtData, 
		    std::vector<float> &Example);
  
 private:
  //Weight file pointer
  TFile *m_InPutF;
  //Class names container 
  std::vector<std::string> m_ClassNames;
  //variable names container
  std::vector<std::string> m_VarNames;
  // For every class of events, stores all of the available examples
  std::vector< std::pair<std::string, std::vector<float>* > > m_EventVarCont;
  // contains distances labled for every class
  std::vector<DistObject*> m_dists;
};

/* Function used for sorting the distances container */
inline bool LessFunct(const DistObject& p1, const DistObject& p2)
{  
  return (p1.m_dist < p2.m_dist);
}
#endif
