/* **************************************
 * KNN based classifier training class. *
 * Author: M.Babai@rug.nl               *
 * Version 1.0 beta1.                   *
 * **************************************
 */
#include "PndKnnTrain.h"

/*
 * @param OutPut: The file name to hold the weight values.
 * @param ClassNames: Name of classes for wich the classifier is
 * trained.
 * @param VarNames: The name of variables creating the feature vector.
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
   * All trees for all classes are initialized an now we are ready to
   * do the training
   */
}

/* ========== Class Destructor =========== */
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

/* 
 * Here we fill the Signal trees and train the classifier.
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

/* ******************************************
 * Test function can be deleted afterwards. *
 * ******************************************
 */
/*
  int main(int argc, char** argv){
  int totEvtNum = 100000;
  
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  clas.push_back("El"); clas.push_back("Pi"); clas.push_back("Ka");
  clas.push_back("gam"); clas.push_back("mu");
  
  nam.push_back("ep");  nam.push_back("tof"); nam.push_back("mvd");
  nam.push_back("p");  nam.push_back("f"); nam.push_back("d");
  nam.push_back("a");  nam.push_back("b"); nam.push_back("c");
  
  std::string ot   = "Test.root";
  const char* file = ot.c_str();
  
  PndKnnTrain bla (file,clas,nam);
  
  std::vector<float> evt;
  
  for(int i=0; i < totEvtNum; i++){
  
  evt.clear();
  evt.push_back(1.0);
  evt.push_back(2.0);
  evt.push_back(3.0);
  evt.push_back(4.0);
  evt.push_back(5.0);
  evt.push_back(6.0);
  evt.push_back(7.0);
  evt.push_back(8.0);
  evt.push_back(9.0);
  bla.Train(evt,"El");
  
  evt.clear();
  evt.push_back(11.0);
  evt.push_back(12.0);
  evt.push_back(13.0);
  evt.push_back(14.0);
  evt.push_back(15.0);
  evt.push_back(16.0);
  evt.push_back(17.0);
  evt.push_back(18.0);
  evt.push_back(19.0);
  bla.Train(evt,"Pi");
  
  evt.clear();
  evt.push_back(21.0);
  evt.push_back(22.0);
  evt.push_back(23.0);
  evt.push_back(24.0);
  evt.push_back(25.0);
  evt.push_back(26.0);
  evt.push_back(27.0);
  evt.push_back(28.0);
  evt.push_back(29.0);
  bla.Train(evt,"Ka");
  
  evt.clear();
  evt.push_back(31.0);
  evt.push_back(32.0);
  evt.push_back(33.0);
  evt.push_back(34.0);
  evt.push_back(35.0);
  evt.push_back(36.0);
  evt.push_back(37.0);
  evt.push_back(38.0);
  evt.push_back(39.0);
  bla.Train(evt,"gam");
  
  evt.clear();
  evt.push_back(41.0);
  evt.push_back(42.0);
  evt.push_back(43.0);
  evt.push_back(44.0);
  evt.push_back(45.0);
  evt.push_back(46.0);
  evt.push_back(47.0);
  evt.push_back(48.0);
  evt.push_back(49.0);
  bla.Train(evt,"mu");
  }
  return 0;
  }
*/
