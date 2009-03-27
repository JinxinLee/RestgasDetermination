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

void printMap(std::map<std::string, int>& res){
  std::cout << "\n===== Output For debugging ========== \n"
	    << " We have seen:" << std::endl;
  for( std::map<std::string, int>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout << (*ii).first << " => " << (*ii).second << std::endl;
  }
  std::cout << "===== Output For debugging ====== \n";
}

const std::string* classifyCand(PndPidCand* cand)
{
  // Check on NULL pointer
  if(!cand){
    std::cout << "Empty candidate object" << std::endl;
    return (new std::string ("Error"));
  }
  
  std::vector <std::string> clsNames;
  cand->GetClsName(clsNames);

  float minVal = std::numeric_limits<float>::max();
  std::string Cls;
  
  for(unsigned int i = 0; i < clsNames.size(); i++){
    std::string CurCls = clsNames[i];
    float val = cand->GetClsVal(CurCls);
    std::cout << CurCls << " = " << val << std::endl;
    
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
  std::vector <std::string> Classes;
  std::vector <PndPidCand*> candList;
  std::map <std::string, int> counts;

  // Open File
  TFile pidFile (inPutFile,"READ");
  
  // Set tree addresses
  TTree *tr = (TTree*) pidFile.Get("cbmsim");
  TClonesArray* arr = new TClonesArray("PndPidCand");
  tr->SetBranchAddress("PndPidCand",&arr);

  if(tr->GetEntriesFast() > 0){
    tr->GetEntry(1);
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
      ClsCurEvt = *(classifyCand(cand));
      //std::cout << ClsCurEvt << std::endl;
      counts[ClsCurEvt] += 1;
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
