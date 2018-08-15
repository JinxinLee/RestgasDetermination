/***************************
 * Author: M.Babai.        *
 * E-Mail: M.Babai@rug.nl  *
 * License:                *
 * Verion:                 *
 ***************************
 */
/*
 * Example program. This code shows how to use PCA within Panda MVA
 * package.
 */

// Local includes
#include "PndMvaDataSet.h"
#include "PndMvaVarPCATransform.h"

// C++
#include <map>

// Root includes
#include "TFile.h"
#include "TTree.h"

int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cerr << "<ERROR> Usage: "<< argv[0] 
	      << " <inputeventFile> <OutFile>"
	      << std::endl;
    return 1;
  }
  
  std::string inFile  = argv[1];
  std::string outFile = argv[2];
  
  std::vector <std::string> clas;
  std::vector <std::string> vars;
  
  clas.push_back("electron");
  clas.push_back("pion");
  clas.push_back("muon");
  clas.push_back("kaon");
  clas.push_back("proton");
  
  //vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  
  // Read data.
  PndMvaDataSet data(inFile, clas, vars,TRAIN);
  data.Initialize();

  //data.Trim();

  // Fetch available events.
  std::vector< std::pair<std::string, std::vector<float>*> > const& dd = data.GetData();
  
  // Create PCA transformation object.
  PndMvaVarPCATransform pca;

  // Init PCA transformation object.
  pca.InitPCATranformation(dd);

  // To store transform input vars.
  std::vector< std::pair<std::string, std::vector<float>*> > outEvt;
  
  // Events loop
  for(size_t evt = 0; evt < dd.size(); evt++)
  {
    // Current class name
    std::string curClass = (dd[evt]).first;
    
    // Current event vector
    std::vector<float>* curEvt = (dd[evt]).second;
    
    // Transform current event and copy
    std::vector<float>* trsEvt = pca.Transform(*curEvt);
    
    // Add result to the out vector.
    outEvt.push_back(std::make_pair(curClass, trsEvt));
  }
  
  // Create output File, Trees and write
  TFile outPutFile(outFile.c_str(), "RECREATE", "PCA_Transformed_Events", 9);
  
  for(size_t cls = 0; cls < clas.size(); cls++)
  {
    std::vector<float> buffer(vars.size(), 0.0);
    
    std::string name = clas[cls];
    std::string desc = "Description Of " + name;
    char const* treeName = name.c_str();
    char const* treeDesc = desc.c_str();
    
    // Create a tree
    TTree sig (treeName, treeDesc);
    
    // Create branches and bind the variables
    for(size_t j = 0; j < vars.size(); j++)
    {
      std::string vname = vars[j];
      std::string leaf  = vname + "/F" ;
      char const* bname = vname.c_str();
      char const* lname = leaf.c_str();
      
      // Bind the parameters to the tree elements.
      sig.Branch(bname, &buffer[j], lname);
    }
    
    // Fill The tree
    for(size_t i = 0; i < outEvt.size(); i++)
    {
      if(outEvt[i].first == name)
      {
        for(size_t k = 0; k < buffer.size(); k++)
        {
          buffer[k] = (outEvt[i].second)->at(k);
        }
        sig.Fill();
      }
    }
    // Write the created tree
    sig.Write();
  }//End for cls
  outPutFile.Close();
  

  // ========= Cleaning ======================
  for(size_t i = 0; i < outEvt.size(); i++)
  {
    delete (outEvt[i]).second;
  }
  outEvt.clear();
  //--------------------------------------------
  std::cout << "<INFO> Done PCA transformation.\n";
  return 0;
}
