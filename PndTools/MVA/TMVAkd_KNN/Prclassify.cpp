/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the KNN
 * algorithm. An implementation of kd-tree is used to improve the
 * recognition performance.
 */
// C++
#include <sstream>
#include <iomanip>

// PANDA ROOT
#include "PndProjectedKNN.h"

// ROOT
#include "TFile.h"
#include "TNtuple.h"
#include "TStopwatch.h"


void PrintResult(std::map<std::string, float>& res, size_t evNum){
  std::cout << "\n\t================================== \n"
	    << "Evt num = " << evNum << std::endl;
  for( std::map<std::string, float>::iterator ii=res.begin(); 
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
    std::cerr <<"\t<ERROR> "
	      << argv[0] << " <inputFileWeight> <inputEventsFile> "
	      << "<TreeName> <numOfneigh>"
	      << std::endl;
      return 1;
  }
  
  std::string InPutFileName = argv[1];
  std::string InputEvtFile  = argv[2];
  std::string TreeName      = argv[3];
  std::string NumNeistr     = argv[4];

  std::istringstream buff(NumNeistr);
  int NumNei = 0;
  buff >> NumNei;

  std::vector<std::string> clas;
  std::vector<std::string> stru;

  std::vector< std::vector<std::string> > combination;

  std::vector<std::string> nam1;
  std::vector<std::string> nam2;
  std::vector<std::string> nam3;
 
  // Container to hold the class names (labels)
  clas.push_back("electron");
  clas.push_back("pion");
  //clas.push_back("kaon");
  //clas.push_back("muon");
  //clas.push_back("proton");
  
  // Event structure
  stru.push_back("p");
  stru.push_back("emc");
  stru.push_back("lat");
  stru.push_back("z20");
  stru.push_back("z53");

  // Variables (combinations)
  nam1.push_back("emc");
  nam1.push_back("lat");

  nam2.push_back("z20");
  nam2.push_back("z53");

  nam3.push_back("emc");
  nam3.push_back("lat");
  nam3.push_back("z20");
  nam3.push_back("z53");

  //combination.push_back(stru); 
  combination.push_back(nam1);
  combination.push_back(nam2);
  //combination.push_back(nam3);

  // Create the classifier object and specify the weight file
  PndProjectedKNN cls (InPutFileName, clas, stru, combination);

  cls.SetKnn(NumNei);
  cls.SetEvtParam(0.8, 1.0);
  cls.InitKNN();

  std::cout << "............... Init is done." << std::endl;
  
  // Open InputFile containing events for classification
  TFile inFile(InputEvtFile.c_str(), "READ");

  // Prepare events to be classified.
  TNtuple* events = (TNtuple*) inFile.Get(TreeName.c_str());
  
  // Deactivate Branches
  events->SetBranchStatus("*",0);

  std::vector<float> curEvt(stru.size(), 0.0);
  
  // Bind tree branches to the container.
  for(size_t i = 0; i < stru.size(); i++){
    // Activate branches
    events->SetBranchStatus( stru[i].c_str(), 1);
    // Bind
    events->SetBranchAddress( stru[i].c_str(), &(curEvt[i]));
  }
  
  // Map to store the results
  std::map<std::string, float> res;
  
  //==============================================
  // Perform classification of the available events.
  size_t misCl = 0;
  int numberOfEvt = events->GetEntriesFast();
  
  //numberOfEvt = 4;
  
  std::cout << "<INFO> Classifying " << numberOfEvt
	    << std::endl;
  
  // Start the timer
  TStopwatch ti;
  ti.Start();
  
  for(int ev = 0; ev < numberOfEvt; ev++){
    events->GetEntry(ev);
    
    //cls.GetMvaValues(curEvt, res);
    //PrintResult(res, ev);
    
    std::string* bla = cls.Classify(curEvt);    
    if((*bla) != TreeName){
      misCl++;
    }
    delete bla;
  }
  
  // Stop timer
  ti.Stop();

  //______________________________________________
  double rtime = ti.RealTime();
  double ctime = ti.CpuTime();
  std::cout << "timer 1: Classifier timing results:"<< std::endl;
  std::cout<< "RealTime = " << rtime << " seconds, CpuTime = " 
           << ctime <<" Seconds\n" << std::endl;

  // Classifier evaluation info.
  std::cout << "+++++++++++++++++++++++++++++++++++++++" 
	    << std::endl
	    << " Total number of classified events: "
	    << numberOfEvt << std::endl
	    << " Number of missclassified: " << misCl << " = "
	    << ( static_cast<float>(misCl) * 100.00)/ static_cast<float>(numberOfEvt)
	    <<" %"
	    << std::endl 
	    << " Correct cassified = " << (numberOfEvt - misCl)
	    << std::endl 
	    << " (time / event) = " << rtime/ static_cast<double>(numberOfEvt)
	    << std::endl 
	    << " With #neighb = " << NumNei 
	    << std::endl
	    << "+++++++++++++++++++++++++++++++++++++++" 
	    << std::endl;
  //==============================================
  
  // CLose open file.
  inFile.Close();
  
  return 0;
}
