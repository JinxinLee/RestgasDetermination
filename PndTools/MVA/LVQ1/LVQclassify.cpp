/* ***************************************
 * Author: M. Babai                      *
 * M.Babai@rug.nl                        *
 * ***************************************
 */
/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the LVQ
 * algorithm.
 */
// FIXME Create Dist histograms.

#include "PndLVQClassify.h"
// C++
#include <fstream>
// ROOT
#include "TStopwatch.h"

// Print the results map.
void printResult(std::map<std::string,float>& res)
{
  std::cout << "\n================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin();
       ii != res.end(); ++ii)
  {
    std::cout << (*ii).first << " => " << (*ii).second << '\n';
  }
  std::cout << "======================================= \n";
}

// Read the events features from a given file
void readEvents(const char* infile, const std::vector<std::string>& varNames,
		const std::vector<std::string>& classNames, 
		std::vector<std::pair<std::string, std::vector<float>*> >& coNt)
{
  coNt.clear();
  TFile inf(infile,"READ");
  
  // Class Loop
  for(size_t cls = 0; cls < classNames.size(); cls++)
  {
    // Tree name
    const char *name = classNames[cls].c_str();
    
    // Get the tree object
    TTree *t = (TTree*) inf.Get(name);
    if(!t)
    {
      std::cerr << "Could not find tree named: " << name << std::endl;
      return;
    }
    // Init a container to bind to the tree branches
    std::vector<float> ev (varNames.size(), 0.0);

    // Bind the parameters to the tree branches
    for(size_t j = 0; j < varNames.size(); j++)
    {
      const char* branchName = varNames[j].c_str();
      //Binding the branches
      t->SetBranchAddress(branchName, &(ev[j]));
    }// Tree parameters are bounded

    // Fetch and store the variables to per class variable container
    for(int k = 0; k < t->GetEntriesFast(); k++)
    {
      t->GetEntry(k);

      // Container to store the vent data read from the input tree
      std::vector<float>* EvtDat = new std::vector<float>();

      // Var Loop
      for(size_t idx = 0; idx < varNames.size(); idx++)
      {
        EvtDat->push_back(ev[idx]);
      }// Var Loop
      
      // Store the event and its class name
      coNt.push_back(std::make_pair(classNames[cls], EvtDat));
    }
    // We are done and can delete the tree pointer
    delete t;
  }// Class Loop
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */
int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "\t<Usage> ./classify <InputFile with Prototypes>"
	      <<" <Classify EventFile> <Results OutPutFile>"
	      << std::endl;
    return 1;
  }
  
  std::string inF  = argv[1];
  std::string evtF = argv[2];
  std::string outF = argv[3];

  std::cout << "\tClassifying events from " << evtF << '\n'
	    << "\tUsing prototypes from " << inF << '\n'
	    << "\tThe outoput will be stored in txt format in "<< outF 
	    << '\n';

  // Create variables
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  std::map<std::string, float> res;
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Classes
  clas.push_back("electron");
  clas.push_back("pion");
  
  // Variables
  //nam.push_back("p");
  nam.push_back("emc");
  nam.push_back("lat");
  nam.push_back("z20");
  nam.push_back("z53");

  //nam.push_back("thetaC");
  //nam.push_back("tof"); 
  //nam.push_back("stt");
  //nam.push_back("mvd"); 
  
  // Create classifier.
  PndLVQClassify cls (inF, clas, nam);
  
  // Read events.
  readEvents(evtF.c_str(), nam, clas, events);
  
  std::cout << "Total number of events is " << events.size() << '\n';
  
  std::ofstream OutPut;
  OutPut.open (outF.c_str());
  OutPut << "# Classification results for the events from\n"
	 << "# "<< evtF << '\n'
	 << "# Total number of events was " << events.size() << '\n';
  
  TStopwatch timer;
  timer.Start();
  
  // Class loop
  for(size_t cl = 0; cl < clas.size(); cl++)
  {
    // Current class Name
    std::string curClsName = clas[cl];
    int correctCls = 0;
    int wrongCls = 0;
    int totNumEvt = 0;

    // Events Loop
    for(size_t k = 0; k < events.size(); k++)
    {
      if( curClsName == (events[k]).first )
      {
	std::vector<float>* evt = (events[k]).second;
	// Get Mva Value
	cls.GetMvaValues(*evt, res);
	totNumEvt++;
	
	// Perform winner takes all.
	std::string* tmpClsName = cls.Classify(*evt);

	// Store the results
	OutPut<< "======================================= \n";
	OutPut << "# Event " << k 
	       << " Original className " << (events[k]).first
	       << "\n Classifier output name " << *tmpClsName << '\n';

	
	for( std::map<std::string,float>::iterator it = res.begin(); 
	     it != res.end(); ++it)
	{
	  OutPut << (*it).first << " => " << (*it).second
		 << " ";
	}
	OutPut<< "\n======================================= \n";

	if(*tmpClsName == curClsName)
	{// Correct Label
	  correctCls++;
	}
	else{// Wrong label classification.
	  wrongCls++;
	}
      delete tmpClsName;
      }// End if
    }// Events Loop

    OutPut << "++++++++++++++ Results for classification of " << curClsName 
	   << "+++++++++++++++++++++++++++++++++++++++++++++\n"
	   << "We have seen " << totNumEvt << " Events in this class\n"
	   << "Number of Correct classified events = " << correctCls 
	   << "\nNumber of mis-classified events = " <<  wrongCls
	   << "\nErro = " 
	   << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>(totNumEvt)) << " %\n" ;
  }// CLass Loop

  timer.Stop();

  // Close Open file
  OutPut.close();

  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout << "Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds\n"
	    << "It took " << (rtime/static_cast<double>(events.size())) << " Per event.\n";

  // Clean up
  std::cout << "Clean up.\n";
  for(size_t i = 0; i < events.size(); ++i)
  {
    delete (events[i]).second;
  }
  events.clear();
  res.clear();
  return 0;
}
