/********************
 * Author: M. Babai *
 * M.Babai@rug.nl   *
 ********************
 */
/*
 * Example program. This code shows how to use PCA within Panda MVA
 * package.
 */

// Local includes
#include "PndMvaDataSet.h"
#include "PndMvaVarPCATransform.h"

// Root includes
#include "TFile.h"
#include "TNtuple.h"

int main(int argc, char** argv)
{
  if(argc < 3){
    std::cerr << "<ERROR> Usage: "<< argv[0] << " <inputeventFile> <OutFile>"
	      << std::endl;
    return 1;
  }
  
  std::string inFile  = argv[1];
  std::string outFile = argv[2];
  
  std::vector <std::string> clas;
  std::vector <std::string> vars;
  
  clas.push_back("electron"); clas.push_back("pion");
  clas.push_back("muon"); clas.push_back("kaon");
  clas.push_back("proton");//clas.push_back("gamma");
  
  vars.push_back("p"); vars.push_back("emc");
  vars.push_back("z20");vars.push_back("z53");
  vars.push_back("lat");
  
  // Read data.
  PndMvaDataSet data(inFile, clas, vars);

  // Fetch available events.
  const std::vector< std::pair<std::string, std::vector<float>*> >& dd = data.GetData();

  // Create PCA transformation object and init. 
  PndMvaVarPCATransform pca;
  pca.InitPCATranformation(dd);
  
  // Transform input vars.
  std::vector< std::pair<std::string, std::vector<float>*> > outEvt;

  // Events loop
  for(size_t evt = 0; evt < dd.size(); evt++){
    std::string curClass = (dd[evt]).first;// Current class name
    std::vector<float>* curEvt = (dd[evt]).second;// Current event vector
    // Transform current event and copy
    std::vector<float>* trEvt = new std::vector<float>(pca.Transform(*curEvt));
    // Add result to the out vector.
    outEvt.push_back(std::make_pair(curClass, trEvt));
  }

  // Create output Ntuple
  TFile outPutFile(outFile.c_str(), "RECREATE", "OutFileTitle", 9);
  for(size_t i = 0; i < outEvt.size(); i++){
    TNtuple output( ((outEvt[i]).first).c_str(), ((outEvt[i]).first + "desc").c_str(), "x");
    output.Write();
  }
  outPutFile.Close();
  //===== Cleaning
  for(size_t i = 0; i < outEvt.size(); i++){
    delete (outEvt[i]).second;
  }
  outEvt.clear();

  return 0;
}
