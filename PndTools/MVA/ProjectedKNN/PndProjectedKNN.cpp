#define PRINT_DEBUG 0
#include "PndProjectedKNN.h"

//========================= Public functions and vars.
PndProjectedKNN::PndProjectedKNN(const char* InputPutFile,
				 const std::string& sharedVar,
				 const std::vector<std::string>& ClassNames,
				 const std::vector<std::string>& VarNames
				 )
{
  std::cout << "\t<INFO> Projected_KNN: Initialization." 
	    << std::endl;
  m_inputFile = InputPutFile;
  m_sharedVar = sharedVar; 
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;
  
  CreateParPairs();
}

PndProjectedKNN::~PndProjectedKNN()
{
  destroy();
}

void PndProjectedKNN::Classify(const std::vector<float> &EvtData,
			       const unsigned int Neighbours,
			       std::map<std::string, float>& result)
{
  // Initialize results
  result.clear();
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    result.insert( std::make_pair( m_ClassNames[cls], 0.0 ) );
  }

  // Split EvtData in pairs
  std::vector<std::vector<float>* > parPairs;
  
  for(unsigned int idx = 0; idx < m_pairContainer.size(); idx++)
  {
    std::vector<float>* p = new std::vector<float>();
    int idxShared = (m_pairContainer[idx]->second)->at(0);
    int idxParam  = (m_pairContainer[idx]->second)->at(1);

    p->push_back(EvtData[idxShared]);
    p->push_back(EvtData[idxParam]);
    parPairs.push_back(p);
  }
  
  // Each classifier produces, number of classes results
  std::vector<std::map<std::string, float>*> tmpRes;
  
  for(unsigned int i = 0; i < m_classifiers.size(); i++)
  {
    std::map<std::string, float>* rs = new std::map<std::string, float>();
    m_classifiers[i]->Classify( *(parPairs[i]), Neighbours, *rs);
    tmpRes.push_back(rs);
  }
  //Normalize results and store
  std::map<std::string, float>::iterator it;
  int totNumClassifiers = tmpRes.size();
  for(unsigned int k = 0; k < tmpRes.size(); k++)
  {
    std::map<std::string, float>* rslt = tmpRes[k];
    for(it = rslt->begin(); it != rslt->end(); it++)
    {
      result[it->first] += (it->second)/static_cast<float>(totNumClassifiers);
    }
  }

#if PRINT_DEBUG
  std::cout << "\t<INFO>Size of tmpRes " << tmpRes.size() << std::endl;

  for(unsigned int ix = 0; ix < tmpRes.size(); ix++)
  {
    std::map<std::string, float>* bla = tmpRes[ix];
    std::cout << "\t<INFO>Size of result map is " << bla->size() 
	      << std::endl;
    std::cout << "\t<INFO> Classifier out put" << std::endl;
    std::map<std::string, float>::iterator it;
    for(it = bla->begin(); it != bla->end(); it++)
    {
      std::cout << it->first << " " << it->second << ", ";
    }
    std::cout << std::endl;
  }
#endif
  
  // Done, cleaning
  for(unsigned int k = 0; k < tmpRes.size(); k++)
  {
    tmpRes[k]->clear();
    delete tmpRes[k];
  }
  tmpRes.clear();
}

void PndProjectedKNN::InitKNN()
{
  for(unsigned int i = 0; i < m_pairContainer.size(); i++)
  {
    std::vector<std::string>* Vars = m_pairContainer[i]->first;
    
    std::cout << "creating classifier for (";
    for(unsigned int j = 0; j < Vars->size(); j++)
    {
      std::cout << (*Vars).at(j) <<", ";
    }
    std::cout <<") using input file = " 
	      << m_inputFile.c_str() << std::endl;
    // Create classifier object
    PndKnnClassify* clsa = new PndKnnClassify(m_inputFile.c_str(), m_ClassNames, *Vars);
    clsa->SetEvtParam(m_ScaleFact, m_weight);
    // Initialize classifier
    clsa->InitKNN();
    m_classifiers.push_back(clsa);
  }
}
//=============================================================
//========================= Protected functions and vars.
//=============================================================
//========================= Private functions and vars.
void PndProjectedKNN::destroy()
{
  m_ClassNames.clear();
  m_VarNames.clear();
  
  for(unsigned int i = 0; i < m_pairContainer.size(); i++)
  {
    ((m_pairContainer[i])->first)->clear();
    ((m_pairContainer[i])->second)->clear();
    delete (m_pairContainer[i])->first;
    delete (m_pairContainer[i])->second;
    delete m_pairContainer[i];
  }
  
  m_pairContainer.clear();

  for(unsigned int i = 0; i < m_classifiers.size(); i++)
  {
    delete m_classifiers[i];
  }
  m_classifiers.clear();
}

void PndProjectedKNN::CreateParPairs()
{
  // Find shared index
  unsigned int sharedIdx = 0;

  while(sharedIdx < m_VarNames.size() && m_VarNames[sharedIdx] != m_sharedVar)
  {
    sharedIdx++;
  }
  if( sharedIdx == m_VarNames.size() )
  {
    std::cerr<<"\t<ERROR> Could not find the specified shared parameter."
	     << std::endl;
    destroy();
    abort();
  }
  std::string sharedName = m_VarNames[sharedIdx];
  for(unsigned int i = 0; i < m_VarNames.size(); i++)
  {
    if(m_VarNames[i] != sharedName)
    {
      std::vector<std::string>* pairNames = new std::vector<std::string>();
      std::vector<int>* pairIndx          = new std::vector<int>();

      pairNames->push_back(sharedName);
      pairIndx->push_back(sharedIdx);

      pairNames->push_back(m_VarNames[i]);
      pairIndx->push_back(i);
      
      std::pair <std::vector<std::string>*, std::vector<int>* >* par = new std::pair <std::vector<std::string>*, 
	std::vector<int>* >(pairNames, pairIndx);
      m_pairContainer.push_back(par);
    }
  }//End for(i = 0; i < m_VarNames.size())

  // Print the content of the pair container to std out.
  for(unsigned int a = 0; a < m_pairContainer.size(); a++)
  {
    std::vector<std::string>* bb = m_pairContainer[a]->first;
    std::vector<int>* cc = m_pairContainer[a]->second;
    std::cout <<"Created pair " << a << " : ";
    for(unsigned int k = 0; k < bb->size(); k++)
    {
      std::cout << " Var = " << bb->at(k) 
		<< " Idx = " << cc->at(k) << " ";
    }
    std::cout << std::endl;
  }
}
//=============================================================
