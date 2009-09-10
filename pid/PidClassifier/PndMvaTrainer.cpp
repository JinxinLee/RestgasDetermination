/* **********************************************
 * MVA classifiers trainers interface.          *
 * Author: M. Babai                             *
 * M.Babai@rug.nl                               *
 * Version: 0.1 beta1.                          *
 * LICENSE:                                     *
 * **********************************************
 */

#include "PndMvaTrainer.h"

using namespace std;

//! Constructor
PndMvaTrainer::PndMvaTrainer(const std::string& InPut,
			     const std::vector<std::string>& ClassNames, 
			     const std::vector<std::string>& VarNames,
			     bool trim)
  : m_dataSets(InPut, ClassNames, VarNames)
{
  // Trim data set
  if(trim)
  {
    m_dataSets.Trim();
  }

  // Initialize class conditional means.
  m_dataSets.InitClsCondMeans();
  
  // Init random seed for this run.
  time_t seconds;
  seconds = time (NULL);
  m_RND_seed = seconds;

  /// DEBUG
  //m_dataSets.WriteDataSet("InputVarNormalized.root");
  ///DEBUG
}

//! Destructor
PndMvaTrainer::~PndMvaTrainer()
{
  // CLean Error container
  /*
  for(size_t i = 0; i < m_StepErro.size(); i++)
  {
    delete m_StepErro[i];
  }
  m_StepErro.clear();
  */
}

/**
 * Creates test and train data sets.
 * @param percent Percent of the data set to be used for testing and
 * cross-validation
 */
void PndMvaTrainer::splitTetsSet(int percent)
{
  TRandom3 rndIndx(m_RND_seed);
  
  const std::vector<std::pair<std::string, std::vector<float>*> >& events = m_dataSets.GetData();
  
  unsigned int TestEvtCnt = (percent * events.size()) / 100 ;
  
  std::cout << "<INFO> preparing train and test sets.\n"
	    << "       Test set containes "<< TestEvtCnt
	    <<" events and train set "<< (events.size() - TestEvtCnt) 
	    << std::endl;
  
  while(m_testSet_indices.size() < TestEvtCnt)
  {
    int trindx = static_cast<int>(rndIndx.Uniform(0.0, events.size() - 1));
    m_testSet_indices.insert(trindx);
  }
}

void PndMvaTrainer::WriteErroVect(const std::string FileName)
{
  std::ofstream Outfile;

  Outfile.open(FileName.c_str(),ios::out| ios::trunc);

  Outfile << "# Error vector output\n";
  Outfile << "#<Step number> \t<Train Error> \t<Test Error>\n";

  for(size_t i = 0; i < m_StepErro.size(); i++)
  {
    Outfile << " " << (m_StepErro[i]).m_step  <<"\t"
	    << " " << (m_StepErro[i]).m_trErr <<"\t"
	    << " " << (m_StepErro[i]).m_tsErr 
	    << std::endl;
  }
  Outfile << flush;
  Outfile.close();
}
/**
 * Write the training and normalization data to outFile.
 */
void PndMvaTrainer::WriteToWeightFile(const std::vector< std::pair<std::string, 
				      std::vector<float>*> >& weights)
{
  
  std::cout << "<INFO> Writing classifier out put to "
	    << m_outFile << std::endl;
  const std::vector<PndMvaClass>& classes      = m_dataSets.GetClasses();
  const std::vector<PndMvaVariable>& vars = m_dataSets.GetVars();
 
  /* 
   * Open out-put file and write coordinates.
   */
  if(m_outFile == "")
  {
    std::cerr << "<ERROR> The output file name could not be an empty string.\n"
	      << "        Set the outPut name if you want to store"
	      << " the generated weights." << std::endl;
    return;
  }

  TFile out (m_outFile.c_str(), "RECREATE", "WeightOutput", 9);

  for(size_t cls = 0; cls < classes.size(); cls++)
  {
    std::vector<float> buffer(vars.size(), 0.0);
    
    std::string name = classes[cls].Name;
    std::string desc = "Description Of " + name;
    const char* treeName = name.c_str();
    const char* treeDesc = desc.c_str();
    
    // Create a tree
    TTree sig (treeName, treeDesc);
    
    // Create branches and bind the variables
    for(size_t j = 0; j < vars.size(); j++)
    {
      std::string vname = vars[j].Name;
      std::string leaf  = vname + "/F" ;
      const char* bname = vname.c_str();
      const char* lname = leaf.c_str();
      
      // Bind the parameters to the tree elements.
      sig.Branch(bname, &buffer[j], lname);
    }
    
    // Fill The tree
    for(size_t i = 0; i < weights.size(); i++)
    {
      if(weights[i].first == name)
      {
        for(size_t k = 0; k < buffer.size(); k++)
        {
          buffer[k] = (weights[i].second)->at(k);
        }
        sig.Fill();
      }
    }
    // Write the created tree
    sig.Write();
  }//End for cls

  // Write normFactors
  std::vector<float> buffer(vars.size(), 0.0);
  std::string name = "NormFact";
  std::string desc = "desc of " + name;
  
  TTree fact(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    fact.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].NormFactor;
  }
  fact.Fill();
  fact.Write();

  // Write mean
  name = "Means";
  desc = "desc of " + name;
  
  TTree meanTree(name.c_str(), desc.c_str());

  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    meanTree.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].Mean;
  }
  meanTree.Fill();
  meanTree.Write();
  
  //Close open file
  out.Close();
}

void PndMvaTrainer::WriteToWeightFile(const std::vector<TMVA::PDEFoam*>& foamList)
{
  std::cerr << "<INFO> Writing Foams to file "
	    << m_outFile << std::endl;
  /* 
   * Open out-put file and write coordinates.
   */
  if(m_outFile == "")
  {
    std::cerr << "<ERROR> The output file name could not be an empty string.\n"
	      << "        Set the outPut name if you want to store"
	      << " the generated weights." << std::endl;
    return;
  }
  
  TFile rootFile (m_outFile.c_str(), "RECREATE", "foamfile", 9);

  for(size_t j = 0; j < foamList.size(); j++)
  {
    foamList[j]->Write(foamList[j]->GetFoamName().Data());

    // DEBUG FIXME
    foamList[j]->Print();
    std::cout << "  i is: " 
	      << typeid(foamList[j]).name() 
	      << std::endl;
  }
  
  const std::vector<PndMvaVariable>& vars = m_dataSets.GetVars();
  // Write normFactors
  std::vector<float> buffer(vars.size(), 0.0);
  std::string name = "NormFact";
  std::string desc = "desc of " + name;
  
  TTree fact(name.c_str(), desc.c_str());
  
  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    fact.Branch(bname, &buffer[j], lname);
  }
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].NormFactor;
  }
  fact.Fill();
  fact.Write();
  
  // Write mean
  name = "Means";
  desc = "desc of " + name;
  
  TTree meanTree(name.c_str(), desc.c_str());
  
  // Create branches and bind the variables
  for(size_t j = 0; j < vars.size(); j++)
  {
    std::string vname = vars[j].Name;
    std::string leaf  = vname + "/F" ;
    const char* bname = vname.c_str();
    const char* lname = leaf.c_str();
    
    // Bind the parameters to the tree elements.
    meanTree.Branch(bname, &buffer[j], lname);
  }
  
  // Fill the trees.
  for(size_t i = 0; i < vars.size(); i++)
  {
    buffer[i] = vars[i].Mean;
  }
  meanTree.Fill();
  meanTree.Write();
  
  // Close open file
  rootFile.Close();
}
