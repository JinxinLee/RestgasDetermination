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
#include <iomanip>

// Local
#include "PndPrzWindowClassify.h"

// ROOT & PandaRoot
#include "TFile.h"
#include "TNtuple.h"
#include "TStopwatch.h"


void printResultMap(std::map<std::string,float>& res)
{
  std::cout << "\n\t================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii)
  {
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second << '\n';
  }
  std::cout << "\n\t================================== \n";
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
	      << argv[0] <<" <inputWeightFile> <InputEventsFile>"
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
  std::map<std::string, float> wsize;
  
  // Classes (container to hold the class names)
  clasNames.push_back("electron");
  clasNames.push_back("pion");
  clasNames.push_back("kaon");
  clasNames.push_back("muon");
  clasNames.push_back("proton");
  
  // Variables (names)
  //vars.push_back("p");
  //wsize["p"]   = 1.6;
  
  vars.push_back("emc");
  wsize["emc"] = 2.4;
  
  vars.push_back("lat");
  wsize["lat"] = 5.0;
  
  vars.push_back("z20");
  wsize["z20"] = 5.0;
  
  vars.push_back("z53");
  wsize["z53"] = 5.0;

  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("tof");
  //vars.push_back("stt"); 
  
  TStopwatch timer;
  timer.Start();
  
  //Create the classifier object and specify the weight file
  PndPrzWindowClassify cls (InPutFile, clasNames, vars);
  cls.Initialize();

  // Set classifier parameters and init.
  cls.setWindowSize(wsize);
  
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
  
  std::vector<float> curEvt(vars.size(), 0.0);
  
  // Bind tree branches to the container.
  for(size_t i = 0; i < vars.size(); i++)
  {
    events->SetBranchAddress( (vars[i]).c_str(), &(curEvt[i]));
  }
  
  // Map to store the results
  std::map<std::string, float> res;
  
  // Reste and start the timer.
  timer.Reset();
  timer.Start();
  
  std::cout << "<INFO> Classification.\n";
  
  // ___________ Classification ________
  unsigned int misCnt = 0;
  int totNumEvt = 10;//events->GetEntriesFast();
  
  // Open OutputFile.
  std::ofstream Outfile;
  Outfile.open(OutPutFile.c_str(), std::ios::out| std::ios::trunc);
  
  Outfile << "# =========================================================\n"
	  << "# Classification output of a parzenwindow based classifier.\n"
	  << "# Total number of test events = " << totNumEvt
	  << "\n# treename = " << EvtTreeName << '\n'
	  << "# Init Window edges\n";
  
  for(size_t i = 0; i < vars.size(); i++)
  {
    Outfile << "# " << vars[i] << " = " << wsize[vars[i]] << '\n';
  }
  Outfile << "\n# =========================================================\n"
	  << "# Using vaiables: ";
  
  for(size_t i = 0; i < vars.size(); i++)
  {
    Outfile << vars[i] << " ";
  }
  Outfile << "\n# =========================================================\n"; 
  
  // ___________ Classify input events ________
  for(int ev = 0; ev < totNumEvt; ev++)
  {
    events->GetEntry(ev);
    //cls.GetMvaValues(curEvt, res);
    std::string* Winner = cls.Classify(curEvt);
    
    if( (*Winner) != EvtTreeName )
    {
      misCnt++;
    }
    delete Winner;
    //printResultMap(res);
  }
  Outfile << std::setprecision(5) << "# Number of Missclassified events = " << misCnt
	  << " "<< ( static_cast<float>(misCnt * 100)/static_cast<float>(totNumEvt) )
	  << " %\n";

  // Close open file
  inFile.Close();
  Outfile.close();
  
  timer.Stop();
  rtime = timer.RealTime();
  ctime = timer.CpuTime();
  std::cout << "==============================================\n"
	    << "<INFO> Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds.\n\n";
  
  return 0;
}
