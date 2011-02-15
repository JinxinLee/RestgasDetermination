#include "PndMultiClassMlpTrain.h"

PndMultiClassMlpTrain::PndMultiClassMlpTrain(const std::string& InPut,
					     const std::vector<std::string>& ClassNames, 
					     const std::vector<std::string>& VarNames,
					     bool trim)
  : PndMvaTrainer(InPut, ClassNames, VarNames, trim), m_factory(0),
    m_JName(""), m_transform(""), m_MlpOptions("")
{}

PndMultiClassMlpTrain::~PndMultiClassMlpTrain()
{
  if(m_factory)
  {
    delete m_factory;
  }
}

void PndMultiClassMlpTrain::Train()
{
  Init();
}
void PndMultiClassMlpTrain::Init()
{
  if(m_JName == "" ||  m_transform == "" ||  m_MlpOptions == "")
  {
    std::cerr << "<ERROR> Failed to start a job with the following parameters.\n"
	      << "\tName = " << m_JName;    
  }
  //m_factory = new TMVA::Factory();
}
