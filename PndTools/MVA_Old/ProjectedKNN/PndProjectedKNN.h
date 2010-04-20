#pragma once
#ifndef PNDPROJECTEDKNN_H
#define PNDPROJECTEDKNN_H

// C++ headers
#include <iostream>
#include <vector>
#include <map>
//#include <algorithm>

// PANDA Headers
#include "PndKnnClassify.h"

class PndProjectedKNN{
  //Public methods and vars
 public:
  // Constructor
  PndProjectedKNN(const char* InputPutFile,
		  const std::vector<std::string>& ClassNames,
		  const std::vector<std::string>& m_EvtStructure,
		  const std::vector< std::vector<std::string> >& VarCombinations);
  // Destructor
  virtual ~PndProjectedKNN();
  // Classify
  void Classify(const std::vector<float> &EvtData,
                const unsigned int Neighbours,
                std::map<std::string, float>& result);
  // Set params
  inline void SetEvtParam(const float scFact, const double weight)
  {m_ScaleFact = scFact; m_weight = weight; };
   //Init classifiers
  void InitKNN();
  //=============================================
  //Protected methods and vars
  //protected:
  //=============================================
  //Private methods and vars
 private:
  // Private by design
  PndProjectedKNN();
  void destroy();

  std::string m_inputFile;
  std::vector<std::string> m_ClassNames;
  std::vector<std::string> m_EvtStructure;
  std::vector<std::vector<std::string> > m_VarCombs;
  // First = classifier, Second = parameter indices
  std::vector <std::pair<PndKnnClassify*, std::vector<int>*> > m_Classifiers;
  float  m_ScaleFact;
  double m_weight;
};
#endif//End, class interface
