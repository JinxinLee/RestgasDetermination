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

// Local headers
#include "PndKnnClassify.h"
#include "PndMvaTools.h"

// Root and PandaRoot.
#include "TStopwatch.h"

//______________________________________
#define DEBUG_PRINT 0

// If produce ROC
#define PRODUCE_ROC 0
//________________________

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */
int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "\t<ERROR>" 
	      << argv[0] << " <inputWeightFile> <InputEventsFile>"
	      << " <numOfneigh>"
	      << std::endl;
    return 1;
  }
  
  // Init input variables.
  std::string InPutFileName = argv[1];
  std::string InputEvents   = argv[2];
  std::string NumNeistr     = argv[3];
  
  // Convert to int.
  unsigned int NumNei = str2Uint(NumNeistr);
  
  // Containers to hold labels and variable names.
  std::string sgName = "electron";
  std::string bgName = "pion";
  
  std::vector<std::string> labels;
  std::vector<std::string> vars;
  
  // Classes (container to hold the class names)
  labels.push_back("electron");
  labels.push_back("pion");
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");
  
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
  PndKnnClassify cls (InPutFileName, labels, vars);

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
  
  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Read events to be classified for a given label.
  std::map<std::string, size_t>* counts = readEvents(InputEvents.c_str(),
						     vars, labels,
						     events);
  // Map to store the results
  std::map<std::string, float> res;
  
  // Store classifier outputs per event.
  std::vector< ClassifierOutPuts > classifiedEvents;
  
  // Reste and start the timer.
  timer.Reset();
  timer.Start();
  
  // Perform classification of the available events.
  size_t numberOfEvt = events.size();
  
#if DEBUG_PRINT
  numberOfEvt = 20;
#endif
  std::cout << "Total number of events to be classified = "
	    << numberOfEvt
	    << '\n';

  // Events are ready Start to classify.
  // Examples loop
  for(size_t ev = 0; ev < numberOfEvt; ++ev)
  {
    // Fetch the current event
    std::vector<float>* curEvt = (events[ev]).second;
    
    // Get Mva Value
    cls.GetMvaValues((*curEvt), res);
  
    // Do classification
    std::string* givenLabel = cls.Classify( (*curEvt));

    // Store results.
    classifiedEvents.push_back(ClassifierOutPuts((events[ev]).first, (*givenLabel),
						 res[sgName], res[bgName], 0.00));

#if DEBUG_PRINT
    std::cout << " Given label is "
	      << (*givenLabel)
	      << std::endl;
    print(res, ev);
#endif
    
    delete givenLabel;
  }// End Examples loop
  
  timer.Stop();
  rtime = timer.RealTime();
  ctime = timer.CpuTime();
  
  std::cout << "<INFO> Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = "
	    << ctime <<" Seconds.\n\n";
  
  //__________________ Clean up _____________//
  // We are done with events vector. Cleaning
  std::cout << "Clean up Events.\n";
  for(size_t i = 0; i < events.size(); ++i)
  {
    delete (events[i]).second;
  }
  events.clear();
  
  //_______ Classifier evaluation info.
  size_t misCl = 0;
  
  // Events loop
  for(size_t ev = 0; ev < classifiedEvents.size(); ++ev)
  {
    if( classifiedEvents[ev].realLabel != classifiedEvents[ev].givenLabel )
    {
      misCl++;
    }
  }
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

#if PRODUCE_ROC
  // Create ROC points.
  std::cout << "<-I-> Creating ROC.\n";
  std::vector< ROCPoints > Roc;
  Produce_ROC( classifiedEvents, sgName, bgName,
	       (*counts)[sgName], (*counts)[bgName], Roc);

  WriteRocToFile("ROCKNN.root", Roc); 
#endif

  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;
  
  return 0;
}
