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
//#include <fstream>
#include <iomanip>

// Local
#include "PndPrzWindowClassify.h"
#include "PndMvaTools.h"

// ROOT & PandaRoot
#include "TFile.h"
#include "TNtuple.h"
#include "TStopwatch.h"

#define DEBUG_PRITN 0

/*
 * *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr << "\t<ERROR>" 
	      << argv[0] <<" <inputWeightFile> <InputEventsFile>"
	      << " <OutPutLogFile>"
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

  // Signal and background labels
  std::string sgName = "electron";
  std::string bgName = "pion";
  
  // Containers to hold labels and variable names.
  std::vector<std::string> labels;
  std::vector<std::string> vars;
  std::map<std::string, float> wsize;
  
  // Labels (container to hold the class names)
  labels.push_back("electron");
  labels.push_back("pion");
  
  // Variables (names)
  vars.push_back("emc");
  wsize["emc"] = 2.4;
  
  vars.push_back("lat");
  wsize["lat"] = 5.0;
  
  vars.push_back("z20");
  wsize["z20"] = 5.0;
  
  vars.push_back("z53");
  wsize["z53"] = 5.0;
  
  TStopwatch timer;
  timer.Start();
  
  //Create the classifier object and specify the weight file
  PndPrzWindowClassify cls (InPutFile, labels, vars);
  cls.Initialize();

  // Set classifier parameters and init.
  cls.setWindowSize(wsize);
  
  timer.Stop();
  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
  std::cout << "<INFO> Initialization time:\n"
	    << "RealTime = " << rtime
	    << " seconds, CpuTime = " << ctime
	    <<" Seconds.\n";
  
  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Read events to be classified.
  std::map<std::string, size_t>* counts = readEvents(InputEvents.c_str(), vars,
						     labels, events);
  size_t totNumEvt;
#if DEBUG_PRITN
  totNumEvt = 10;
#else
  totNumEvt = events.size();
#endif
  
  std::cout << "Total number of events to be classified = "
	    << totNumEvt
	    << '\n';
  
  // Map to store the results
  std::map<std::string, float> res;
  
  // Store classifier outputs per event.
  std::vector< ClassifierOutPuts > classifiedEvents;  
  
  // Reste and start the timer.
  timer.Reset();
  timer.Start();
  
  // ___________ Classification ________//
  std::cout << "<INFO> Classification.\n";
  // Events loop
  for(size_t k = 0; k < totNumEvt; ++k)
  {
    std::vector<float> const* evt = (events[k]).second;
    
    // Get Mva Value
    cls.GetMvaValues( (*evt), res);

#if DEBUG_PRITN
    printResultMap(res);
#endif

    // Do classification
    std::string* givenLabel = cls.Classify( (*evt) );
    
    // Prob. Estimation the better
    classifiedEvents.push_back(ClassifierOutPuts((events[k]).first, *givenLabel,
						 res[sgName], res[bgName], 0.00));
    delete givenLabel;
  }// Events Loop
  
  // Print some timing information
  timer.Stop();
  rtime = timer.RealTime();
  ctime = timer.CpuTime();
  std::cout << "Classifier timing results:\n"
	    << "RealTime = " << rtime
	    << " seconds, CpuTime = " << ctime <<" Seconds\n"
	    << "It took " << rtime/static_cast<double>(totNumEvt)
	    << " Per event.\n";
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
  size_t correctCls, wrongCls;
  size_t totMisCls = 0;

  // Class loop
  for(size_t l = 0; l < labels.size(); ++l)
  {
    std::string curLabel = labels[l];
    // Reset counters for each label
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
	  totMisCls++;
	}
      }// If current label
    }//Events loop
    
    // Write classification results to the output file.    
    Outfile << "++++++++++++++ Classification Results ++++++++++++\n"
	    << "\tCurrent class Name " << curLabel
	    << "\n\tWe have seen "     << (*counts)[curLabel]
	    << " Events in this class\n"
	    << "\tNumber of Correct classified events = " << correctCls
	    << "\n\tNumber of mis-classified events = "   <<  wrongCls
	    << "\n\tErro = "
	    << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>( (*counts)[curLabel] ))
	    << " %.\n";
  }// Labels loop
  Outfile << std::setprecision(5) << "Total Number of Missclassified events = " << totMisCls
	  << " "<< ( static_cast<float>(totMisCls * 100)/static_cast<float>(totNumEvt) )
	  << '%' << std::endl;
  
  // Close open file
  Outfile.close();
  
  timer.Stop();
  rtime = timer.RealTime();
  ctime = timer.CpuTime();
  std::cout << "==============================================\n"
	    << "<INFO> Classifier timing results:\n"
	    << "RealTime = " << rtime << " seconds, CpuTime = " 
	    << ctime <<" Seconds.\n\n";

  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;

  return 0;
}
