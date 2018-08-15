/* ***************************************
 * PDEFoam  Classifier                   *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * License:                              *
 * ***************************************
 */
#include "PndPDEFoamClassify.h"

using namespace std;

PndPDEFoamClassify::PndPDEFoamClassify(const std::string& inputFile,
				       const std::vector<std::string>& classNames, 
				       const std::vector<std::string>& varNames)
  : m_inputFile(inputFile)
{
  // Initialize classes
  for(size_t i = 0; i < classNames.size(); i++)
  {
    m_classes.push_back(PndMvaClass(classNames[i]));
  }
  
  // Init varaiables
  for(size_t i = 0; i < varNames.size(); i++)
  {
    m_vars.push_back(PndMvaVariable(varNames[i]));
  }
  
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
  result.clear();
  // Norm. current event
  for(size_t i = 0; i < m_vars.size(); i++)
  {//eventData[i] = eventData[i] - m_vars[i].Mean; eventData[i] = eventData[i] / m_vars[i].NormFactor;
    eventData[i] -= m_vars[i].Mean;
    eventData[i] /= m_vars[i].NormFactor;
  }

  TMVA::EKernel kk = TMVA::EKernel(0);
  for(size_t fm = 0; fm < m_foams.size(); fm++)
  {
    std::string foamName (m_foams[fm]->GetFoamName());
    if(!m_foams[fm])
      std::cout << "Empty foam, NULL" << std::endl;
    else
      result[foamName] = m_foams[fm]->GetCellDensity(eventData, kk);
  }
}

const std::string& PndPDEFoamClassify::Classify(std::vector<float> EvtData)const
{
  EvtData.clear();
  std::string* bla = new std::string("Not available yet.");
  return *bla;
}

void PndPDEFoamClassify::ReadInputFile()
{
  std::cout << "<INFO>: Reading Foam weight file." << std::endl;
  assert(m_inputFile != "");
  TFile input(m_inputFile.c_str(),"READ");

  for(size_t i = 0; i < m_classes.size(); i++)
  {
    std::string foamName = m_classes[i].Name;

    assert( typeid(*(input.Get(foamName.c_str()))) == typeid(TMVA::PDEFoam));

    TMVA::PDEFoam* ff = (TMVA::PDEFoam*) input.Get(foamName.c_str());
    if(!ff){
      std::cout << "<ERROR>: Could not find the foam for " 
		<< foamName << std::endl;
      assert(ff);
    }
    //ff->Init();
    m_foams.push_back(ff);
  }  // All foams are read.

  // Reading normalization parameters.
 // Read Norm facts
  TTree* fact = (TTree*) input.Get("NormFact");
  if(fact)
  {
    std::cout << "<INFO> Reading normalization values."<< std::endl;
    vector<float> normVars(m_vars.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < m_vars.size(); j++)
    {
      const char* branchName = m_vars[j].Name.c_str();
      
      //Binding the branches
      fact->SetBranchAddress(branchName, &(normVars[j]));
    }// Tree parameters are bounded
    
    // Fetch and store the variables to variable container
    for(unsigned int k = 0; k < fact->GetEntriesFast(); k++)
    {
      fact->GetEntry(k);
      
      for(size_t idx = 0; idx < m_vars.size(); idx++)
      {
	m_vars[idx].NormFactor = normVars[idx];
      }
    }//End of tree loop
    delete fact;

    // Read Means
    TTree* m = (TTree*) input.Get("Means");
    if(m)
    {
      cout << "<INFO> Reading mean values."<< endl;
      
      vector<float> meanVals(m_vars.size(), 0.0);
      
      // Bind the parameters to the tree branches
      for(size_t j = 0; j < m_vars.size(); j++)
      {
	const char* branchName = m_vars[j].Name.c_str();
	
	//Binding the branches
	m->SetBranchAddress(branchName, &(meanVals[j]));
      }// Parameters are bounded
      
      // Fetch and store the variables to variable container
      for(unsigned int k = 0; k < m->GetEntriesFast(); k++)
      {
	m->GetEntry(k);
	
	for(size_t idx = 0; idx < m_vars.size(); idx++)
	{
	  m_vars[idx].Mean = meanVals[idx];
	}
      }//End of tree loop
      delete m;
    }
    // Close open file
    input.Close();
  }
}
