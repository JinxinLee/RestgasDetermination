/*
 * Macro to read and analyse the results produced by PndGPidTask.
 * Implemented By:
 * M. Babai.
 */

// C++
#include <iostream>
#include <map>

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
// Main method to analyse the data file produced by GPID task.
void AnalysPidResults(const char* inPutFile)
{
  TFile pidFile (inPutFile,"READ");

  TTree *tr = (TTree*) pidFile.Get("cbmsim");
  TClonesArray* arr = new TClonesArray("PndPidCand");
  
  tr->SetBranchAddress("PndPidCand",&arr);
  
  for (Int_t i = 0; i< tr->GetEntriesFast(); i++){
    std::cout << "\n<INFO>: Candidate number " << i << std::endl;
    tr->GetEntry(i);

    std::cout << "length of Arr is " << arr->GetEntriesFast() << std::endl;
 
    for (Int_t j = 0; j < arr->GetEntriesFast(); j++){
      PndPidCand* cand = (PndPidCand*) arr->At(j);

      std::vector<std::string> List;
      cand->GetVarName(List);
      printVect(List);
    
      for(unsigned int id = 0; id < List.size(); id++){
	std::string na = List[id];
	std::cout << na << " = " << cand->Get(na) << std::endl;
      }
    }
  }
  pidFile.Close();
  delete arr;
}
