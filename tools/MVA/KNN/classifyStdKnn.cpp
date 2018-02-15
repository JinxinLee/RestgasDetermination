/********************
 * Author: M. Babai *
 * M.Babai@rug.nl   *
 ********************
 */

/*
 * Example program. This code shows how to use the classify
 * procedure. This classifier is implemented based on the standard KNN
 * algorithm.
 */

#include <iomanip>

#include "PndStdKnnClassify.h"
#include "PndMvaTools.h"

#include "TStopwatch.h"

/* *********************************************
 * Testing routine, can be deleted afterwards. *
 * *********************************************
 */

int main(int argc, char** argv)
{
  if(argc < 4)
  {
    std::cerr <<"\t<ERROR>" 
	      << argv[0] <<" <input Weights> <Events FIlename> <numOfneigh>"
	      << std::endl;
    return 1;
  }
  
  std::string InWeights = argv[1];
  std::string InEvents  = argv[2];
  std::string NumNeistr = argv[3];
  int NumNei = str2int(NumNeistr);

  // Signal and background labels
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
  vars.push_back("emc");
  vars.push_back("lat");
  vars.push_back("z20");
  vars.push_back("z53");

  //vars.push_back("mvd");  vars.push_back("stt");
  //vars.push_back("tof"); vars.push_back("tpc");
  
  //Create the classifier object and specify the weight file
  PndStdKnnClassify cls (InWeights, labels, vars);
  cls.SetKNN(NumNei);
  cls.Initialize();

  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;
  
  // Read events to be classified.
  std::map<std::string, size_t>* counts = readEvents(InEvents.c_str(), vars,
						     labels, events);

  // Map to store the results
  std::map<std::string,float> res;
  
  // Store classifier outputs per event.
  std::vector< ClassifierOutPuts > classifiedEvents;
  size_t totNumEvt;

#if DEBUG_PRINT
  totNumEvt = 10;
#else
  totNumEvt = events.size();
#endif

  TStopwatch ti;
  ti.Start();
  
  // Events loop
  for(size_t k = 0; k < totNumEvt; k++)
  {
    std::vector<float> const* evt = (events[k]).second;
    
    // Get Mva Value
    cls.GetMvaValues( (*evt), res);

#if DEBUG_PRINT
    print(res);    
#endif

    // Do classification
    std::string* givenLabel = cls.Classify( (*evt) );
    
    // Store results.
    classifiedEvents.push_back(ClassifierOutPuts((events[k]).first, *givenLabel,
						 res[sgName], res[bgName], 0.00));
    delete givenLabel;
  }
  

  ti.Stop();
  // Print some timing information
  ti.Stop();
  double rtime = ti.RealTime();
  double ctime = ti.CpuTime();
  std::cout << "Classifier timing results:\n"
	    << "RealTime = " << rtime
	    << " seconds, CpuTime = " << ctime <<" Seconds\n"
	    << "It took " << (rtime/static_cast<double>(events.size()))
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
  
  // ___________ Classify input events ________
  size_t correctCls, wrongCls;
  size_t totMisCls = 0;
  // Class loop
  for(size_t l = 0; l < labels.size(); ++l)
  {
    std::string const& curLabel = labels[l];
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
    std::cout << "++++++++++++++ Classification Results ++++++++++++\n"
	      << "\tCurrent class Name " << curLabel
	      << "\n\tWe have seen "     << (*counts)[curLabel]
	      << " Events in this class\n"
	      << "\tNumber of Correct classified events = " << correctCls
	      << "\n\tNumber of mis-classified events = "   <<  wrongCls
	      << "\n\tErro = "
	      << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>( (*counts)[curLabel] ))
	      << " %.\n";
  }
  std::cout << std::setprecision(5) << "Total Number of Missclassified events = " << totMisCls
	    << " "<< ( static_cast<float>(totMisCls * 100)/static_cast<float>(totNumEvt) )
	    << '%' << std::endl;
 
  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;

 return 0;
}
