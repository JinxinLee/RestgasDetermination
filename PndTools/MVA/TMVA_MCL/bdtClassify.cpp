/* ***************************************
 * MultiClass BDT Classify example       *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
/*
 * Note: This is just an interface to the original TMVA
 * implementation. To find out the available options, please read TMVA
 * manuals. In case of errors or wrong outputs produced by TMVA
 * classifiers, try to read their mailing list and send your questions
 * to the same list.
 ******* VERY IMORTANT ****
 * You NEED TMVA version > 4.1.X before this works.
 */

#include "PndMultiClassBdtClassify.h"

// Print the results map.
void printResult(std::map<std::string, float> const& res)
{
  std::cout << "\n================================== \n";
  for( std::map<std::string,float>::const_iterator ii=res.begin();
       ii != res.end(); ++ii)
  {
    std::cout << (*ii).first << " => " << (*ii).second << '\n';
  }
  std::cout << "======================================= \n";
}

int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "<ERROR>\n\t<USAGE>: " << argv[0]
	      << " <Weight File> <Events File> <Tree Name>\n";
    exit(10);
  }

  // The weightFile.  
  std::string WeightFile = argv[1];
  
  std::string EvtFile    = argv[2];

  std::string TreeName   = argv[3];

  // Labels.
  std::vector<std::string> labels;

  // Variables.
  std::vector<std::string> variables;

  // Class names
  labels.push_back("electron");
  labels.push_back("pion");
 
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");
  
  // Variable names 
  variables.push_back("emc");
  variables.push_back("lat");
  variables.push_back("z20");
  variables.push_back("z53");
  
  // Create and init the classifier object
  PndMultiClassBdtClassify cls(WeightFile, labels, variables);
  cls.Initialize();

  // Open file and read the events.
  TFile evtF(EvtFile.c_str(), "READ");
  TTree* evTr = (TTree*) evtF.Get(TreeName.c_str());
  
  evTr->SetBranchStatus("*",0);

  std::vector<float> curEvt(variables.size(), 0.0);

  // Bind tree branches to the container.
  for(size_t i = 0; i < variables.size(); i++)
  {
    // Activate branches
    evTr->SetBranchStatus( variables[i].c_str(), 1);
    
    // Bind
    evTr->SetBranchAddress( (variables[i]).c_str(), &(curEvt[i]));
  }

  std::map<std::string, float> result;

  int numberOfEvt = evTr->GetEntriesFast();
  //numberOfEvt = 3;
  size_t misCl = 0;
  
  for(int ev = 0; ev < numberOfEvt; ev++)
  {
    evTr->GetEntry(ev);
    
    //cls.GetMvaValues(curEvt, result);
    //printResult (result);

    std::string* resStr = cls.Classify(curEvt);
    if( *resStr != TreeName)
    {
      misCl++;
    }
    delete resStr;
  }

  // Classifier evaluation info.
  std::cout << "+++++++++++++++++++++++++++++++++++++++\n" 
	    << " Total number of classified events: "
	    << numberOfEvt << '\n'
	    << " Number of missclassified: " << misCl << " = "
	    << ( static_cast<float>(misCl) * 100.00)/ static_cast<float>(numberOfEvt)
	    <<" %\n"
	    << " Correct cassified = " << (numberOfEvt - misCl)
	    << "\n+++++++++++++++++++++++++++++++++++++++\n";

  return 0;
}
