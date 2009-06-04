#include "PndProjectedKNN.h"

PndProjKNN::PndProjKNN(const char *InputPutFile,
		       const std::string sharedVar,
		       const std::vector<std::string>& ClassNames, 
		       const std::vector<std::string>& VarNames)
{
  m_sharedVar = sharedVar;
  m_ClassNames = ClassNames;
  m_VarNames   =  VarNames;
}

PndProjKNN::~PndProjKNN()
{
  m_ClassNames.clear();
  m_VarNames.clear();
  for(unsigned int i = 0; i < m_pairContainer.size(); i++)
  {
    (m_pairContainer[i])->clear();
    delete m_pairContainer[i];
  }
  m_pairContainer.clear();
  
}
void PndProjKNN::Classify(const std::vector<float> &EvtData, 
			  const unsigned int Neighbours, 
			  std::map<std::string, float>& result)
{
}
