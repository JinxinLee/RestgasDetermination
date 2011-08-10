// Define the maximum number of neighbors.
#define MIN_NUM_NEIGH 10
#define MAX_NUM_NEIGH 90 //650

// C++ headers
#include <iomanip>

// Local headers
#include "PndKnnClassify.h"

// Root and PandaRoot.
#include "TNtuple.h"

void printResult( std::map<std::string,float>& res, unsigned int evtId)
{
  std::cout << "\t==================================\n"
	    << " Evt Num = " << evtId << '\n';
  std::map<std::string,float>::iterator ii;
  for( ii=res.begin(); ii != res.end(); ++ii)
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
    std::cerr << "\t<ERROR>:\n\t"
	      << argv[0]
	      << " <Weights> <EventFile> <treeName> <OutputFileName>"
	      << std::endl;
    return 1;
  }
  
  //_____ Init input variables.
  std::string InPutFileName = argv[1];// Weight File
  std::string InputEvents   = argv[2];// Events file
  std::string EvtTreeName   = argv[3]; // Treename
  std::string OutFileName   = argv[4];// OutPut Name

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
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");

  //vars.push_back("thetaC");
  //vars.push_back("mvd");
  //vars.push_back("tof");
  //vars.push_back("stt"); 
    
  // Open input events file.
  TFile inFile(InputEvents.c_str(), "READ");
  
  // Prepare events to be classified.
  TNtuple* events = (TNtuple*) inFile.Get(EvtTreeName.c_str());
  if(!events)
  {
    std::cerr << "<ERROR>: Could not read events for <"
	      << EvtTreeName << "> from the file."
	      << std::endl;
    exit(1);
  }
  std::vector<float> curEvt(vars.size(), 0.0);
  
  // Bind tree branches to the container.
  for(size_t i = 0; i < vars.size(); i++)
  {
    events->SetBranchAddress( (vars[i]).c_str(), &(curEvt[i]));
  }
  
  //Create the classifier object and specify the weight file
  PndKnnClassify cls (InPutFileName, clasNames, vars);
  cls.SetKnn(1);

  // Set classifier parameters and init.
  cls.SetEvtParam(0.8, 1.0);

  // Init the classifier.
  cls.Initialize();
  
  // Open OutputFile.
  std::ofstream Outfile;

  Outfile.open(OutFileName.c_str(), std::ios::out| std::ios::trunc);
  Outfile << "# +++++++++++++++++++++++++++++++++++++++\n"
	  << "# Total number of "<< EvtTreeName
	  << " classified events: " << events->GetEntriesFast()
	  << "\n#<neighb>\t<missclassified>\t<%>\n";
    
  size_t nm = MIN_NUM_NEIGH;
  while ( nm <= MAX_NUM_NEIGH )
  {
    // Set number of neighbors.
    cls.SetKnn(nm);
    
    std::cout << "-I- NUM. Neigh. = " << nm << '\n';

    // Number of misclassified.
    unsigned int misCl = 0;

    // Perform classification of the available events.
    for(int ev = 0; ev < events->GetEntriesFast(); ev++)
    {
      events->GetEntry(ev);
      
      std::string* resStr = cls.Classify(curEvt);
      
      if( (*resStr) != EvtTreeName)
      {
	misCl++;
      }
      delete resStr;
    }
    // Write to output.
    // Classifier evaluation info.
    Outfile <<"  " << nm
	    << "\t" << misCl << "\t"
	    << ( static_cast<float>(misCl) * 100.00)/ static_cast<float>(events->GetEntriesFast())
	    << '\n';
    // Incremtn num neighbors. 
    nm += 10;
  }
  Outfile << "+++++++++++++++++++++++++++++++++++++++\n";

  // Close open file
  inFile.Close();
  Outfile.close();

  return 0;
}
