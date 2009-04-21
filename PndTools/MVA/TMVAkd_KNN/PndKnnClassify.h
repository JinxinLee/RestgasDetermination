#pragma once
#ifndef PndKnnClassify_H
#define PndKnnClassify_H

// Standard C++ libraries
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

// Root includes
#include "TFile.h"
#include "TTree.h"

// TMVA
#include "TMVA/NodekNN.h"
#include "TMVA/ModulekNN.h"

typedef std::list < std::pair<const TMVA::kNN::Node<TMVA::kNN::Event>*, 
  float> > ResList;

/**
 * KNN based classification alg. implementation.
 */
class PndKnnClassify{
  
 public:
  /**
   * Constructor.
   * @param InputPutFile: File that holds the weights
   * @param ClassNames: Class names.
   * @param VarNames: Variable names from which the feature vector is
   * built.
   */
  PndKnnClassify(const char *InputPutFile,
		 const std::vector<std::string>& ClassNames, 
		 const std::vector<std::string>& VarNames);
  //! Destructor
  virtual ~PndKnnClassify();
  
  /**
   * Classification function.
   * @param EvtData: Feature vector of the current event.
   * @param Neighbours: Number of Neighbours.
   * @param result: Holds the normalized results of classification
   */
  void Classify(const std::vector<float> &EvtData, 
		const unsigned int Neighbours, 
		std::map<std::string, float>& result);
  /**
   * Set the scalefactor and the event weight for KNN classifier.
   * @param scFact  Scale factor.
   * @param weight  Events weight.
   */
  inline void SetEvtParam(const float scFact, const double weight)
  {m_ScaleFact = scFact; m_weight = weight; };
  
  
  /// DEBUG
  void print(){m_module->Print();}
  /// DEBUG
  // Protected functions and variables

 protected:
  /**
   * Euclidean distance between two given vectors of event features.
   */
  float ComputeDist(const std::vector<float> &EvtData, 
		    const std::vector<float> &Example);
  
  // Private functions and variables
 private:
  /**
   * Initialize the KNN classifier.
   */
  void Init();
  
  TFile* m_InPutF;
  float m_ScaleFact;//!< Scalefactor Default =  0.8
  double m_weight;//!< Event weight
  TMVA::kNN::ModulekNN* m_module;

  //! Class names container 
  std::vector<std::string> m_ClassNames;
  
  //! Variable names container
  std::vector<std::string> m_VarNames;
  
  /**
   * Pairs to hold the number of available examples per calss and to
   * map classNames to classNumbers.
   * std::pair<std::string, int> containes className and classNumber.
   * Second int = the number of events for a given class
   */
  std::map< std::pair<std::string, int>*, int> m_perClassExamples;
};
#endif// End of classifier definition
