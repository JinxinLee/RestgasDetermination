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
#define LVQ_CLS_DEBUG 0
#define LVQ_CLS_PRINT_ROC 0

// Local includes
#include "PndLVQClassify.h"
#include "PndMvaTools.h"

// C++
#include <fstream>

// ROOT
#include "TFile.h"
#include "TStopwatch.h"
#include "TH1.h"

/////////_______ Inline header ______///////////////////////////
// Prints the classification result to stdout.
void printResult(std::map<std::string, float> const& res);

/*
 * Reads the event data from the inputfile (inFile). The output is
 * stored in coNt. Per label counts are the return values.
 */
std::map<std::string, size_t>* readEvents(char const* infile,
					  std::vector<std::string> const& varNames,
					  std::vector<std::string> const& classNames,
					  std::vector<std::pair<std::string, std::vector<float>*> >& coNt);

/*
 * creates pairs of [fpRate, tpRate]
 * List of (LVQ output, label) decreasing.
 * Roc: List of (FP/N, TP/P)
 */

void ProduceROC(std::vector< ClassifierOutPuts >& input,
		std::string const& SigName,
		std::string const& BgName,
		size_t sigCnt, size_t bgCnt,
		std::vector< ROCPoints >& Roc);

/////////_______ Inline header ______///////////////////////////
//________________________________________________________________

void ProduceROC( std::vector< ClassifierOutPuts >& input,
		 std::string const& SigName,
		 std::string const& BgName,
		 size_t sigCnt, size_t bgCnt,
		 std::vector< ROCPoints >& Roc)
{
  float sg, bg;
  sg = bg = 0.0;

  if( (sigCnt > 0) && (bgCnt > 0) )
  {
    sg = static_cast<float>(sigCnt);
    bg = static_cast<float>(bgCnt);
  }
  else
  {
    std::cerr << "Signal OR Background count is zero\n";
    exit(EXIT_FAILURE);
  }
  
  float MinVal, MaxVal;
  MinVal = std::numeric_limits<float>::max();
  MaxVal = std::numeric_limits<float>::min();

  // We need to find Min and Max output for Signal.
  for(size_t i = 0; i < input.size(); ++i)
  {
    // MinVal
    if( (input[i]).clsOuts[SigName] < MinVal)
    {
      MinVal = (input[i]).clsOuts[SigName];
    }
    
    // MaxVal
    if( (input[i]).clsOuts[SigName] > MaxVal)
    {
      MaxVal = (input[i]).clsOuts[SigName];
    }
  }

  // We want to make 10 steps.
  float inc;
  inc = (MaxVal - MinVal )/10.00;

  // Add (0,0)
  Roc.push_back(ROCPoints());
  
  float trhold, fpRate, tpRate;
  float tnRate, fnRate;
  size_t fpCnt, tpCnt, fn, tn;

  trhold = MinVal;
  fpRate = tpRate = tnRate = fnRate = 0.00;
  fpCnt = tpCnt = fn = tn = 0;
  
  while( trhold <= MaxVal )
  {
    // Reset counters
    fpCnt = tpCnt = 0;
    fn    = tn    = 0;
    // Event loop (classification outputs)
    for(size_t k = 0; k < input.size(); ++k)
    {
      ClassifierOutPuts& a = input[k];
      
      //if( a.clsOuts[a.givenLabel] < trhold )
      if( a.clsOuts[SigName] <= trhold )
      {// In the Signal region (Assume signal)
	if( a.realLabel == SigName)
	{// True positief
	  tpCnt++;
	}
	else
	{// False positief
	  fpCnt++;
	}
      }// End of In region
      else// Out of region
      {
	if( a.realLabel == BgName)
	{// True negatief.
	  tn++;
	}
	else
	{// False negatief.
	  fn++;
	}
      }// End out of region
    }// Evt loop

    // True positief.
    tpRate = static_cast<float>(tpCnt)/sg;
    
    // False negatief.
    fpRate = static_cast<float>(fpCnt)/bg;
    
    // True negatief.
    tnRate = static_cast<float>(tn)/bg;
    
    // False negatief.
    fnRate = static_cast<float>(fn)/sg;
    
    // Add the current ROC point
    Roc.push_back(ROCPoints(fpRate, tpRate, tnRate, fnRate,
			    fpCnt, tpCnt, fn, tn, trhold));
    
    trhold += inc;
  }//While

#if LVQ_CLS_PRINT_ROC
  std::cout << "<-I-> DEBUG INFO\n  inc = "  << inc 
	    << " Min = " << MinVal
	    << " Max = " << MaxVal
	    << " tn  = " << tpCnt
	    << " fn  = " << fpCnt
	    << std::endl;
#endif

#if LVQ_CLS_DEBUG
  printRoc(Roc);
#endif
}

// Print the results map.
void printResult(std::map<std::string, float> const& res)
{
  std::cout << "\n================================== \n";
  for( std::map<std::string,float>::const_iterator ii=res.begin();
       ii != res.end(); ++ii)
  {
    std::cout << (*ii).first << " => " << (*ii).second << '\n';
    //std::cout << (*ii).first << " => " << (1 - (*ii).second) << '\n';
  }
  std::cout << "======================================= \n";
}

// Read the events from a given file
std::map<std::string, size_t>* readEvents(const char* infile, std::vector<std::string> const& varNames,
					  std::vector< std::string > const& classNames, 
					  std::vector< std::pair< std::string, std::vector<float>* > >& coNt)
{
  std::cout << "<-I-> Reading examples from " << infile
	    << '\n';
  // The file containing the examples.
  TFile inf(infile, "READ");

  // Holds the number of examples per label.
  std::map<std::string, size_t>* counts = new std::map<std::string, size_t>();

  // Class Loop
  for(size_t cls = 0; cls < classNames.size(); cls++)
  {
    // Tree name
    char const* name = classNames[cls].c_str();
    
    // Get the tree object
    TTree *t = (TTree*) inf.Get(name);
  
    if(!t)
    {
      std::cerr << "Could not find tree named: " << name 
		<< std::endl;
      delete counts;
      exit(EXIT_FAILURE);
    }
    
    // Get the counts for the current label
    size_t NumEvtCurLabel = static_cast<size_t>(t->GetEntriesFast());
    
    counts->insert( std::make_pair (classNames[cls], NumEvtCurLabel) );

    // Disable all branches
    t->SetBranchStatus("*",0);
    
    // Init a container to bind to the tree branches
    std::vector<float> ev (varNames.size(), 0.0);
    
    // Bind the parameters to the tree branches
    for(size_t j = 0; j < varNames.size(); j++)
    {
      char const* branchName = varNames[j].c_str();

      // Activate branches
      t->SetBranchStatus(branchName, 1);

      // Binding the branches
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

  // Close open file
  inf.Close();

  return counts;
}

/* ******************
 * Testing routine, *
 * ******************
 */
int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "\t<Usage>"<<  argv[0]
	      << " <InputFile with Prototypes>"
	      << " <Classify EventFile> <Results OutPutFile>"
	      << std::endl;
    return 1;
  }
  // Number of Proto types
  std::string inF  = argv[1];
  
  // Events to classify
  std::string evtF = argv[2];
  
  // Output results file
  std::string outF = argv[3];
  
  std::cout << "\tClassifying events from " << evtF << '\n'
	    << "\tUsing prototypes from " << inF << '\n'
	    << "\tThe outoput will be stored in txt format in "<< outF 
	    << '\n';
  
  // Labels.
  std::vector<std::string> labels;

  // Variables.
  std::vector<std::string> varNames;

  // Add labels
  labels.push_back("electron");
  labels.push_back("pion");
  
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");
  
  // Add Variables
  varNames.push_back("emc");
  varNames.push_back("lat");
  varNames.push_back("z20");
  varNames.push_back("z53");
  
  //varNames.push_back("thetaC");
  //varNames.push_back("tof"); 
  //varNames.push_back("stt");
  //varNames.push_back("mvd"); 

  // Create classifier.
  PndLVQClassify cls (inF, labels, varNames);

  // Init classifier
  cls.Initialize();
  
  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Read events to be classified.
  std::map<std::string, size_t>* counts = readEvents(evtF.c_str(), varNames,
						     labels, events);
  
  std::cout << "Total number of events to be classified = "
	    << events.size()
	    << '\n';

  // Start the timer.
  TStopwatch timer;
  timer.Start();

  // Map to store results.
  std::map<std::string, float> res;

  // Store classifier outputs per event.
  std::vector< ClassifierOutPuts > classifiedEvents;

  // Events loop
  for(size_t k = 0; k < events.size(); k++)
  {
    std::vector<float>* evt = (events[k]).second;

    // Get Mva Value
    cls.GetMvaValues( (*evt), res);

    // Do classification
    std::string* givenLabel = cls.Classify( (*evt) );

    // Store results.
    classifiedEvents.push_back(ClassifierOutPuts((events[k]).first, (*givenLabel), res));

    delete givenLabel;
  }// Events Loop

  // Print some timing information
  timer.Stop();  
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout << "Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds\n"
	    << "It took " << (rtime/static_cast<double>(events.size()))
	    << " Per event.\n";  
  
  // We are done with events vector. Cleaning
  std::cout << "Clean up Events.\n";
  for(size_t i = 0; i < events.size(); ++i)
  {
    delete (events[i]).second;
  }
  events.clear();

#if LVQ_CLS_DEBUG
  std::cout << "\n<-I-> DEBUG INFO\n";
  for(size_t ot = 0; ot < 5; ++ot)
  {
    std::cout << "\nReal lable is "  << classifiedEvents[ot].realLabel
	      << "  given lable is " << classifiedEvents[ot].givenLabel;
    printResult( classifiedEvents[ot].clsOuts );
  }
#endif
  
  // Open file to write the results to  
  std::ofstream OutPut;
  
  OutPut.open (outF.c_str());
  OutPut << "# Classification results for the events from\n# "
	 << evtF
	 << "\n# Total number of events was " << classifiedEvents.size()
	 << "\n\n";

  // For each label we need to find out the number of missclassified
  // events.
  size_t correctCls;
  size_t wrongCls;

  // Class loop
  for(size_t l = 0; l < labels.size(); ++l)
  {
    std::string curLabel = labels[l];
    // Reset for each label
    correctCls = 0;
    wrongCls   = 0;
    
    // Events loop
    for(size_t ev = 0; ev < classifiedEvents.size(); ++ev)
    {
      // Element of the Current labels
      if( classifiedEvents[ev].realLabel == curLabel )
      {
	if (classifiedEvents[ev].realLabel == classifiedEvents[ev].givenLabel )
	{//Correct classified
	  correctCls++;
	}
	else// Wrong labels
	{
	  wrongCls++;
	}
      }// If current label
    }//Events loop
    
    // Write the results to the output file.    
    OutPut << "++++++++++++++ Classification Results ++++++++++++\n"
	   << "\tCurrent class Name " << curLabel
	   << "\n\tWe have seen "     << (*counts)[curLabel]
	   << " Events in this class\n"
	   << "\tNumber of Correct classified events = " << correctCls
	   << "\n\tNumber of mis-classified events = "   <<  wrongCls
	   << "\n\tErro = "
	   << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>( (*counts)[curLabel] ))
	   << " %.\n"
	   << std::flush;
  }// Labels loop
  // Close Open file
  OutPut.close();
  
  // Create ROC points.
  std::cout << "<-I-> Creating ROC.\n";
  std::vector< ROCPoints > Roc;
  ProduceROC( classifiedEvents, "electron", "pion", (*counts)["electron"], (*counts)["pion"], Roc);
  
  WriteRocToFile("ROC" + outF, Roc); 

  // TestPart
  TH1F myHist ("myHist","myHistDesc", 100, 0.0, 1.0);
  for(size_t k = 0; k < classifiedEvents.size(); ++k)
  {
    ClassifierOutPuts& a = classifiedEvents[k];
    float val = a.clsOuts["electron"]/ a.clsOuts["pion"];
    
    if(val < 1.0)
    {
      myHist.Fill(val);
    }
  }
  myHist.SaveAs("testHistDists.root");
  
  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;
  
  return 0;
}
