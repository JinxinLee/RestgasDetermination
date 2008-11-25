/* **************************************
 * KNN based classifier training class. *
 * Author: M.Babai@rug.nl               *
 * LICENSE:                             *
 * Version 1.0 beta1.                   *
 * **************************************
 */
#pragma once
#ifndef PndKnnTrain_H
#define PndKnnTrain_H

// Standard C++ libraries
//#include <sstream>
#include <iostream>
#include <vector>

// Root includes
#include "TFile.h"
#include "TTree.h"

class PndKnnTrain{
 public:
  //Constructors and destructor definitions
  
  /*
   * @param OutPut: The file name to hold the weight values.
   * @param ClassNames: Name of classes for wich the classifier is
   * trained.
   * @param VarNames: The name of variables creating the feature vector.
   */
  PndKnnTrain(const char *OutPut, 
	      const std::vector<std::string>& ClassNames, 
	      const std::vector<std::string>& VarNames);
  /* Class destructor */
  virtual ~PndKnnTrain();
  
  /* Train the classifier */
  void Train(const std::vector<float> &EvtData, const std::string cls);
  
 protected:
  
 private:
  // Variables
  TFile *m_OutPutF;
  int   m_numClass;
  int   m_numVars;
  
  std::vector<TTree *> m_SigTrees;
  std::vector< std::vector<float>* > m_varContainer;
  std::vector<std::string> m_ClassNames;
};
#endif
//end of interface definition
