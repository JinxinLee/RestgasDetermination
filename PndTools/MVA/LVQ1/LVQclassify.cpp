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
#include "TFile.h"
#include "TH1.h"
#include "TStopwatch.h"

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

// Read the events features from a given file
void readEvents(const char* infile, std::vector<std::string> const& varNames,
		std::vector<std::string> const& classNames, 
		std::vector<std::pair<std::string, std::vector<float>*> >& coNt)
{
  coNt.clear();
  TFile inf(infile,"READ");
  
  // Class Loop
  for(size_t cls = 0; cls < classNames.size(); cls++)
  {
    // Tree name
    char const* name = classNames[cls].c_str();
    
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
      char const* branchName = varNames[j].c_str();
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
  std::string outHistFile = "Hists_" + outF;

  std::cout << "\tClassifying events from " << evtF << '\n'
	    << "\tUsing prototypes from " << inF << '\n'
	    << "\tThe outoput will be stored in txt format in "<< outF 
	    << '\n';

  // Create variables
  std::vector<std::string> clas;
  std::vector<std::string> nam;
  std::map<std::string, float> res;
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  std::map<std::string, TH1F*> histograms;
  
  // Classes (labels)
  clas.push_back("electron");
  clas.push_back("pion");
  //clas.push_back("kaon");
  //clas.push_back("muon");
  //clas.push_back("proton");

  // Variables names
  nam.push_back("emc");
  nam.push_back("lat");
  nam.push_back("z20");
  nam.push_back("z53");

  //nam.push_back("thetaC");
  //nam.push_back("tof"); 
  //nam.push_back("stt");
  //nam.push_back("mvd"); 
  
  // Init histograms.
  for(size_t i = 0; i < clas.size(); ++i)
  {
    std::string des = "DescriptionOf" + clas[i];
    TH1F* h1 = new TH1F(clas[i].c_str(), des.c_str(), 200, 0.0, 2.0);
    histograms.insert(std::make_pair(clas[i], h1));
  }

  // Create classifier.
  PndLVQClassify cls (inF, clas, nam);

  cls.Initialize();  
  
  // Read events.
  readEvents(evtF.c_str(), nam, clas, events);
  
  std::cout << "Total number of events is " << events.size() << '\n';
  
  std::ofstream OutPut;
  OutPut.open (outF.c_str());
  OutPut << "# Classification results for the events from\n# "
	 << evtF << "\n# Total number of events was " << events.size()
	 << "\n\n";
  
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
	/*
	  OutPut<< "======================================= \n";
	  OutPut << "# Event " << k 
	  << " Original className " << (events[k]).first
	  << "\n Classifier output name " << *tmpClsName << '\n';
	*/
	
	for( std::map<std::string,float>::iterator it = res.begin(); 
	     it != res.end(); ++it)
	{
	  /*
	    OutPut << (*it).first << " => " << (*it).second
	    << " ";
	  */
	  std::string nn = (*it).first;
	  (histograms[nn])->Fill((*it).second);
	}
	//OutPut<< "\n======================================= \n";

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
    std::cout << "\t<-I-> Writing results for " << curClsName
	      << '\n';
    OutPut << "++++++++++++++ Classification Results ++++++++++++\n"
	   << "\tCurrent class Name " << curClsName
	   << "\n\tWe have seen " << totNumEvt << " Events in this class\n"
	   << "\tNumber of Correct classified events = " << correctCls
	   << "\n\tNumber of mis-classified events = " <<  wrongCls
	   << "\n\tErro = "
	   << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>(totNumEvt))
	   << " %\n" ;
  }// CLass Loop

  timer.Stop();

  // Close Open file
  OutPut.close();

  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout << "Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds\n"
	    << "It took " << (rtime/static_cast<double>(events.size()))
	    << " Per event.\n";

  // Clean up
  std::cout << "Clean up.\n";
  for(size_t i = 0; i < events.size(); ++i)
  {
    delete (events[i]).second;
  }
  events.clear();
  res.clear();

  TFile routf(outHistFile.c_str(), "RECREATE");
  
  for(size_t h = 0; h < clas.size(); ++h)
  {
    std::string nn = clas[h];
    (histograms[nn])->Write();
  }
  routf.Close();

  for(size_t h = 0; h < clas.size(); ++h)
  {
    std::string nn = clas[h];
    delete (histograms[nn]);
  }
  histograms.clear();

  return 0;
}
