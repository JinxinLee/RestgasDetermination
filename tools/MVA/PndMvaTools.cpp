/* ********************************************
 * MVA Tools and function definitions.        *
 * Author: M.Babai@rug.nl                     *
 * Version:                                   *
 * License:                                   *
 * *******************************************
 */
#include "PndMvaTools.h"

/**
 * Print list of ROC objects.
 *@param RocList The list of ROC points
 */
void printRoc(std::vector< ROCPoints > const& rc)
{
  for(size_t i = 0; i < rc.size(); ++i)
  {
    std::cout << "rc ["<< i<< "] { FP_rate = " << rc[i].FP_rate
	      << " TP_rate = " << rc[i].TP_rate
	      << " TN_rate = " << rc[i].TN_rate
	      << " FN_rate = " << rc[i].FN_rate
	      << " fp = " << rc[i].fp
	      << " tp = " << rc[i].tp
	      << " thr = " << rc[i].thr
	      << " }\n";
  }
}

// Print the results map.
void print(std::map<std::string, float> const& res)
{
  std::cout << "\n================================== \n";
  std::map<std::string,float>::const_iterator ii;
  for( ii = res.begin(); ii != res.end(); ++ii)
  {
    std::cout << (*ii).first << " => " << (*ii).second << '\n';
    //std::cout << (*ii).first << " => " << (1 - (*ii).second) << '\n';
  }
  std::cout << "======================================= \n";
}

/**
 * Write the list of ROC objects in a file.
 *@param FileName The name of the file to write into.
 *@param RocList The list of the ROC poits to write.
 */
void WriteRocToFile( std::string const& fName,
		     std::vector< ROCPoints > const& rc)
{
  std::cout << "<-I-> Write ROC values to file " << fName
	    << '\n';
  std::ofstream OutPut;
  
  OutPut.open (fName.c_str());
  OutPut << "# ROC graph points\n"
	 << "# <index>\t <FP_rate>\t <TP_rate>\t <TN_rate>\t <FN_rate>\t"
	 << " <fp>\t <tp>\t <fn>\t <tn>\t <threshold>\n";
  for(size_t i = 0; i < rc.size(); ++i)
  {
    OutPut << "   " << i << "\t "
	   << rc[i].FP_rate << "\t "
	   << rc[i].TP_rate << "\t "
	   << rc[i].TN_rate << "\t "
	   << rc[i].FN_rate << "\t "
	   << rc[i].fp << "\t "
	   << rc[i].tp << "\t "
	   << rc[i].fn << "\t "
	   << rc[i].tn << "\t "
	   << rc[i].thr
	   << '\n';
  }
  OutPut.close();
}

/**
 * Print the list of classifier outputs.
 *@param OutPutList The list of outputs created by a classifier.
 */
void print(std::vector <ClassifierOutPuts> const& el)
{
  for(size_t i = 0; i < el.size(); ++i)
  {
    std::cout << "O_Label = "   << el[i].realLabel
	      << ", G_Label = " << el[i].givenLabel
	      << ", SgValue = " << el[i].sgValue
	      << ", bgValue = " << el[i].bgValue
	      << '\n';
  }
  std::cout << '\n';
}

/**
 * Reads the event data from the inputfile.
 *@param inFile The input file.
 *@param varNames Name of the parameters (branches per tree).
 *@param classNames Name of the labels to read.
 *@param coNt Output is stored here.
 */
std::map<std::string, size_t>* readEvents(const char* infile,
					  std::vector<std::string> const& varNames,
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
    size_t NumEvtCurLabel = static_cast<size_t>(t->GetEntries());
    
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
    for(int k = 0; k < t->GetEntries(); k++)
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

/**
 * Function to produce ROC curve. This will work if the classifier can
 * produce probs or scores.
 *@param input   Vector containing classifier outputs for a given test data set.
 *@param SigName Signal name.
 *@param BgName  Background name.
 *@param sigCnt  Number of signal events.
 *@param bgCnt   Number of background events.
 *@param Roc     The list of ROC points (output var).
 */
void Produce_ROC(std::vector< ClassifierOutPuts >& input,
		 std::string const& SigName, std::string const& BgName,
		 size_t sigCnt, size_t bgCnt, std::vector< ROCPoints >& Roc)
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

  // We need to find Min and Max output for Signal.
  std::sort(input.begin(), input.end());
  std::reverse(input.begin(), input.end());
    
  float fprev, trhold, fpRate, tpRate;
  float tnRate, fnRate;
  size_t fpCnt, tpCnt, fn, tn;

  fprev = std::numeric_limits<float>::min();
  trhold = fprev;

  fpRate = tpRate = tnRate = fnRate = 0.00;
  fpCnt = tpCnt = fn = tn = 0;
  size_t idx = 0;
  
  while( idx < input.size() )
  {
    // True positief.
    tpRate = static_cast<float>(tpCnt)/sg;
    
    // False negatief.
    fpRate = static_cast<float>(fpCnt)/bg;
    
    // True negatief.
    tnRate = static_cast<float>(tn)/bg;
    
    // False negatief.
    fnRate = static_cast<float>(fn)/sg;

    // if(fprev != input[idx].sgValue)
    if( (fprev > input[idx].sgValue) ||
	(fprev < input[idx].sgValue)
	)
    {
      Roc.push_back(ROCPoints(fpRate, tpRate, tnRate, fnRate,
			      fpCnt, tpCnt, fn, tn, trhold));
      // New threshold
      fprev = input[idx].sgValue;
      trhold = fprev;
      // Determine new TN and FN values.
      // Reste counters.
      tn = fn = 0;
      //_____________
      BgName.size();
      /*
      // Assume the rest of the list is negatief.
      for(size_t k = idx; k < input.size(); ++k)
      {
      // If input[idx] == True Negatief
      if( input[idx].realLabel == BgName )
      {
      tn++;
      }
      else// False negatief.
      {
      fn++;
      }
      }// End for(idx to size())
      */
    }//END IF

    // If input[idx] == True positief
    if( input[idx].realLabel == SigName )
    {
      tpCnt++;
    }
    else// False positief.
    {
      fpCnt++;
    }    
    idx++;
  }//END While
  //(1,1)
  Roc.push_back(ROCPoints(fpRate, tpRate, tnRate, fnRate,
			  fpCnt, tpCnt, fn, tn, trhold));
}
