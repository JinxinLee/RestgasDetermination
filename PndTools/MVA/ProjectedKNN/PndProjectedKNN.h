#ifndef PNDPROJECTEDKNN_H
#define PNDPROJECTEDKNN_H

// C++ headers
#include <vector>

//PANDA Headers
#include "PndKnnClassify.h"

class PndProjKNN{
  //Public methods and vars
 public:
  // Constructor
  PndProjKNN(const char *InputPutFile,
	     const std::string sharedVar,
	     const std::vector<std::string>& ClassNames, 
	     const std::vector<std::string>& VarNames);
  // Destructor
  virtual ~PndProjKNN();

  void Classify(const std::vector<float> &EvtData, 
                const unsigned int Neighbours, 
                std::map<std::string, float>& result);

  inline void SetEvtParam(const float scFact, const double weight)
  {m_ScaleFact = scFact; m_weight = weight; };

  //Protected methods and vars
 protected:
  //Private methods and vars
 private:
  // Do not change, private by design
  PndProjKNN();
  
  void CreateParPairs();
  std::vector<std::vector<std::string>*> m_pairContainer;
  std::string m_sharedVar;
  std::vector<std::string> m_ClassNames;
  std::vector<std::string> m_VarNames;
  float m_ScaleFact; 
  double m_weight;
};
#endif//End, class interface
