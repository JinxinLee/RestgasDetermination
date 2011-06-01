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
// Local includes
#include "PndLVQClassify.h"

// C++
#include <fstream>

// ROOT
#include "TFile.h"
#include "TH1.h"
#include "TStopwatch.h"

/////////_______ Inline header ______///////////////////////////
void printResult(std::map<std::string, float> const& res);

std::map<std::string, size_t>* readEvents(const char* infile, std::vector<std::string> const& varNames,
					  std::vector<std::string> const& classNames,
					  std::vector<std::pair<std::string, std::vector<float>*> >& coNt);

struct ROCPoints
{
  //public:
  // Constructors
  ROCPoints()
    : FP_rate(0.0), TP_rate(0.0)
  {};
  ROCPoints(float fpr, float tpr)
    : FP_rate(fpr), TP_rate(tpr)
  {};
  // Destructor
  ~ROCPoints(){};

  // Copy Const
  ROCPoints(ROCPoints const& ot)
    : FP_rate(ot.FP_rate), TP_rate(ot.TP_rate)
  {};
  // Operators.  
  ROCPoints& operator=(ROCPoints const& ot)
  {
    this->FP_rate = ot.FP_rate;
    this->TP_rate = ot.TP_rate;
    return (*this);
  };

  // Variables
  float FP_rate;
  float TP_rate;

  //protected:
  private:
  bool operator==(ROCPoints const& ot) const;
  bool operator>( ROCPoints const& ot) const;
  bool operator<( ROCPoints const& ot) const;
};

struct ProbLabel
{
  //public:
  // Constructors
  ProbLabel()
    : clsOut(0.0), label("UNKNOWN"), Orig_label("DONTKNOW")
  {};
  ProbLabel(float x, std::string lb, std::string Orlb)
    : clsOut(x), label(lb), Orig_label(Orlb)
  {};
  // Destructor
  ~ProbLabel(){};

  // Copy Const
  ProbLabel(ProbLabel const& ot)
    : clsOut(ot.clsOut), label(ot.label), Orig_label(ot.Orig_label)
  {};
  // Operators.
  inline bool operator> (ProbLabel  const& ot) const
  {
    return (this->clsOut > ot.clsOut);
  };
  
  inline bool operator< (ProbLabel  const& ot) const
  {
    return (this->clsOut < ot.clsOut);
  };
  
  ProbLabel& operator=(ProbLabel const& ot)
  {
    this->clsOut = ot.clsOut;
    this->label  = ot.label;
    this->Orig_label = ot.Orig_label;
    return (*this);
  };

  // Variables
  float clsOut; // Classifier output
  std::string label; // Given label (by the classifier)
  std::string Orig_label;// True label
  //protected:
  private:
  //==
  inline bool operator==(ProbLabel const& ot) const;
};
/*
 * creates pairs of [fpRate, tpRate]
 * List of (LVQ output, label) decreasing.
 * Roc: List of (FP/N, TP/P)
 */
void ProduceROC( std::vector< ProbLabel >& input, std::string SigName,
		 size_t sigCnt, size_t bgCnt, std::vector< ROCPoints >& Roc);
//////////////////////////////////////////////////////////////////

void ProduceROC( std::vector< ProbLabel >& input, std::string SigName,
		 size_t sigCnt, size_t bgCnt, std::vector< ROCPoints >& Roc)
{
  // Clean the output
  Roc.clear();

  // First we need to sort the list (decreasing) (input)
  std::sort( input.begin(), input.end());
  std::reverse( input.begin(), input.end() );
  
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
    return;
  }

  float fp, tp, prevF;
  fp = tp = 0.0;
  prevF = std::numeric_limits<float>::min();

  for(size_t i = 0; i < input.size(); ++i)
  {
    //if(prevF != input[i].clsOut)
    if( (prevF < input[i].clsOut) || (prevF > input[i].clsOut))
    {
      Roc.push_back(ROCPoints(fp/bg, tp/sg));
      prevF = input[i].clsOut;
    }
    // If input[i] == signal
    //if(input[i].label == SigName)
    //SigName.size();
    if( (input[i].label == SigName) && (input[i].Orig_label == SigName) )
    {
      tp++;
    }
    else if( (input[i].label == SigName) && (input[i].Orig_label != SigName) )
    {
      fp++;
    }
  }
  //(1,1)
  Roc.push_back(ROCPoints(fp/bg, tp/sg));
  std::cerr << "Added ( " << fp/bg <<", " << tp/sg << ")\n";
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
					  std::vector<std::string> const& classNames, 
					  std::vector<std::pair<std::string, std::vector<float>*> >& coNt)
{
  // Clear event container
  coNt.clear();

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
      exit(10);
    }

    // Get the counts for the current label
    size_t NumEvtCurLabel = static_cast<size_t>(t->GetEntriesFast());
    counts->insert( std::make_pair (classNames[cls], NumEvtCurLabel) );

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
  // Proto types
  std::string inF  = argv[1];

  // Events to classify
  std::string evtF = argv[2];

  // Output results file
  std::string outF = argv[3];

  // Output hists-File  
  std::string outHistFile = "Hists_" + outF;
  
  std::cout << "\tClassifying events from " << evtF << '\n'
	    << "\tUsing prototypes from " << inF << '\n'
	    << "\tThe outoput will be stored in txt format in "<< outF 
	    << '\n';
  
  // Labels.
  std::vector<std::string> labels;

  // Sig label name
  std::vector<std::string> sigName;
  
  // Variables.
  std::vector<std::string> varNames;

  // Map to store results.
  std::map<std::string, float> res;

  // To be classified events.
  std::vector<std::pair<std::string, std::vector<float>* > > events;

  // Store distances from the winning codebooks.
  std::map<std::string, TH1F*> histograms;
  
  // Add Sig. label
  sigName.push_back("electron");

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

  //nam.push_back("thetaC");
  //nam.push_back("tof"); 
  //nam.push_back("stt");
  //nam.push_back("mvd"); 
  
  // Init histograms.
  for(size_t i = 0; i < labels.size(); ++i)
  {
    std::string des = "DescriptionOf" + labels[i];
    TH1F* h1 = new TH1F(labels[i].c_str(), des.c_str(), 100, 0.0, 1.0);
    histograms.insert(std::make_pair(labels[i], h1));
  }
  
  // Create classifier.
  PndLVQClassify cls (inF, labels, varNames);

  // Init
  cls.Initialize();  
  
  // Read events to be classified.
  std::map<std::string, size_t>* counts = readEvents(evtF.c_str(), varNames, labels, events);
  
  std::cout << "Total number of events to be classified = "
	    << events.size()
	    << '\n';
  
  std::ofstream OutPut;
  OutPut.open (outF.c_str());
  OutPut << "# Classification results for the events from\n# "
	 << evtF
	 << "\n# Total number of events was " << events.size()
	 << "\n\n";
  
  TStopwatch timer;
  timer.Start();

  // Class loop
  for(size_t cl = 0; cl < labels.size(); cl++)
  {
    // Current class Name
    std::string curClsName = labels[cl];
    size_t correctCls      = 0; // Correct classified
    size_t wrongCls        = 0; // Mis classified

    // Events Loop
    for(size_t k = 0; k < events.size(); k++)
    //for(size_t k = 0; k < 20; k++)
    {
      // Only the events from the current class.
      if( (events[k]).first == curClsName )
      {
	std::vector<float>* evt = (events[k]).second;
	
	// Get Mva Value
	// cls.GetMvaValues(*evt, res);
	//printResult(res);
		
	// Perform winner takes all.
	std::string* tmpClsName = cls.Classify(*evt);

	// std::cout << "Class Name is " << *tmpClsName << '\n';

	if( (*tmpClsName) == curClsName)
	{// Correct Label
	  correctCls++;
	  // Fill the distance to the wining Codebook.
	  (histograms[curClsName])->Fill(res[curClsName]);
	}
	else
	{// Wrong label classification.
	  wrongCls++;
	}
	// Delete string
	delete tmpClsName;
      }// End if
    }// Events Loop
    std::cout << "\t<-I-> Writing results for " << curClsName
	      << '\n';
    
    OutPut << "++++++++++++++ Classification Results ++++++++++++\n"
	   << "\tCurrent class Name " << curClsName
	   << "\n\tWe have seen " << (*counts)[curClsName] << " Events in this class\n"
	   << "\tNumber of Correct classified events = " << correctCls
	   << "\n\tNumber of mis-classified events = " <<  wrongCls
	   << "\n\tErro = "
	   << ((static_cast<float>(wrongCls) * 100.00)/static_cast<float>( (*counts)[curClsName] ))
	   << " %.\n"
	   << std::flush;
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

  //__________________ Clean up _____________//

  // Clean events
  std::cout << "Clean up.\n";
  for(size_t i = 0; i < events.size(); ++i)
  {
    delete (events[i]).second;
  }
  events.clear();

  // Results
  res.clear();

  // Write distances histogram.
  TFile routf(outHistFile.c_str(), "RECREATE");
  
  for(size_t h = 0; h < labels.size(); ++h)
  {
    std::string nn = labels[h];
    (histograms[nn])->Write();
  }
  routf.Close();

  // Clean histo list
  for(size_t h = 0; h < labels.size(); ++h)
  {
    std::string nn = labels[h];
    delete (histograms[nn]);
  }
  histograms.clear();

  // Delete per label example counts
  counts->clear();
  delete counts;

  return 0;
}
