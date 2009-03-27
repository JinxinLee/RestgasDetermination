/*
 * Macro to read and analyse the results produced by PndGPidTask.
 * Implemented By:
 * M. Babai.
 */

// C++
#include <iostream>
#include <map>
#include "vector"

// Root
#include "TFile.h"
#include "TClonesArray.h"
#include "TTree.h"

// Panda
#include "PndPidCand.h"

// Prints a given vector to standard cout.
void printVect (std::vector<std::string>& vect)
{
  std::cout << "Length of vector = " << vect.size() <<std::endl;
  for(unsigned int i = 0; i < vect.size(); i++){
    std::cout << "var["<< i <<"] = "<< vect[i] << ", ";
  }
  std::cout << std::endl;
}
//FIXME FIXME We need to have TFile
void collectCandidates(const char* inPutFile, std::vector <PndPidCand*>& con)
{
  TFile pidFile (inPutFile,"READ");
    
  TTree *tr = (TTree*) pidFile.Get("cbmsim");
  TClonesArray* arr = new TClonesArray("PndPidCand");
  
  tr->SetBranchAddress("PndPidCand",&arr);
  
  for (Int_t i = 0; i< tr->GetEntriesFast(); i++){
    std::cout << "\n<INFO>: Candidate number " << i << std::endl;
    tr->GetEntry(i);
    
    std::cout << "length of Arr is " << arr->GetEntriesFast() << std::endl;
    // Get the candidates.
    for (Int_t j = 0; j < arr->GetEntriesFast(); j++){
      PndPidCand* cand = (PndPidCand*) arr->At(j);
      con.push_back(cand);
    }
  }
  // Finished clean-up
  //pidFile.Close();
  delete arr;
  //std::cout << con.size() << std::endl;
}

void classifyCand(PndPidCand* cand)
{
  if(!cand){
    std::cout << "Empty candidate object" << std::endl;
    return;
  }
  std::vector <std::string> clsNames;
  cand->GetClsName(clsNames);
  
  for(unsigned int i = 0; i < clsNames.size(); i++){
    std::string cls = clsNames[i];
    float val = cand->GetClsVal(cls);
    std::cout << cls << " " << val << std::endl;
  }
  std::cout << "Call " << clsNames.size() << std::endl;
}

// Main method to analyse the data file produced by GPID task.
void AnalysPidResults(const char* inPutFile)
{
  // Container to store candidates.
  std::vector <PndPidCand*> candList;
  collectCandidates(inPutFile, candList);

  for(unsigned int i = 0; i < candList.size(); i++){
    classifyCand(candList[i]);
  }
  // Finished clean-up
  std::cout <<"\n\n Number of processed tracks = " 
	    << candList.size() << std::endl;
  candList.clear();
  
  // exit(0);
}
