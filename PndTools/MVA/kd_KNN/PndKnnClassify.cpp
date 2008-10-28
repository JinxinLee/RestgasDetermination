/* ***************************************
 * KNN based Classifier using kd-tree    *
 * data structure for better recognition *
 * performance.                          *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version 1.0 beta1.                    *
 * ***************************************
 */
#include "PndKnnClassify.h"

/* Function used for sorting the distances container */
bool LessFunct(DistObject* p1, DistObject* p2)
{  
  if(p1->m_dist < p2->m_dist)
  {
    return true;
  }
  return false;
}

/*
 * @param InputPutFile: The name of the file that holds the weits
 * @param ClassNames: The names of classes to which an event might be
 * assigned to.
 * @param VarNames: Variable names from which the feature vector is
 * built.
 */
PndKnnClassify::PndKnnClassify(const char *InputPutFile,
			       const std::vector<std::string>& ClassNames, 
			       const std::vector<std::string>& VarNames)
{
  // Initialize the input file pointer
  TFile* m_InPutF = new TFile(InputPutFile,"READ");

  // Dimension of the data vectors
  const unsigned int DIM = VarNames.size();
  
  //Copy the class and var names.
  m_ClassNames = ClassNames;
  m_VarNames   = VarNames;
  
  // Fetch the class trees and read the event data.
  for(unsigned int i = 0; i < m_ClassNames.size();i++){
    const char *name = m_ClassNames[i].c_str();
    
    TTree *t = (TTree*) m_InPutF->Get(name);

    // Init a container to bind to the tree branches
    std::vector<float> ev (m_VarNames.size(),0.0);
    
    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < m_VarNames.size(); j++){
      const char *bname = m_VarNames[j].c_str();

      t->SetBranchAddress(bname, &(ev[j]));
    }// Tree parameters are bounded
    
    // Get number of available examples for the current class
    const unsigned int NumExamples = t->GetEntriesFast();
    //Store the number of events per class (Normalization)
    m_perClassExamples.insert(std::make_pair(m_ClassNames[i], NumExamples));

    //Create temporary data storage
    array2dfloat EvtPoints;
    EvtPoints.resize(extents[NumExamples][DIM]);

    // Fetch and store the variables to per class variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++){
      t->GetEntry(k);
      
      // Store the event data read from the input tree
      for(unsigned int idx = 0; idx < DIM; idx++){
	EvtPoints[k][idx] = ev[idx];
      }
    }
    // Create tree for the current values and optimise
    kdtree2* EvTree = new kdtree2(EvtPoints,true);
    EvTree->sort_results = true;
    //Insert EvTree in the tree container
    m_EventTreeCont.push_back(std::make_pair(m_ClassNames[i], EvTree));

    //We are done and can delete the tree pointer
    delete t;
  } // Values from the (ROOT)trees are inserted into the KNN_TREE container
  std::cout << "Trees are created and optimised" << std::endl;
  
  // Close the open file.
  m_InPutF->Close();
  delete m_InPutF;
}

PndKnnClassify::~PndKnnClassify()
{
  m_ClassNames.clear();
  m_VarNames.clear();
  m_perClassExamples.clear();

  for(unsigned int i = 0; i < m_EventTreeCont.size(); i++){
    delete (m_EventTreeCont[i]).second;
  }
  m_EventTreeCont.clear();
  
  for(unsigned int i = 0; i < m_dists.size(); i++){
    delete m_dists[i];
  }
  m_dists.clear();
}

/* 
 * Computes the Euclidean distance between two given vectors of
 * event features.
 */
float PndKnnClassify::ComputeDist(std::vector<float> &EvtData, 
				  std::vector<float> &Ex)
{
  float dist = 0.0;
  
  for(unsigned int i = 0; i< Ex.size(); i++){
    dist += (EvtData[i] - Ex[i]) * (EvtData[i] - Ex[i]);
  }
  return dist;
}

/*
 * @param InputPutFile: The name of the file that holds the weits
 * @param ClassNames: The names of classes to which an event might be
 * assigned to.
 * @param VarNames: Variable names from which the feature vector is
 * built.
 */
void PndKnnClassify::Classify(std::vector<float> &EvtData, 
			      unsigned int Neighbours,
			      std::map<std::string, float>& result)
{
  result.clear();
  // Initialize results
  for(unsigned int id = 0; id < m_ClassNames.size(); id++){
    result.insert( std::make_pair( m_ClassNames[id], 0.0 ) ); 
  }
  // We need to store the results.
  kdtree2_result_vector TempResult;
  
  //Loop through all available classes.
  for(unsigned int cls = 0; cls < m_ClassNames.size(); cls++){
    kdtree2* tr = m_EventTreeCont[cls].second;
    
    // Fetch the search results for the current class
    tr->n_nearest(EvtData, Neighbours, TempResult);
    
    // Compute the distances and store
    for(unsigned int ds = 0; ds< TempResult.size(); ds++){
      kdtree2_result rs = TempResult[ds];
      DistObject* ds = new DistObject(rs.dis, m_EventTreeCont[cls].first);
      m_dists.push_back(ds);
    }
    //Clear TempResult container
    TempResult.clear();
  }// All classes are processed
  
  if(Neighbours > m_dists.size()){
    std::cout << "=============================================\n"
	      << "< ERROR > Number of Neighbours is larger than\n"
	      << " the number of available examples.\n"
	      << "============================================="
	      << std::endl;
    return;
  }

  //All distances are determined, now we can classify
  sort(m_dists.begin(), m_dists.end(), LessFunct);

  for(unsigned int id = 0; id < Neighbours; id++){
    DistObject* dOb = m_dists[id];//take the distance object
    std::string clas = dOb->m_cls;//Find the Object class
    //Increment the number of objects found for a certain class
    result[clas] += 1.0;
  }
  // Normalizing the results
  float probSum = 0.0;
  for(unsigned int i = 0; i < result.size(); i++){
    std::string className = m_ClassNames[i];
    int num = m_perClassExamples[className];
    result[className] = result[className]/num;
    probSum += result[className];
  }
  
  for(unsigned int cl = 0; cl < m_ClassNames.size(); cl++){
    std::string className = m_ClassNames[cl];
    result[className] = result[className]/probSum;
  }

  /* 
   * Clear the m_dists list, for the next classification. This needs
   * to be reimplemented in order to do object reuse.
   */
  for(unsigned int i = 0; i < m_dists.size(); i++){
    delete m_dists[i];
  }
  m_dists.clear();
}
