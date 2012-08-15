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
// C++
#include <fstream>

// Local includes
#include "PndLVQClassify.h"
#include "PndMvaTools.h"

// ROOT
#include "TStopwatch.h"
#include "TH1.h"
//________________________________________________________________

// Print DEBUG info
#define LVQ_CLS_DEBUG 0

// Number of elements to print
#define NUM_DEBUG_PRINT 10

// If produce ROC
#define PRODUCE_ROC 0

// Use VQ ROC procedure
#define USE_PRODUCE_VQ_ROC 1

// Create the distance histograms
#define CREATE_DIST_HISTS 1

// Determine recognition error as function of momentum.
#define PER_MOMENTUM_INTERVAL 0

//________________________________________________________________

#if PRODUCE_ROC
#if USE_PRODUCE_VQ_ROC
// Produce a set of points to draw the ROC.
void Produce_VQ_ROC( std::vector< ClassifierOutPuts >& input,//Alg. input
		     std::string const& SigName,// Signal name
		     std::string const& BgName,// Background name
		     size_t sigCnt, size_t bgCnt,// number of sg and bg
		     std::vector< ROCPoints >& Roc,// Produced set of ROC points
		     size_t numSteps = 20)// Number of steps (ROC points)
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
    if( input[i].sgValue < MinVal )
    {
      MinVal = input[i].sgValue;
    }
    
    // MaxVal
    if( input[i].sgValue > MaxVal )
    {
      MaxVal = input[i].sgValue;
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
  fpCnt  = tpCnt = fn = tn = 0;
  
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
      if( a.sgValue <= trhold )
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
#endif// VQ ROC
#endif// IF ROC

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

  // Input file containing prototypes
  std::string inF  = argv[1];
  
  // Events to classify
  std::string evtF = argv[2];
  
  // Output results file
  std::string outF = argv[3];
  
  std::cout << "\tClassifying events from " << evtF << '\n'
	    << "\tUsing prototypes from " << inF << '\n'
	    << "\tThe outoput will be stored in txt format in "<< outF 
	    << '\n';

  // Signal and background labels
  std::string sgName = "electron";
  std::string bgName = "pion";

  // Labels.
  std::vector<std::string> labels;

  // Variables.
  std::vector<std::string> varNames;

  std::vector<std::string> TsEvtVarNames;

  // Add labels
  labels.push_back("electron");
  labels.push_back("pion");
  
  //labels.push_back("kaon");
  //labels.push_back("muon");
  //labels.push_back("proton");
  
  // Add Variables
  //varNames.push_back("p");
  varNames.push_back("emc");
  varNames.push_back("lat");
  varNames.push_back("z20");
  varNames.push_back("z53");
  varNames.push_back("E9E25");

  // varNames.push_back("E1");
  //varNames.push_back("E9");
  // varNames.push_back("E25");
  // varNames.push_back("E1E9");
  
  // Create classifier.
  PndLVQClassify cls (inF, labels, varNames);

  // Init classifier
  cls.Initialize();

  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;

  // Add variable "p" to the head of list.
#if (PER_MOMENTUM_INTERVAL == 1)
  varNames.insert(varNames.begin(), "p");
#endif

  // Read events to be classified.
  std::map<std::string, size_t>* counts = readEvents(evtF.c_str(), varNames,
						     labels, events);

  std::cout << "Total number of events to be classified = "
	    << events.size()
	    << '\n';

  // Map to store results.
  std::map<std::string, float> res;

  // Start the timer.
  TStopwatch timer;
  timer.Start();
  
  // Store classifier outputs per event.
  std::vector< ClassifierOutPuts > classifiedEvents;

  // Temporary to store events momentum.  
  float tmp_mom = 0.0;

  // Events loop
  for(size_t k = 0; k < events.size(); k++)
  {
    std::vector<float>* evt = (events[k]).second;
    
#if (PER_MOMENTUM_INTERVAL == 1)
    // Remember p, first element.
    tmp_mom = evt->at(0);

    // Delete first element (p)
    evt->erase(evt->begin());
#endif
  
    // Get Mva Value
    cls.GetMvaValues( (*evt), res);
    
    // Do classification
    std::string* givenLabel = cls.Classify( (*evt) );
    
    // Store results.
#if USE_PRODUCE_VQ_ROC
    // The smaller (the output) the better
    classifiedEvents.push_back(ClassifierOutPuts((events[k]).first, *givenLabel,
						 res[sgName], res[bgName], tmp_mom));
#else
    // If using the general ROC function. The larger the better
    classifiedEvents.push_back(ClassifierOutPuts((events[k]).first, *givenLabel,
						 (1.0 - res[sgName]),
						 (1.0 - res[bgName]), tmp_mom) );
#endif
    
    delete givenLabel;
  }// Events Loop
  
  // Print some timing information
  timer.Stop();

  double rtime = timer.RealTime();
  double ctime = timer.CpuTime();
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

#if LVQ_CLS_DEBUG
  std::cout << "\n<-I-> DEBUG INFO\n";
  for(size_t ot = 0; ot < NUM_DEBUG_PRINT; ++ot)
  {
    std::cout << "Real lable is "  << classifiedEvents[ot].realLabel
	      << "  given lable is " << classifiedEvents[ot].givenLabel
	      << " clsOut[signal]  " << classifiedEvents[ot].sgValue
	      << " clsOut[bground] " << classifiedEvents[ot].bgValue
	      << " Mom = "           << classifiedEvents[ot].mom
	      <<'\n';
  }
  std::cout <<'\n';
#endif

  // Open file to write the results to  
  std::ofstream OutPut;

  OutPut.open (outF.c_str());
  OutPut << "# The weight file name is:\n# "
         << inF
         << "\n# Classification results for the events from:\n# "
	 << evtF
	 << "\n# Total number of events was " << classifiedEvents.size()
	 << "\n\n";
  
#if (PER_MOMENTUM_INTERVAL == 0)

  // For each label we need to find out the number of missclassified
  // events.
  size_t correctCls;
  size_t wrongCls;
  size_t totMissCls = 0;

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
	  // Overal missclassified.
	  totMissCls++;
	}
      }// If current label
    }//Events loop
    
    // Write classification results to the output file.    
    OutPut << "++++++++++++++ Classification Results ++++++++++++\n"
	   << "\tCurrent class Name " << curLabel
	   << "\n\tWe have seen "     << (*counts)[curLabel]
	   << " Events in this class\n"
	   << "\tNumber of Correct classified events = " << correctCls
	   << "\n\tNumber of mis-classified events = "   <<  wrongCls
	   << "\n\tErro = "
	   << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>( (*counts)[curLabel] ))
	   << " %."
	   << std::endl;
  }// Labels loop
  OutPut << "_______________ Overal error _______________\n"
	 << "Total number of missclassified events = " << totMissCls
	 << " ("
	 << ((static_cast<float>(totMissCls) * 100.00)/static_cast<float>(classifiedEvents.size()))
	 << " %).\n\n";

#else // Do per momentum region. (PER_MOMENTUM_INTERVAL != 0)
  // 0.00 <= p <= 1.0 low
  // 1.0  <  p <= 4.0 mid
  // 4.0  <  p high
  std::map <std::string, size_t> pionErr;
  std::map <std::string, size_t> electErr;
  pionErr["low"] = pionErr["mid"] = pionErr["high"]  = 0;
  electErr["low"]= electErr["mid"]= electErr["high"] = 0; 
  
  float plow, pmid, phigh, elow, emid, ehigh;
  plow = pmid = phigh = elow = emid = ehigh = 0.00;

  //___________________________ FIXME
  // Find the total number of events for each class.
  for(size_t ev = 0; ev < classifiedEvents.size(); ++ev)
  {
    if( classifiedEvents[ev].realLabel == "pion")
    {
      if(classifiedEvents[ev].mom <= 1.0)
      {
	plow++;
      }
      else// 1.0 < mom
      {
	if(classifiedEvents[ev].mom <= 4.0)
	{
	  pmid++;
	}
	else// 4.0 < mom
	{
	  phigh++;
	}
      }
    }
    else//Electrons
    {
      if(classifiedEvents[ev].mom <= 1.0)
      {
	elow++;
      }
      else// 1.0 < mom
      {
	if(classifiedEvents[ev].mom <= 4.0)
	{
	  emid++;
	}
	else// 4.0 < mom
	{
	  ehigh++;
	}
      }
    }
  }
  // Events loop
  for(size_t ev = 0; ev < classifiedEvents.size(); ++ev)
  {
    if (classifiedEvents[ev].realLabel != classifiedEvents[ev].givenLabel )
    {// Wrong labels
      if( classifiedEvents[ev].realLabel == "pion")
      {
	if(classifiedEvents[ev].mom <= 1.0)
	{
	  pionErr["low"] += 1;
	}
	else// 1.0 < mom
	{
	  if(classifiedEvents[ev].mom <= 4.0)
	  {
	    pionErr["mid"] += 1;
	  }
	  else// 4.0 < mom
	  {
	    pionErr["high"] += 1;
	  }
	}
      }
      else//Electrons
      {
	if(classifiedEvents[ev].mom <= 1.0)
	{
	  electErr["low"] += 1;
	}
	else// 1.0 < mom
	{
	  if(classifiedEvents[ev].mom <= 4.0)
	  {
	    electErr["mid"] += 1;
	  }
	  else// 4.0 < mom
	  {
	    electErr["high"] += 1;
	  }
	}
      }
    }
  }//Events loop
  OutPut << "0.00 <= p <= 1.0 (low)\n1.0  <  p <= 4.0 (mid)\n4.0  <  p (high)"
	 << "\n\n"
	 << "electErr[low] = " << electErr["low"]
	 << " total = "<< elow<< " ("
	 << (static_cast<float>(electErr["low"]) * 100.0)/ elow << " %)\n"
	 << "electErr[mid] = " << electErr["mid"]
	 << " total = " << emid << " ("
	 << (static_cast<float>(electErr["mid"]) * 100.0)/ emid << " %)\n"
	 << "electErr[high] = " << electErr["high"]
	 << " total = "<< ehigh << " ("
	 << (static_cast<float>(electErr["high"]) * 100.0)/ ehigh << " %)\n\n"
	 << "pion[low] = " << pionErr["low"]
	 << " total = " << plow << " ("
	 << (static_cast<float>(pionErr["low"]) * 100.0)/ plow << " %)\n"
	 << "pion[mid] = " << pionErr["mid"]
	 << " total = " << pmid << " ("
	 << (static_cast<float>(pionErr["mid"]) * 100.0)/ pmid << " %)\n"
	 << "pion[high] = " << pionErr["high"]
	 << " Total = " << phigh << " ("
	 << (static_cast<float>(pionErr["high"]) * 100.0)/ phigh << " %)\n\n";
#endif
  // Close Open file
  OutPut.close();
  
#if PRODUCE_ROC
  // Create ROC points.
  std::cout << "<-I-> Creating ROC.\n";
  std::vector< ROCPoints > Roc;
  
#if USE_PRODUCE_VQ_ROC  
  Produce_VQ_ROC( classifiedEvents, sgName, bgName,
		  (*counts)[sgName], (*counts)[bgName],
		  Roc, 100);
#else
  Produce_ROC( classifiedEvents, sgName, bgName,
	       (*counts)[sgName], (*counts)[bgName], Roc);
#endif

  // Write the ROC points to a file.
  WriteRocToFile( ("ROC" + outF), Roc); 
#endif

#if ( LVQ_CLS_DEBUG && PRODUCE_ROC)
  printRoc(Roc);
#endif

#if CREATE_DIST_HISTS
  // Create dist histograms.
  TH1F corClsHist ("corClsHist","Correct classification distancess", 100, 0.0, 1.0);
  TH1F misClsHist ("misClsHist","Mis classification distancess", 100, 0.0, 1.0);

  for(size_t k = 0; k < classifiedEvents.size(); ++k)
  {
    ClassifierOutPuts& a = classifiedEvents[k];
    if(a.realLabel == a.givenLabel)
    {
      corClsHist.Fill(a.sgValue);
    }
    else
    {
      misClsHist.Fill(a.bgValue);
    }
  }

  outF = "Hists" + outF;
  TFile histsfile(outF.c_str(),"RECREATE");
  corClsHist.Write();
  misClsHist.Write();
  histsfile.Close();
#endif

  //__________________ Clean up _____________//
  // Delete per label example counts
  counts->clear();
  delete counts;
  
  return 0;
}
