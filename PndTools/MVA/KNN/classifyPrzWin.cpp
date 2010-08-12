/********************
 * Author: M. Babai *
 * M.Babai@rug.nl   *
 ********************
 */

/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the Parzen
 * Window algorithm.
 */

// C++
#include <fstream>

// LOcal
#include "PndPrzWindowClassify.h"

// ROOT & PandaRoot
#include "TFile.h"
#include "TNtuple.h"
#include "TStopwatch.h"


void printResultMap(std::map<std::string,float>& res){
  std::cout << "\n\t================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second << std::endl;
  }
  std::cout << "\n\t================================== \n";
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 5){
    std::cerr << "\t<ERROR>" 
	      << "./classifyPrzWin <inputWeightFile> <InputEventsFile>"
	      << " <TreeName> <OutPutLogFile>"
	      << std::endl;
    return 1;
  }
  
  // Weights.
  std::string InPutFile   = argv[1];
  
  // Events to classify.
  std::string InputEvents = argv[2];
  
  // Events tree name.
  std::string EvtTreeName = argv[3];

  // File to write output.
  std::string OutPutFile  = argv[4];

  // Containers to hold labels and variable names.
  std::vector<std::string> clasNames;
  std::vector<std::string> vars;
  
  // Classes (container to hold the class names)
  clasNames.push_back("electron");
  clasNames.push_back("pion");
  //clasNames.push_back("kaon");
  //clasNames.push_back("muon");
  //clasNames.push_back("proton");
  //clasNames.push_back("gamma");
  
  // Variables (names)
  vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");
  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("tof");
  //vars.push_back("stt"); 
  
  TStopwatch timer;
  timer.Start();
  
  //Create the classifier object and specify the weight file
  PndPrzWindowClassify cls (InPutFile, clasNames, vars);

  // Set classifier parameters and init.
  std::map<std::string, float> wsize;
  wsize["p"]   = 0.5;
  wsize["emc"] = 0.5;
  wsize["lat"] = 0.5;
  wsize["z20"] = 0.5;
  wsize["z53"] = 0.5;
  
  cls.setWindowSize(wsize);

  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout << "<INFO> Initialization time:" << std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;

  // Open input events file.
  TFile inFile(InputEvents.c_str(), "READ");

  // Prepare events to be classified.
  TNtuple* events = (TNtuple*) inFile.Get(EvtTreeName.c_str());
  
  std::vector<float> curEvt(vars.size(), 0.0);
  
  // Bind tree branches to the container.
  for(size_t i = 0; i < vars.size(); i++){
    events->SetBranchAddress( (vars[i]).c_str(), &(curEvt[i]));
  }
  
  // Map to store the results
  std::map<std::string, float> res;
  
  // Reste and start the timer.
  timer.Reset();
  timer.Start();
  
  // Open OutputFile.
  std::ofstream Outfile;
  Outfile.open(OutPutFile.c_str(), std::ios::out| std::ios::trunc);
  
  Outfile << "# ========================================================="
	  << std::endl
	  << "# Classification output of a parzenwindow based classifier."
          << std::endl
	  << "# Total number of test events = " << events->GetEntriesFast()
	  << std::endl
	  << "# treename = " << EvtTreeName << std::endl
	  << "# Window edges" << std::endl;
  
  for(size_t i = 0; i < vars.size(); i++){
    Outfile << "# " << vars[i] << " = " << wsize[vars[i]] << std::endl;
  }
  Outfile << "# =========================================================" 
	  << std::endl;
  
  std::cout << "<INFO> Classification." << std::endl;

  // Classify input events.
  unsigned int misCnt = 0;
  
  // for(int ev = 0; ev < 100; ev++){
  for(int ev = 0; ev < events->GetEntriesFast(); ev++){
    events->GetEntry(ev);
    
    //cls.GetMvaValues(curEvt, res);
    if(cls.Classify(curEvt) != EvtTreeName){
      misCnt++;
    }
    //printResultMap(res);
  }
  
  Outfile << " number of misclassified = " << misCnt
	  << " = " 
	  << ( static_cast<float>(misCnt * 100)/static_cast<float>(events->GetEntriesFast()) )
	  << " %"<< std::endl;
  
  // Close open file
  inFile.Close();
  Outfile.close();
  
  timer.Stop();
  rtime = timer.RealTime();
  ctime = timer.CpuTime();
  std::cout << "=============================================="
	    << std::endl
	    << "<INFO> Classifier timing results:"
	    << std::endl
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds.\n" << std::endl;
  
  return 0;
}
