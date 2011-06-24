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
