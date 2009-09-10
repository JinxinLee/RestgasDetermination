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
    std::cout<< "<INFO> Usage: CreateRandomDataSet <Number of events> <OutputFile.root>"
	     << std::endl;
    return 1;
  }

  int numevt = atoi(argv[1]);
  std::string outFile = argv[2];

  std::cout << "Number of events is " << numevt << " output will be saved in " << outFile
	    << std::endl;

  std::vector< std::vector<float>* > varContainer;
  std::vector <TTree*> trees;

  std::vector <std::string> clas;
  std::vector <std::string> vars;

  clas.push_back("Elect");  clas.push_back("Pion");
  clas.push_back("Muon");   clas.push_back("Kaon");
  clas.push_back("Proton"); clas.push_back("Gamma");

  vars.push_back("p"); vars.push_back("emc");
  vars.push_back("thetaC"); vars.push_back("stt");
  vars.push_back("mvd"); vars.push_back("tof");

  TRandom3 rand(57229);

  std::cout << "<INFO>: Initialization."
	    << std::endl;
  /*
   * For each class we need to initialize a vector to be used for
   * writing the var values in the corresponding tree.
   */
  for(size_t i = 0; i < clas.size(); i++){
    std::vector<float>* v = new std::vector<float>();
    //Initialize the vectors to hold variables
    for(size_t j = 0; j < vars.size(); j++){
      v->push_back(0.0);
    }//Now we have m_numVars zero's in v.
    varContainer.push_back(v);
  }//Here we have per class a vector to hold the variables.

  /*
   * For each class Create a tree and push it in the tree
   * container. We have to bind the variables to the tree branches
   * aswell.
   */
  for(size_t cls = 0; cls < clas.size(); cls++){
    std::string name = clas[cls];
    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();

    TTree *sig = new TTree(treeName, treeDesc);

    //Create branches and bind them to variables;
    for(size_t j = 0; j < vars.size(); j++){
      std::string vname = vars[j];
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      //Bind the parameters to the tree elements.
      sig->Branch(bname,&((varContainer[cls])->at(j)),lname);
    }
    //Add the tree to the per class tree holder container
    trees.push_back(sig);
  }
  std::cout << "<INFO>: Initialization finished."
	    << std::endl;

  // generate per class ,#number of events.
  double sigma = 2.00;
  double mean  = 1.00;

  std::cout << "<INFO>: Generating events."
	    << std::endl;
 //class loop
  for(size_t cls = 0; cls < clas.size(); cls++)
  {
    double mt = mean + cls + vars.size();
    double st = sigma;
    //Event loop
    for(int ev = 0; ev < numevt; ev++)
    {
      //Variable loop
      for(size_t var = 0; var < vars.size(); var++)
      {
	(varContainer[cls])->at(var) = static_cast<float>(rand.Gaus(mt, st));
	mt++;
      }
      trees[cls]->Fill();
      mt -= static_cast<double>(vars.size());
    }
  }
  //Write to file;
  std::cout << "Writing to file." << std::endl;

  TFile rootFile (outFile.c_str(), "RECREATE", "RandomEventDatafile", 9);

  for(size_t t = 0; t < trees.size(); t++)
  {
    (trees[t])->Write();
  }
  rootFile.Close();
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
