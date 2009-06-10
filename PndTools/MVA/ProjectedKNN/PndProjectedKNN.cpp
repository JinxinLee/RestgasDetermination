#include "PndProjectedKNN.h"

//========================= Public functions and vars.
PndProjectedKNN::PndProjectedKNN(const char* InputPutFile,
				 const std::vector<std::string>& ClassNames,
				 const std::vector<std::string>& EvtStructure,
				 const std::vector< std::vector<std::string> >& VarCombs
				 )
  :m_inputFile(InputPutFile), m_ClassNames(ClassNames),
   m_EvtStructure(EvtStructure), m_VarCombs(VarCombs)
{
  std::cout << "\t<INFO> Projected_KNN: Initialization." 
	    << std::endl;
}

PndProjectedKNN::~PndProjectedKNN()
{
  destroy();
}

void PndProjectedKNN::Classify(const std::vector<float> &EvtData,
			       const unsigned int Neighbours,
			       std::map<std::string, float>& result)
{
  result.clear();
  // Initialize results
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++)
  {
    result.insert( std::make_pair( m_ClassNames[cls], 0.0 ) );
  }

  // Each classifier produces number of classes results
  std::vector< std::map<std::string, float>* >tmpRes;
  for(unsigned int i = 0; i < m_Classifiers.size(); i++)
  {
    std::map<std::string, float>* rs = new std::map<std::string, float>();
    std::vector<int>* ixs = m_Classifiers[i].second;
    std::vector<float> evt;
    for(unsigned int pr = 0; pr < ixs->size(); pr++)
    {
      evt.push_back(EvtData[ixs->at(pr)]);
    }
    PndKnnClassify* clsfy = m_Classifiers[i].first;
    clsfy->Classify(evt, Neighbours, *rs);
    tmpRes.push_back(rs);
  }
  //Normalize output
  int numClassifiers = tmpRes.size();
  std::map<std::string, float>::iterator it;
  for(int i = 0; i < numClassifiers ; i++)
  {
    std::map<std::string, float> crs = *tmpRes[i];
    for(it = crs.begin(); it != crs.end(); it++)
    {
      result[(*it).first] += (crs[(*it).first]/static_cast<float>(numClassifiers));
    }
    std::cout << std::endl;
  }
  
  //We are doen clean tmpRes
  for(unsigned int i = 0; i < tmpRes.size(); i++)
  {
    (tmpRes[i])->clear();
    delete tmpRes[i];
  }
  tmpRes.clear();
}

void PndProjectedKNN::InitKNN()
{
  for(unsigned int i = 0; i < m_VarCombs.size(); i++)
  {
    std::vector<std::string> pars = m_VarCombs[i];
    PndKnnClassify* cls = new PndKnnClassify(m_inputFile.c_str(), m_ClassNames, pars);
    std::vector<int>* indxs = new std::vector<int>();
    
    // Find parameter indices.
    for(unsigned int j = 0; j < pars.size(); j++)
    {
      for(unsigned int k = 0; k < m_EvtStructure.size(); k++)
      {
	if(pars[j] == m_EvtStructure[k])
	{
	  indxs->push_back(k);
	}
      }
    }

    cls->SetEvtParam(m_ScaleFact, m_weight);
    cls->InitKNN();
    m_Classifiers.push_back(std::make_pair(cls,indxs)); 
  }
}
//=============================================================
//========================= Protected functions and vars.
//=============================================================
//========================= Private functions and vars.
void PndProjectedKNN::destroy()
{
  m_ClassNames.clear();
  m_VarCombs.clear();

  for(unsigned int i = 0; i < m_Classifiers.size(); i++)
  {
    (m_Classifiers[i].second)->clear();
    delete m_Classifiers[i].first;
    delete m_Classifiers[i].second;
  }
  m_Classifiers.clear();

}
//=============================================================
