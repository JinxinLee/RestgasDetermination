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

#include "PndLVQClassify.h"
// C++
#include <fstream>
// ROOT
#include "TStopwatch.h"


void printResult(std::map<std::string,float>& res){
  std::cout << "\n================================== \n";
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii){
    std::cout << (*ii).first << " => " << (*ii).second << std::endl;
  }
  std::cout << "======================================= \n";
}

void readEvents(const char* infile, const std::vector<std::string>& varNames,
		const std::vector<std::string>& classNames, 
		std::vector<std::pair<std::string, std::vector<float>*> >& coNt)
{
  coNt.clear();
  TFile inf(infile,"READ");
  
  // Class Loop
  for(unsigned int cls = 0; cls < classNames.size(); cls++){
    // Tree name
    const char *name = classNames[cls].c_str();
    
    // Get the tree object
    TTree *t = (TTree*) inf.Get(name);

    // Init a container to bind to the tree branches
    std::vector<float> ev (varNames.size(), 0.0);

    // Bind the parameters to the tree branches
    for(unsigned int j = 0; j < varNames.size(); j++){
      const char* branchName = varNames[j].c_str();
      //Binding the branches
      t->SetBranchAddress(branchName, &(ev[j]));
    }// Tree parameters are bounded

    // Fetch and store the variables to per class variable container
    for(unsigned int k = 0; k < t->GetEntriesFast(); k++){
      t->GetEntry(k);

      // Container to store the vent data read from the input tree
      std::vector<float>* EvtDat = new std::vector<float>();

      // Var Loop
      for(unsigned int idx = 0; idx < varNames.size(); idx++){
        EvtDat->push_back(ev[idx]);
      }// Var Loop
      
      // Store the event and its class name
      coNt.push_back(std::make_pair(classNames[cls], EvtDat));
    }
    // We are done and can delete the tree pointer
    delete t;
  }// Class Loop
}

std::string& classifyEvent(const std::vector<std::string>& clas, 
			   std::map<std::string, float>& res)
{
  float clsVal = std::numeric_limits<float>::max();
  std::string clsName;
  for(unsigned int i = 0; i < clas.size(); i++){
    std::string name = clas[i];
    float val = res[name];
    
    if(val < clsVal){
      clsVal = val;
      clsName = name;
    }
  }
  return *(new std::string(clsName));
}
/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{

  if(argc < 4){
    std::cerr << "\t<Usage> ./classify <InputFile with Prototypes>"
	      <<" <To classify EventFile> <Results OutPutFile>"
	      << std::endl;
    return 1;
  }
  
  std::string inF  = argv[1];
  std::string evtF = argv[2];
  std::string outF = argv[3];

  std::cout << "\tClassifying events from " << evtF << std::endl
	    << "\tUsing prototypes from " << inF << std::endl
	    << "\tThe outoput will be stored in txt format in "<< outF 
	    << std::endl;
  // Create variables
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  std::map<std::string, float> res;
  std::map<std::string, int> TotRes;
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Classes
  clas.push_back("Elect"); clas.push_back("Pion"); clas.push_back("Kaon");
  //clas.push_back("Muon"); //clas.push_back("gam");
  
  // Variables
  nam.push_back("p"); nam.push_back("emc");
  nam.push_back("thetaC");
  //nam.push_back("tof"); 
  //nam.push_back("stt"); nam.push_back("mvd"); 

  // Create classifier.
  PndLVQClassify cls (inF.c_str(), clas, nam);

  // Read events.
  readEvents(evtF.c_str(), nam, clas, events);

  std::cout << "Total number of events is " << events.size() << std::endl;

  std::ofstream OutPut;
  OutPut.open (outF.c_str());
  OutPut << "# Classification results for the events from\n"
	 << "# "<< evtF << std::endl 
	 << "# Total number of events was " << events.size() << std::endl;

  TStopwatch timer;
  timer.Start();

  std::string tmpName;
  for(unsigned int k = 0; k < events.size(); k++)
  {  
    std::vector<float>* evt = (events[k]).second;
    cls.Classify(*evt, res);
    
    // Store the results
    OutPut<< "======================================= \n";
    OutPut << "# Event " << k 
	   << " Original className " << (events[k]).first << std::endl;
    for( std::map<std::string,float>::iterator it = res.begin(); 
	 it != res.end(); ++it){
      OutPut << (*it).first << " => " << (*it).second
	     << std::endl;
    }
    OutPut<< "======================================= \n";
    tmpName = classifyEvent(clas, res);
    TotRes[tmpName] += 1;
  }
  OutPut << std::endl <<" TOTAL RESULTS " << std::endl;
  for( std::map<std::string, int>::iterator it = TotRes.begin(); 
       it != TotRes.end(); ++it){
    OutPut << (*it).first << " => " << (*it).second
	   << std::endl;
  }
  /*
    cls.Classify(evt,res);
    printResult(res);
  */


  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout << "Classifier timing results:"<<std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds" << std::endl;

  // Clean up
  std::cout << "Clean up." << std::endl;
  for(unsigned int i =0; i < events.size(); i++){
    delete (events[i]).second;
  }
  events.clear();
  res.clear();
  TotRes.clear();
  OutPut.close();
  return 0;
}
