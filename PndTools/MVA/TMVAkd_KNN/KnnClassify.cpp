/* ***************************************
 * Author: M.Babai@rug.nl                *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the KNN
 * algorithm. An implementation of kd-tree is used to improve the
 * recognition performance.
 */

// C++ headers
#include <sstream>

// Local headers
#include "PndKnnClassify.h"

// Root and PandaRoot.
#include "TStopwatch.h"
#include "TNtuple.h"

void printResult( std::map<std::string,float>& res, unsigned int evtId)
{
  std::cout << "\t==================================\n"
	    << " Evt Num = " << evtId
	    << '\n';
  
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii)
  {
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second
	      << '\n';
  }
  
  std::cout << "\t==================================\n";
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 5)
  {
    std::cerr << "\t<ERROR>" 
	      << argv[0] << " <inputWeightFile> <InputEventsFile>"
	      << " <Treename> <numOfneigh>"
	      << std::endl;
    return 1;
  }
  
  // Init input variables.
  std::string InPutFileName = argv[1];
  std::string InputEvents   = argv[2];
  std::string EvtTreeName   = argv[3];
  std::string NumNeistr     = argv[4];
  
  // Convert to int.
  std::istringstream buff(NumNeistr);
  unsigned int NumNei = 0;
  buff >> NumNei;
  
  // Containers to hold labels and variable names.
  std::vector<std::string> clasNames;
  std::vector<std::string> vars;
  
  // Classes (container to hold the class names)
  clasNames.push_back("electron");
  clasNames.push_back("pion");
  clasNames.push_back("kaon");
  clasNames.push_back("muon");
  clasNames.push_back("proton");
  
  // Variables (names)
  //vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  
  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("stt"); 
  
  TStopwatch timer;
  timer.Start();
  
  //Create the classifier object and specify the weight file
  PndKnnClassify cls (InPutFileName, clasNames, vars);

  // Set classifier parameters and init.
  cls.SetEvtParam(0.8,1.0);
  cls.SetKnn(NumNei);
  cls.Initialize();

  
  std::cout << ".......... Init is done.\n";
  
  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();

  std::cout << "<INFO> Initialization time:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds.\n";
  
  // Open input events file.
  TFile inFile(InputEvents.c_str(), "READ");
  
  // Prepare events to be classified.
  TNtuple* events = (TNtuple*) inFile.Get(EvtTreeName.c_str());
  
  // Deactivate all branches
  events->SetBranchStatus("*",0);
  
  std::vector<float> curEvt(vars.size(), 0.0);
  
  // Bind tree branches to the container.
  for(size_t i = 0; i < vars.size(); i++)
  {
    // Activate branches
    events->SetBranchStatus( vars[i].c_str(), 1);
    
    // Bind
    events->SetBranchAddress( (vars[i]).c_str(), &(curEvt[i]));
  }
  
  // Map to store the results
  std::map<std::string, float> res;
  
  // Reste and start the timer.
  timer.Reset();
  timer.Start();
  
  // Perform classification of the available events.
  unsigned int misCl = 0;
  int numberOfEvt = events->GetEntriesFast();
  
  //numberOfEvt = 20;
  
  for(int ev = 0; ev < numberOfEvt; ev++)
  {
    events->GetEntry(ev);
    
    //cls.GetMvaValues(curEvt, res);
    //printResult(res, ev);
    
    std::string* resStr = cls.Classify(curEvt);
    if( *resStr != EvtTreeName)
    {
      misCl++;
    }
    delete resStr;
  }
  
  timer.Stop();
  rtime = timer.RealTime();
  ctime = timer.CpuTime();
  
  std::cout << "<INFO> Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = "
	    << ctime <<" Seconds.\n\n";
  
  // Classifier evaluation info.
  std::cout << "+++++++++++++++++++++++++++++++++++++++\n" 
	    << " Total number of classified events: "
	    << numberOfEvt << '\n'
	    << " Number of missclassified: " << misCl << " = "
	    << ( static_cast<float>(misCl) * 100.00)/ static_cast<float>(numberOfEvt)
	    <<" %\n"
	    << " Correct cassified = " << (numberOfEvt - misCl)
	    << "\n (time / event) = " << rtime/ static_cast<double>(numberOfEvt)
	    << "\n With #neighb = " << NumNei 
	    << "\n+++++++++++++++++++++++++++++++++++++++\n";

  // Close open file
  inFile.Close();
  
  return 0;
}
