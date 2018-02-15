/* ***************************************
 * MultiClass BDT Classify example       *
 * Author: M.Babai@rug.nl                *
 * Version:                              *
 * LICENSE:                              *
 * ***************************************
 */
/*
 * Note: This is just an interface to the original TMVA
 * implementation. To find out the available options, please read TMVA
 * manuals. In case of errors or wrong outputs produced by TMVA
 * classifiers, try to read their mailing list and send your questions
 * to the same list.
 ******* VERY IMORTANT ****
 * You NEED TMVA version > 4.1.X before this works.
 */

#include "PndMultiClassBdtClassify.h"
#include "PndMvaTools.h"

#define DEBUG_PRINT 0
#define DEBUG_NUM_EVENTS 20

int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cerr << "<ERROR>\n\t<USAGE>: " << argv[0]
	      << " <Weight File> <Events File>\n";
    exit(10);
  }

  // The weightFile.  
  std::string WeightFile = argv[1];
  
  std::string EvtFile    = argv[2];

  // Labels.
  std::vector<std::string> labels;

  // Variables.
  std::vector<std::string> variables;

  // Signal and background labels
  std::string sgName = "electron";
  std::string bgName = "pion";
  
  // Class names
  labels.push_back("electron");
  labels.push_back("pion");
  
  // Variable names 
  // variables.push_back("p");
  
  variables.push_back("emc");
  variables.push_back("lat");
  variables.push_back("z20");
  variables.push_back("z53");
  variables.push_back("E9E25");
  
  // variables.push_back("E1");
  // variables.push_back("E9");
  // variables.push_back("E25");
  // variables.push_back("E1E9");

  
  // Create and init the classifier object
  PndMultiClassBdtClassify cls(WeightFile, labels, variables);
  cls.Initialize();

  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Read events to be classified.
  std::map<std::string, size_t>* counts = readEvents(EvtFile.c_str(), variables,
						     labels, events);
  
  std::cout << "The file contains " << events.size()
	    << " events.\n";
  
  // Map to store results.
  std::map<std::string, float> res;
  
  // Store classifier outputs per event.
  std::vector< ClassifierOutPuts > classifiedEvents;
  
  size_t numberOfEvt;
  
#if (DEBUG_PRINT == 0)
  numberOfEvt = events.size();
#else
  numberOfEvt = DEBUG_NUM_EVENTS;
#endif
  
  // Events loop
  for(size_t k = 0; k < numberOfEvt; k++)
  {
    std::vector<float>* evt = (events[k]).second;
    
    // Get Mva Value
    cls.GetMvaValues( (*evt), res);
    
    // Do classification
    std::string* givenLabel = cls.Classify( (*evt) );
    
    classifiedEvents.push_back(ClassifierOutPuts((events[k]).first, *givenLabel,
						 res[sgName], res[bgName], 0.00));
    delete givenLabel;
  }// Events Loop

  /*
   * Events vector is not needed anymore.
   * Cleaning.
   */
  std::cout << "Clean up Events.\n";
  for(size_t i = 0; i < events.size(); ++i)
  {
    delete (events[i]).second;
  }
  events.clear();
  
#if (DEBUG_PRINT != 0)
  print(classifiedEvents);
#endif
  
  // We have seen all the events.
  size_t misCl = 0;
  for(size_t k = 0; k < classifiedEvents.size(); ++k)
  {
    if( classifiedEvents[k].realLabel != classifiedEvents[k].givenLabel)
    {
      misCl++;
    }
  }
  // Classifier evaluation info.
  std::cout << "+++++++++++++++++++++++++++++++++++++++\n" 
	    << " Total number of classified events: "
	    << numberOfEvt << '\n'
	    << " Number of missclassified: " << misCl << " = "
	    << ( static_cast<float>(misCl) * 100.00)/ static_cast<float>(numberOfEvt)
	    <<" %\n"
	    << " Correct cassified = " << (numberOfEvt - misCl)
	    << "\n+++++++++++++++++++++++++++++++++++++++\n";
  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;
  
  return 0;
}
