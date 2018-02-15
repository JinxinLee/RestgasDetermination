/* ***************************************
 * PDEFoam  Trainerer                    *
 * Author: M.Babai@rug.nl                *
 * LICENSE:                              *
 * Version: 0.1 beta1.                   *
 * License:                              *
 * ***************************************
 */
#include "PndPDEFoamTrain.h"

PndPDEFoamTrain::PndPDEFoamTrain(const std::string& inputFile,
				 const std::vector<std::string>& ClassNames,
				 const std::vector<std::string>& VarNames,
				 bool trim)
  : PndMvaTrainer(inputFile, ClassNames, VarNames, trim)
{
  Init();
}

PndPDEFoamTrain::~PndPDEFoamTrain()
{
  std::cout << "Cleaning-up foam list." << std::endl;
  if(PseRan){
    delete PseRan;
    PseRan = 0;
  }
  
  for(size_t i = 0; i < m_foams.size(); i++)
  {
    delete m_foams[i];
  }
  m_foams.clear();
}

void PndPDEFoamTrain::Init( void )
{
  // default initialization called by all constructors
  
  // init PDEFoam options
  m_SigBgSeparated = true;   // default values for options
  m_Frac           = 0.001;
  m_VolFrac        = 30;
  m_nActiveCells   = 500;
  m_nCells         = m_nActiveCells * 2 - 1;
  m_nSampl         = 2000;
  m_nBin           = 10;
  m_OptRej         = 1;       // use unweighted MC events
  m_OptDrive       = 1;       // use variance reduction
  m_EvPerBin       = 1000;
  m_Chat           = 0;
  m_CutNmin        = true;
  m_Nmin           = 400;
  m_CutRMSmin      = false;   // default TFoam method
  m_RMSmin         = 0.01;
  
  m_Kernel         = TMVA::EKernel(0); // default: use no kernel
    
  PseRan = new TRandom3();  // Create random number generator for PDEFoams
  PseRan->SetSeed(m_RND_seed);
}

void PndPDEFoamTrain::Train()
{
  const std::vector<PndMvaClass>& ClassNames = m_dataSets.GetClasses();
  const std::vector< std::pair<std::string, std::vector<float>*> >& EvtData = m_dataSets.GetData();

  CalcXminXmax();

  // Create a foam object for each class
  for(size_t cls = 0; cls < ClassNames.size(); cls++)
  {
    std::string clsName = ClassNames[cls].Name;
    std::string dsc = ClassNames[cls].Name;// + "_Foam";
    TMVA::PDEFoam* fo = new TMVA::PDEFoam(dsc.c_str());
    m_foams.push_back(fo);
    
    InitFoam(m_foams[cls], TMVA::kSeparate);

    std::cout << "<INFO> Filling binary search tree of " 
	      << ClassNames[cls].Name 
	      << " with events" << std::endl;

    // insert event to BinarySearchTree
    for (size_t k = 0; k < EvtData.size(); k++)
    {
      // Check className or type
      if( EvtData[k].first == clsName)
      {
	std::vector<float>* evtVect = EvtData[k].second;
	const TMVA::Event* ev = new TMVA::Event(*evtVect, cls, 1.0, 1.0);
	//m_foams[cls]->FillBinarySearchTree(ev, TMVA::kSeparate, true);
	m_foams[cls]->FillBinarySearchTree(ev, true);
      }
    }
    // build foam
    m_foams[cls]->SetNElements(1);
    m_foams[cls]->Create(m_CutNmin);

    // Reset Cell Integrals
    // init space for 2 variables on every cell (N_ev, RMS)
    m_foams[cls]->SetNElements(2);
    m_foams[cls]->ResetCellElements();

    std::cout << "Filling foam cells with events" << std::endl;
    // loop over all events -> fill foam cells
    for (size_t k = 0; k< EvtData.size(); k++)
    {
      // Check className or type
      if(clsName == EvtData[k].first)
      {
	std::vector<float>* evtVect = EvtData[k].second;
	const TMVA::Event* ev = new TMVA::Event(*evtVect, cls, 1.0, 1.0);
	//m_foams[cls]->FillFoamCells(ev, TMVA::kSeparate, false);
	m_foams[cls]->FillFoamCells(ev, false);
      }
    }
    
    std::cout << "Check all cells and remove cells with volume 0" << std::endl;
    m_foams[cls]->CheckCells(true);
  }
  //All Foams are created and checked.
  WriteFoamsToFile();
}

void PndPDEFoamTrain::InitFoam(TMVA::PDEFoam *pdefoam, TMVA::EFoamType ft)
{
  if (!pdefoam){
    std::cout << "<Error> Null pointer given! (InitFoam)" 
	      << std::endl;
    return;
  }
  
  // set signal and background classes
  if (ft == TMVA::kSeparate || ft == TMVA::kDiscr)
  {
    pdefoam->SetSignalClass    (1);
    pdefoam->SetBackgroundClass(-1);
  }
  const std::vector<PndMvaVariable>& Vars =  m_dataSets.GetVars();

  pdefoam->SetFoamType(ft);
  pdefoam->SetkDim( Vars.size());
  pdefoam->SetVolumeFraction(m_VolFrac);

  pdefoam->SetnCells(m_nCells); // optional
  pdefoam->SetnSampl(m_nSampl);    // optional
  pdefoam->SetnBin(m_nBin);      // optional

  // pdefoam->SetOptRej(m_OptRej);    // optional
  // pdefoam->SetOptDrive(m_OptDrive);  // optional
  // pdefoam->SetEvPerBin(m_EvPerBin);  // optional
  // pdefoam->SetChat(m_Chat);      // optional

  // cuts
  
  // pdefoam->CutNmin(m_CutNmin);     // cut on minimal number of events per cell
  // pdefoam->SetNmin(m_Nmin);
  // pdefoam->CutRMSmin(m_CutRMSmin); // cut on minimal RMS in cell
  // pdefoam->SetRMSmin(m_RMSmin);
  
  // SetPseRan
  //pdefoam->SetPseRan(PseRan);

  // Init PDEFoam
  pdefoam->Init();
 
  // Set Xmin, Xmax
  SetXminXmax(pdefoam);
}

void PndPDEFoamTrain::SetXminXmax( TMVA::PDEFoam *pdefoam)
{
  if (!pdefoam){
    std::cout << "Null pointer given! (SetXminXmax)" << std::endl;
    return;
  }
  const std::vector<PndMvaVariable>& VarsList = m_dataSets.GetVars();

  // set upper/ lower limit in foam

  for (size_t idim = 0; idim < VarsList.size(); idim++)
  {
    pdefoam->SetXmin(idim, Xmin.at(idim));
    pdefoam->SetXmax(idim, Xmax.at(idim));
  }
}

// calculate Xmin and Xmax for Foam
void PndPDEFoamTrain::CalcXminXmax()
{
  Xmin.clear();
  Xmax.clear();
  const std::vector<PndMvaVariable>& variables = m_dataSets.GetVars();
  const std::vector< std::pair<std::string, std::vector<float>*> >& events = m_dataSets.GetData();
  std::vector <float> tmpVals;
  
  // Loop over events and clac minimal and maximal value of every variable.
  // Variable Loop
  for(size_t var = 0; var < variables.size(); var++)
  {
    //Event Loop
    for(size_t ev = 0; ev < events.size(); ev++)
    {
      std::vector<float>* evtVars = events[ev].second;
      tmpVals.push_back(evtVars->at(var));
    }// event lopp

    // Have seen all events
    std::sort(tmpVals.begin(), tmpVals.end());
    float min, max;
    min = tmpVals[0];
    max = tmpVals[tmpVals.size() - 1];
    Xmin.push_back(min);
    Xmax.push_back(max);
    tmpVals.clear();
    // Print info
    std::cout << "<INFO> foam: SetXmin[varName = "
	      << variables[var].Name << "]: " << min 
	      << std::endl;
    std::cout << "<INFO> foam: SetXmax[VarName = " 
	      << variables[var].Name << "]: " << max 
	      << std::endl;
  }// variable loop
}

void PndPDEFoamTrain::FillVariableNamesToFoam() const
{
  const std::vector<PndMvaVariable>& VarsList = m_dataSets.GetVars();

  for(size_t ifoam = 0; ifoam < m_foams.size(); ifoam++)
  {
    for (Int_t dim=0; dim<m_foams[ifoam]->GetTotDim(); dim++)
    {
      m_foams[ifoam]->AddVariableName( (VarsList[dim].Name).c_str());
    }
  }
}

void PndPDEFoamTrain::WriteFoamsToFile()
{
  if(m_outFile == "")
  {
    std::cerr << "<Error> No output file name was set.\n"
	      << "Can not write to file"<< std::endl;
    return;
  }

  FillVariableNamesToFoam();
  WriteToWeightFile(m_foams);
}

// Parallel training of foams.
void PndPDEFoamTrain::TrainPar()
{
  const std::vector<PndMvaClass>& ClassNames = m_dataSets.GetClasses();
  const std::vector< std::pair<std::string, std::vector<float>*> >& EvtData = m_dataSets.GetData();
  
  CalcXminXmax();
  
  // Create a foam object for each class
  for(size_t cls = 0; cls < ClassNames.size(); cls++)
  {
    std::string clsName = ClassNames[cls].Name;
    std::string dsc = ClassNames[cls].Name;// + "_Foam";
    TMVA::PDEFoam* fo = new TMVA::PDEFoam(dsc.c_str());
    m_foams.push_back(fo);
    
    InitFoam(m_foams[cls], TMVA::kSeparate);
  }

  // Filling Foams
  int NumClasses = ClassNames.size();
  int cls = 0;
#ifdef _OPENMP
#pragma omp parallel shared(NumClasses) private(cls)
 {
#pragma omp for nowait
#endif
  for(cls = 0; cls < NumClasses; cls++)
  {
    std::string clsName = ClassNames[cls].Name;
    std::cout << "<INFO> Filling binary search tree of " 
	      << ClassNames[cls].Name 
	      << " with events" << std::endl;
    
    // insert event to BinarySearchTree
    for (size_t k = 0; k < EvtData.size(); k++)
    {
      // Check className or type
      if( EvtData[k].first == clsName)
      {
	std::vector<float>* evtVect = EvtData[k].second;
	const TMVA::Event* ev = new TMVA::Event(*evtVect, cls, 1.0, 1.0);
	//m_foams[cls]->FillBinarySearchTree(ev, TMVA::kSeparate, true);
	m_foams[cls]->FillBinarySearchTree(ev, true);
      }
    }// end insert binary tree

    // build foam
    m_foams[cls]->SetNElements(1);
    m_foams[cls]->Create(m_CutNmin);
    
    // Reset Cell Integrals
    // init space for 2 variables on every cell (N_ev, RMS)
    m_foams[cls]->SetNElements(2);
    m_foams[cls]->ResetCellElements();
    
    std::cout << "Filling foam cells with events" << std::endl;
    // loop over all events -> fill foam cells
    for (size_t k = 0; k< EvtData.size(); k++)
    {
      // Check className or type
      if(clsName == EvtData[k].first)
      {
	std::vector<float>* evtVect = EvtData[k].second;
	const TMVA::Event* ev = new TMVA::Event(*evtVect, cls, 1.0, 1.0);
	//m_foams[cls]->FillFoamCells(ev, TMVA::kSeparate, false);
	m_foams[cls]->FillFoamCells(ev, false);
      }
    }// end event loop
    
    std::cout << "Check all cells and remove cells with volume 0" << std::endl;
    m_foams[cls]->CheckCells(true);
  }// End filling foams (class loop)
#ifdef _OPENMP
 }
#endif
  //All Foams are created and checked.
  WriteFoamsToFile();
}
// =========================== end of implementation ===============

/*
//std::ofstream tfile(m_outPutFName.c_str());
std::ofstream tfile("AFoamTestFiletxt.root");
for(size_t j = 0; j < m_foams.size(); j++)
{
m_foams[0]->PrintStream(tfile);
}
tfile.close();
// XML OutPut (Do not use)
void* doc      = TMVA::gTools().xmlengine().NewDoc();
void* rootnode = TMVA::gTools().xmlengine().NewChild(0,0,"MethodSetup");
TMVA::gTools().xmlengine().DocSetRootElement(doc,rootnode);
TMVA::gTools().xmlengine().NewAttr(rootnode,0,"Method", "FOAM::PDE");
m_foams[0]->AddXMLTo(rootnode);
m_foams[1]->AddXMLTo(rootnode);
//m_foams[2]->AddXMLTo(rootnode);
//m_foams[3]->AddXMLTo(rootnode);
//TMVA::gTools().xmlengine().SaveDoc(doc, m_outPutFName.c_str());
TMVA::gTools().xmlengine().SaveDoc(doc, "WeightsXML.root");
*/
