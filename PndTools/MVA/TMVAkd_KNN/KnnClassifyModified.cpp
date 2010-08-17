// Define the maximum number of neighbors.
#define MAX_NUM_NEIGH 20 //650
#define MIN_NUM_NEIGH 10

// C++ headers
#include <sstream>
#include <iomanip>

// Local headers
#include "PndKnnClassify.h"

// Root and PandaRoot.
#include "TNtuple.h"

void printResult( std::map<std::string,float>& res, unsigned int evtId){
  std::cout << "\t==================================" << std::endl;
  std::cout << " Evt Num = " << evtId << std::endl;
  
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii)
  {
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second << std::endl;
  }
  
  std::cout << "\t==================================" << std::endl;
}

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 3){
    std::cerr << "\t<ERROR>" 
	      << "./classify <treeName to be classified> <OutputFileName>"
	      << std::endl;
    return 1;
  }
  
  // Init input variables.
  std::string EvtTreeName = argv[1];
  std::string OutFileName = argv[2];

  std::string InPutFileName = "/media/daq/babaiexp/ParamFiles/VarXNormalizedJun23_10_EvtFeatSmallSet.root";
  std::string InputEvents = "/media/daq/babaiexp/ParamFiles/10_4TestSetParamsCharged.root";

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
  //vars.push_back("p");
  vars.push_back("emc");
  vars.push_back("z20");
  vars.push_back("z53");
  vars.push_back("lat");
  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("tof");
  //vars.push_back("stt"); 
    
  // Open input events file.
  TFile inFile(InputEvents.c_str(), "READ");
  
  // Prepare events to be classified.
  TNtuple* events = (TNtuple*) inFile.Get(EvtTreeName.c_str());
  // TObjArray* Namen = events->GetListOfBranches();
  
  std::vector<float> curEvt(vars.size(), 0.0);
  
  // Bind tree branches to the container.
  for(size_t i = 0; i < vars.size(); i++){
    events->SetBranchAddress( (vars[i]).c_str(), &(curEvt[i]));
  }
  
  //Create the classifier object and specify the weight file
  PndKnnClassify cls (InPutFileName, clasNames, vars);
  // Set classifier parameters and init.
  cls.SetEvtParam(0.8,1.0);
  cls.InitKNN();

  // Open OutputFile.
  std::ofstream Outfile;
  Outfile.open(OutFileName.c_str(), std::ios::out| std::ios::trunc);
  Outfile << "# +++++++++++++++++++++++++++++++++++++++" 
	  << std::endl
	  << "# Total number of "<< EvtTreeName << " classified events: "
	  << events->GetEntriesFast()
	  << std::endl
	  << "#<neighb>\t<missclassified>\t<%>" << std::endl;
    
  int nm = MIN_NUM_NEIGH;
  while ( nm <= MAX_NUM_NEIGH)
  {
    // Set number of neighbors.
    cls.SetKnn(nm);
    
    // Map to store the results
    //std::map<std::string, float> res;

    // Number of misclassified.
    unsigned int misCl = 0;

    // Perform classification of the available events.
    for(int ev = 0; ev < events->GetEntriesFast(); ev++){
      events->GetEntry(ev);

      std::string resStr = cls.Classify(curEvt);
      
      if( resStr != EvtTreeName){
	misCl++;
      }
    }
    // Write to output.
    // Classifier evaluation info.
    Outfile <<"  " << nm << "\t" << misCl << "\t"
	    << ( static_cast<float>(misCl) * 100.00)/ static_cast<float>(events->GetEntriesFast())
	    << std::endl;
    // Incremtn num neighbors. 
    nm = nm + 10;
  }
  Outfile << "+++++++++++++++++++++++++++++++++++++++" 
	  << std::endl;

  // Close open file
  inFile.Close();
  Outfile.close();

  return 0;
}
