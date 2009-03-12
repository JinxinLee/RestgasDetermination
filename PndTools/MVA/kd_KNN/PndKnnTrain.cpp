/* **************************************
 * KNN based classifier training class. *
 * Author: M.Babai@rug.nl               *
 * LICENSE:                             *
 * Version 1.0 beta1.                   *
 * **************************************
 */
#include "PndKnnTrain.h"

/**
 * @param OutPut: File name to hold the weight values.
 * @param ClassNames: Class Names.
 * @param VarNames: Variable names creating the feature vector.
 */
PndKnnTrain::PndKnnTrain(const char *OutPut, 
			 const std::vector<std::string>& ClassNames, 
			 const std::vector<std::string>& VarNames)
{
  m_OutPutF  = new TFile(OutPut,"RECREATE");
  m_numClass = ClassNames.size();
  m_ClassNames = ClassNames;
  m_numVars  = VarNames.size();
  /*
   * For each class we need to initialize a vector to be used for
   * writing the var values in the corresponding tree.
   */
  for(int i=0; i<m_numClass; i++){
    std::vector<float>* v = new std::vector<float>();
    //Initialize the vectors to hold variables
    for(int j=0; j<m_numVars; j++){
      v->push_back(0.0);
    }//Now we have m_numVars zero's in v.
    m_varContainer.push_back(v);
  }//Here we have per class a vector to hold the variables.

  /*
   * For each class Create a tree and push it in the tree
   * container. We have to bind the variables to the tree branches
   * aswell.
   */
  for(int cls=0; cls < m_numClass; cls++){
    std::string name = ClassNames[cls];
    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();

    TTree *sig = new TTree(treeName, treeDesc);

    //Create branches and bind them to variables;
    for(int j = 0; j < m_numVars; j++){
      std::string vname = VarNames[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      //Bind the parameters to the tree elements.
      sig->Branch(bname,&((m_varContainer[cls])->at(j)),lname);
    }
    //Add the tree to the per class tree holder container
    m_SigTrees.push_back(sig);
  }
  /*
   * All trees for all classes are initialized and now we are ready to
   * do the training
   */
}

/**
 * Class Destructor.
*/
PndKnnTrain::~PndKnnTrain()
{
  // Write the generated trees to the output file
  for(unsigned int i = 0; i< m_SigTrees.size();i++){
    (m_SigTrees[i])->Write();
  }
  // Clean the container
  for(unsigned int i = 0; i< m_SigTrees.size();i++){
    delete m_SigTrees[i];
  }
  m_SigTrees.clear();

  // Cleaning the var container 
  for(unsigned int k = 0; k< m_varContainer.size();k++){
    delete m_varContainer[k];
  }
  m_varContainer.clear();
  
  // Close the open file and delete the pointer
  m_OutPutF->Close();
  delete m_OutPutF;
}

/**
 * Fill the Signal trees and train the classifier.
 */
void PndKnnTrain::Train(const std::vector<float> &EvtData, 
			const std::string cls)
{
  // Findout the index of the class cls
  int clsIdx = 0;
  while(m_ClassNames[clsIdx] != cls){
    clsIdx++;
  }
  
  // Class index is found. Assigne the values to the correct holder.
  for(unsigned int i=0; i< EvtData.size(); i++){
    (m_varContainer[clsIdx])->at(i) = EvtData[i];
  }
  //Parameters are assigned. Fill the tree.
  (m_SigTrees[clsIdx])->Fill();
}
