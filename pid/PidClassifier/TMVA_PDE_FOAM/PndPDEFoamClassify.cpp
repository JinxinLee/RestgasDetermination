#include "PndPDEFoamClassify.h"


PndPDEFoamClassify::PndPDEFoamClassify(const std::string& inputFile,
				       const std::vector<std::string>& classNames, 
				       const std::vector<std::string>& varNames)
  : m_inputFile(inputFile), m_classNames(classNames), m_varNames(varNames)
{
  ReadInputFile();
}

PndPDEFoamClassify::~PndPDEFoamClassify()
{
  std::cout << "<INFO>: Cleaning objects(FOAM)." << std::endl;
  for(size_t i = 0; i < m_foams.size(); i++)
  {
    delete m_foams[i];
  }
  m_foams.clear();
}

void PndPDEFoamClassify::GetMvaValues(std::vector<float> eventData, 
				      std::map<std::string, float>& result)
{
  eventData.clear();
  result.clear();
}

const std::string& PndPDEFoamClassify::Classify(std::vector<float> EvtData)const
{
  EvtData.clear();
  std::string* bla = new std::string("leeg");
  return *bla;
}

void PndPDEFoamClassify::ReadInputFile()
{
  std::cout << "<INFO>: Reading Foam weight file." << std::endl;
  assert(m_inputFile != "");
  TFile input(m_inputFile.c_str(),"READ");

  for(size_t i = 0; i < m_classNames.size(); i++)
  {
    std::string foamName = m_classNames[i];

    assert( typeid(*(input.Get(foamName.c_str()))) == typeid(TMVA::PDEFoam));

    TMVA::PDEFoam* ff = (TMVA::PDEFoam*) input.Get(foamName.c_str());
    if(!ff){
      std::cout << "<ERROR>: Could not find the foam for " 
		<< foamName << std::endl;
      assert(ff);
    }
    m_foams.push_back(ff);
  }  // All foams are read.

  // Reading normalization parameters.
  
  // Close open file
  input.Close();
}
