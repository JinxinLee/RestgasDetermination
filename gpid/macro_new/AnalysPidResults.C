/*
 * Macro to read and analyse the results produced by PndGPidTask.
 * Implemented By:
 * M. Babai.
 */

// C++
#include <iostream>
#include <map>
#include <vector>
#include <limits>

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

// Prints the result vector to the standard output.
void printMap(std::map<std::string, int>& res){
  std::cout << "\n===== Output For debugging ========== \n"
	    << " We have seen:" << std::endl;
  for( std::map<std::string, int>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout << (*ii).first << " => " << (*ii).second << std::endl;
  }
  std::cout << "===== Output For debugging ====== \n";
}

// Classifys a given candidate, :LVQ classifier
// The Winner takes all.
const std::string* classifyLVQCand(PndPidCand* cand)
{
  // Check on NULL pointer
  if(!cand){
    std::cout << "Empty candidate object" << std::endl;
    return (new std::string ("Error"));
  }
  // Fetch the possible class names
  std::vector <std::string> clsNames;
  cand->GetClsName(clsNames);
  
  float minVal = std::numeric_limits<float>::max();
  std::string Cls;
  
  for(unsigned int i = 0; i < clsNames.size(); i++){
    std::string CurCls = clsNames[i];
    float val = cand->GetClsVal(CurCls);
    
    if(val < minVal){
      minVal = val;
      Cls = CurCls;
    }
  }
  return ( new std::string(Cls));
}

// Main method to analyse the data file produced by GPID task.
void AnalysPidResults(const char* inPutFile)
{
  //Container to hold the counts
  std::map <std::string, int> counts;
  
  // Open File
  TFile pidFile (inPutFile,"READ");
  
  // Set tree branch addres and bound
  TTree *tr = (TTree*) pidFile.Get("cbmsim");
  TClonesArray* arr = new TClonesArray("PndPidCand");
  tr->SetBranchAddress("PndPidCand",&arr);
  
  std::string ClsCurEvt = "";
  
  //Loop through Candidates and collect them
  for (Int_t i = 0; i< tr->GetEntriesFast(); i++){
    std::cout << "\n<INFO>: Candidate no: " << i << std::endl;
    tr->GetEntry(i);
    
    // Loop through Candidates.
    for (Int_t j = 0; j < arr->GetEntriesFast(); j++){
      PndPidCand* cand = (PndPidCand*) arr->At(j);
      // Classify current candidate
      ClsCurEvt = *(classifyLVQCand(cand));
      
      if(ClsCurEvt == ""){// The track is !Okay
	counts["Unknown"] += 1;
      }
      else{
	counts[ClsCurEvt] += 1;
      }
    }
  }
  // DEBUG
  printMap(counts);
  // Finished clean-up
  std::cout <<"\n\n Number of processed tracks = " 
	    << tr->GetEntriesFast() << std::endl;
  
  counts.clear();
  arr->Delete();
  delete arr;
  pidFile.Close();
  exit(0);
}
