/* ***************************************
 * KNN based Classifier using kd-tree    *
 * data structure for better recognition *
 * performance.                          *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version 1.0 beta1.                    *
 * ***************************************
 */
#ifndef PndKnnClassify_H
#define PndKnnClassify_H

// Standard C++ libraries
#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>

// Root includes
#include "TFile.h"
#include "TTree.h"

// kd-Tree and boost includes
#include "kdtree2.hpp"
#include <boost/multi_array.hpp>

/* 
 * Two dimensional array of floats to sotore input data before
 * generating the search tree.
*/
typedef multi_array<float,2> array2dfloat;

/* 
 * Dummy object to hold the distances computed during classification
 */
class DistObject{

 public:
  
 DistObject(): m_cls(""), m_dist(0.0){};
 DistObject(float d, std::string c): m_cls(c), m_dist(d){};
  ~DistObject(){};
  
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
  //Class names container 
  std::vector<std::string> m_ClassNames;
  //variable names container
  std::vector<std::string> m_VarNames;
  // contains distances labled for every class
  std::vector<DistObject*> m_dists;
  //Container to store the optimized per class trees
  std::vector< std::pair<std::string, kdtree2*> > m_EventTreeCont;
  //Pairs to hold the number of available examples per calss
  std::map< std::string, int > m_perClassExamples;
};
#endif
