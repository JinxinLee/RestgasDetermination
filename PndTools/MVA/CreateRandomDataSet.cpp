/* *****************************************
 * Creates sample datafiles. The variables *
 * can have any distribution. It is just   *
 * for algorithm testing.                  *
 * Author: M.Babai.                        *
 * E-Mail: M.Babai@rug.nl                  *
 * License:                                *
 * Verion:                                 *
 *******************************************/
#include <stdlib.h>

#include <iostream>
#include <vector>

#include "TFile.h"
#include "TTree.h"
#include "TRandom3.h"

int main(int argc, char**argv)
{
  if(argc < 3)
  {
    std::cerr<< "<Usage> Usage: CreateRandomDataSet <Number of events>"
	     << " <OutputFile.root>\n";
    return 1;
  }

  int numevt = atoi(argv[1]);
  std::string outFile = argv[2];
  
  std::cout << "Number of events is " << numevt
	    << " output will be saved in " << outFile
	    << '\n';
  
  std::vector< std::vector<float>* > varContainer;
  std::vector <TTree*> trees;
  
  std::vector <std::string> clas;
  std::vector <std::string> vars;
  
  clas.push_back("electron");
  clas.push_back("pion");
  //  clas.push_back("muon");
  //clas.push_back("kaon");
  //clas.push_back("proton");
  
  //vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  vars.push_back("E9E25");
  // vars.push_back("E1");
  // vars.push_back("E9");
  // vars.push_back("E25");
  // vars.push_back("E1E9");
  // vars.push_back("stt"); vars.push_back("thetaC");
  // vars.push_back("mvd"); vars.push_back("tof");  


  // vars.push_back("thetaC"); vars.push_back("stt");
  // vars.push_back("mvd"); vars.push_back("tof");

  TRandom3 rand(9753);
  
  std::cout << "<INFO>: Initialization.\n";

  /*
   * For each class we need to initialize a vector to be used for
   * writing the var values in the corresponding tree.
   */
  for(size_t i = 0; i < clas.size(); i++)
  {
    //Initialize the vectors to hold variables
    std::vector<float>* v = new std::vector<float>(vars.size(), 0.0);
    varContainer.push_back(v);
  }//per class a vector to hold the variables.
  
  /*
   * For each class Create a tree and push it in the tree
   * container. We have to bind the variables to the tree branches
   * aswell.
   */
  for(size_t cls = 0; cls < clas.size(); cls++)
  {
    std::string name = clas[cls];
    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();
    
    TTree *sig = new TTree(treeName, treeDesc);
    
    //Create branches and bind them to variables;
    for(size_t j = 0; j < vars.size(); j++)
    {
      std::string vname = vars[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      //Bind the parameters to the tree elements.
      sig->Branch(bname, &((varContainer[cls])->at(j)), lname);
    }
    //Add the tree to the per class tree holder container
    trees.push_back(sig);
  }
  std::cout << "<INFO>: Initialization finished.\n";
  
  // generate per class ,#number of events.
  std::cout << "<INFO>: Generating events.\n";

  //class loop
  int NumClasses = clas.size();
  int cls = 0;
  double sigma = 1.00;
  double mean  = 1.00;

  // Class (label) loop.
  for(cls = 0; cls < NumClasses; cls++)
  {
    //mean += sigma + 2;
    //Event loop
    for(int ev = 0; ev < numevt; ev++)
    {
      //Variable loop
      for(size_t var = 0; var < vars.size(); var++)
      {
	(varContainer[cls])->at(var) = static_cast<float>(rand.Gaus(mean, sigma));
	//(varContainer[cls])->at(var) = static_cast<float>(rand.Uniform(cls, (cls + 1)));
      }
      trees[cls]->Fill();
    }
  }
  
  //Write to file;
  std::cout << "<INFO>Writing to file.\n";
  
  TFile rootFile (outFile.c_str(), "RECREATE", "RandomEventDatafile", 9);
  
  for(size_t t = 0; t < trees.size(); t++)
  {
    (trees[t])->Write();
  }
  rootFile.Close();
  
  //===========================
  //Clean up data structures
  for(size_t i = 0; i < varContainer.size(); i++)
  {
    varContainer[i]->clear();
    delete varContainer[i];
  }  
  varContainer.clear();
  
  for(size_t i = 0; i < trees.size(); i++)
  {
    delete trees[i];
  }
  trees.clear();
  
  return 0;
}
