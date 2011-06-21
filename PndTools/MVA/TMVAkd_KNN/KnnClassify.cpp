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

#define DEBUG_PRINT 0

// C++ headers
#include <sstream>

// Local headers
#include "PndKnnClassify.h"
#include "PndMvaTools.h"

// Root and PandaRoot.
#include "TStopwatch.h"

//______________ Helper functions and variables _______________________
void printResult( std::map<std::string,float>& res, unsigned int evtId)
{
  std::cout << "\t==================================\n"
	    << " Evt Num = " << evtId
	    << '\n';
  
  for( std::map<std::string,float>::iterator ii=res.begin(); 
       ii != res.end(); ++ii)
  {
    std::cout <<"\t" << (*ii).first 
	      << "\t=> " << (*ii).second
	      << '\n';
  }
  
  std::cout << "\t==================================\n";
}

// Produce a set of points to draw the ROC.
void Produce_KNN_ROC( std::vector< ClassifierOutPuts >& input,//Alg. input
		      std::string const& SigName,// Signal name
		      std::string const& BgName,// Background name
		      size_t sigCnt, size_t bgCnt,// number of sg and bg
		      std::vector< ROCPoints >& Roc,// Produced set of ROC points
		      size_t numSteps = 10)// Number of steps (ROC points)
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

  // Determine the value for increment.
  float inc;
  if( numSteps == 0 )
  {
    numSteps = 10;
  }
  inc = (MaxVal - MinVal )/static_cast<float>(numSteps);

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

      // LVQ (smaller is better)
      if( a.clsOuts[SigName] >= trhold )
      {// In Signal region (Assume signal)
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
      {//Assume background
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
}
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
  std::istringstream buff(NumNeistr);
  unsigned int NumNei = 0;
  buff >> NumNei;
  
  // Containers to hold labels and variable names.
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
	    << numberOfEvt //events.size()
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
    classifiedEvents.push_back(ClassifierOutPuts((events[ev]).first, (*givenLabel), res));

#if DEBUG_PRINT
    std::cout << " Given label is "
	      << (*givenLabel)
	      << std::endl;
    printResult(res, ev);
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

  // Create ROC points.
  std::cout << "<-I-> Creating ROC.\n";
  std::vector< ROCPoints > Roc;
  Produce_KNN_ROC( classifiedEvents, "electron", "pion",
		  (*counts)["electron"], (*counts)["pion"],
		  Roc);
  
  WriteRocToFile("ROCKNN.root", Roc); 

  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;
  
  return 0;
}
