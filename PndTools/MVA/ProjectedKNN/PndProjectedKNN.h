#pragma once
#ifndef PNDPROJECTEDKNN_H
#define PNDPROJECTEDKNN_H

// C++ headers
#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

// PANDA Headers
#include "PndKnnClassify.h"

class PndProjectedKNN{
  //Public methods and vars
 public:
  // Constructor
  PndProjectedKNN(const char* InputPutFile,
		  const std::string& sharedVar,
		  const std::vector<std::string>& ClassNames,
		  const std::vector<std::string>& VarNames);
  // Destructor
  virtual ~PndProjectedKNN();

  void Classify(const std::vector<float> &EvtData,
                const unsigned int Neighbours,
                std::map<std::string, float>& result);
  
  inline void SetEvtParam(const float scFact, const double weight)
  {m_ScaleFact = scFact; m_weight = weight; };

  void InitKNN();

  //Protected methods and vars
  //protected:
  //Private methods and vars
 private:
  // Private by design
  PndProjectedKNN();
  void destroy();
  void CreateParPairs();
 
  std::vector<PndKnnClassify*> m_classifiers;
  // First:variable names, Second:var indices.
  std::vector<std::pair<std::vector<std::string>*, std::vector<int>*>*> m_pairContainer;
  std::string m_inputFile;
  std::string m_sharedVar;
  std::vector<std::string> m_ClassNames;
  std::vector<std::string> m_VarNames;
  float  m_ScaleFact;
  double m_weight;
};
#endif//End, class interface
