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
    
    //std::cout << CurCls << " = " << val << std::endl;
    
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
  // Container to store candidates.
  std::vector <PndPidCand*> candList;
  std::vector <std::string> Classes;
  std::map <std::string, int> counts;

  // Open File
  TFile pidFile (inPutFile,"READ");
  
  // Set tree addresses
  TTree *tr = (TTree*) pidFile.Get("cbmsim");
  TClonesArray* arr = new TClonesArray("PndPidCand");
  tr->SetBranchAddress("PndPidCand",&arr);

  //Fetch the class names.
  if(tr->GetEntriesFast() > 0){
    tr->GetEntry(0);
    if( arr->GetEntriesFast() > 0 ){
      ((PndPidCand*) arr->At(0))->GetClsName(Classes);
    }
  }
  
  for(unsigned int i = 0; i < Classes.size(); i++){
    std::string b = Classes[i];
    counts[b] = 0;
  }
  
  std::string ClsCurEvt = "";

  //Loop through Candidates and collect them
  for (Int_t i = 0; i< tr->GetEntriesFast(); i++){
    std::cout << "\n<INFO>: Candidate number " << i << std::endl;
    tr->GetEntry(i);
    
    // Collect the candidates.
    for (Int_t j = 0; j < arr->GetEntriesFast(); j++){
      PndPidCand* cand = (PndPidCand*) arr->At(j);
      candList.push_back(cand);
      ClsCurEvt = *(classifyLVQCand(cand));
      
      if(ClsCurEvt == ""){// The track is !Okay
	counts["Unknown"] += 1;
      }
      else{
	counts[ClsCurEvt] += 1;
      }
    }
  }
  //printVect(Classes);
  printMap(counts);
  // Finished clean-up
  std::cout <<"\n\n Number of processed tracks = " 
	    << candList.size() << std::endl;

  Classes.clear();
  candList.clear();
  counts.clear();
  arr->Delete();
  delete arr;
  pidFile.Close();
  //exit(0);
}
