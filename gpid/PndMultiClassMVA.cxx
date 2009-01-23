/*
 *  
 *  S.Vanniarajan  V.Suyam.Jothi@kvi.nl
 *
 *  This is the Mutivariate Event classification tool
 *  designed for PANDAROOT Analysis package for 
 *  PANDA Experiments.
 *       TMVA(Toolkit for MultiVariate Analysis) is a 
 *  two class classifier(signal and background). 
 *  available with ROOT package. mainly used for 
 *  Event Classification in High Energy Physics Experiments.
 *         
 *       This tool here is Designed from TMVA, for Multiclass 
 * Classification purpose. 
 *
 *                  S.Vanniarajan  01-08-08
 * Modified:
 * M. Babai
 */

#include "PndMultiClassMVA.h"
#define NUMBEROFTREES 50

using namespace std;
PndMultiClassMVA::PndMultiClassMVA():fNCLASS(0),fNVAR(0), fNSigTest("100"), 
				     fNSigTrain("100"), fPruneStrengthBDT("4.0"),
				     fBoostTypeBDT("AdaBoost"),fNCutsBDT("10"),
				     fNTreeBDT("100"), fNKNN("40")
{}
/*
  PndMultiClassMVA::PndMultiClassMVA( Int_t NoClass, 
  Int_t NoPar, TString fname,TString anaName )
  {
  fNCLASS   = NoClass;
  fNVAR     = NoPar;
  fFILENAME = fname;
  fAPPNAME  = anaName;
  }
*/
PndMultiClassMVA::~PndMultiClassMVA()
{
  fVarNameArray.clear();
  fClassNameArray.clear();
}

void PndMultiClassMVA::AddVar(const string varName)
{
  fVarNameArray.push_back(varName);
  //fNVAR = fVarNameArray.size()
  //We can remove this one.
  fNVAR += 1;
}

void PndMultiClassMVA::AddClass(const string className)
{
  //  vector<pair<string,string> > stringVec;
  fClassNameArray.push_back(className);
  //  fInFileNameArray.insert(pair<string,vector<pair<string,string> >
  //  >(className,stringVec));
  //This is equal to  fClassNameArray.size()
  // Thus we do not need this one.
  fNCLASS += 1;
}
/*
  void PndMultiClassMVA::AddInFile(string className,
  string simFileName,string recoFileName)
  {
  pair<string,string> stringPair;
  stringPair.first = simFileName;
  stringPair.second = recoFileName;
  cout<<"  -I-  Adding Input files for the class"<<className<<endl;
  if (fInFileNameArray.find(className) == fInFileNameArray.end()) 
  {
  cout<<"  -E- This class named "<<className<<" is not a part of your problem"<<endl;
  return; 
  }
  else
  {
  (fInFileNameArray.find(className)->second).push_back(stringPair); 
  }
  }
  
  void PndMultiClassMVA::GenerateTree()
  {
  cout<<"  -I-  Generating the Input Tree"<<endl;
  system("sleep 2");
  for(int i=0; i <fNCLASS; i++)
  {
  string className;
  vector < pair<string,string> > fileNameVec;
  className = fClassNameArray[i];
  fileNameVec = fInFileNameArray.find(className)->second;
  for(int j = 0; j < fileNameVec.size();j++ )
  {
  string simFile,recoFile;
  pair<string,string> filePair;
  filePair = fileNameVec.at(j);
  cout<<filePair.first<<"  "<<filePair.second<<endl;   
  }  
  }
  }
*/

void PndMultiClassMVA::WriteConfigFile()
{
  TString fileName = fAPPNAME + ".dat";
  TString varString = ":";
  TString classString = ":";
  fstream config;
  
  config.open(fileName,fstream::out);
  config<<fNVAR<<endl;
  config<<fNCLASS<<endl;
  
  for (int i = 0; i < fNVAR; i++ ) {
    varString = varString + fVarNameArray.at(i) + ":";
  }
  
  for (int i = 0; i < fNCLASS; i++ ) {
    classString = classString + fClassNameArray.at(i) + ":";
  }
  
  config<<varString<<endl;
  config<<classString<<endl;

  config.close(); 
}

void PndMultiClassMVA::TrainTest()
{
  TFile *input(0);
  if (fNCLASS < 2 ) {
    std::cout<< " you need atleast two classes for the classification "<<endl;
    return; //exit(0);
  }
  
  else if (fNVAR < 2 ) {
    std::cout<< " you need atleast two variables for the Multivariate Analysis "<<endl;
    return; //exit(0);
  }
  
  else if (!gSystem->AccessPathName( fINFILENAME )) {
    std::cout << "--- BDTAnalysis  : accessing " << fINFILENAME << std::endl;
    input = TFile::Open( fINFILENAME );
  } 
  else if (!input) {
    std::cout << "ERROR: could not open data file" << std::endl;
    return; //exit(0);
  }
  
  {//
    //TTree* TreeArray[50]; // = new TTree[10];
    TTree* TreeArray[NUMBEROFTREES]; // = new TTree[10];

    for (int i = 0; i < fNCLASS ; i++)
    {
      TString s,treeName;
      treeName = fClassNameArray.at(i);
      cout<< (TTree*)input->Get(treeName)<<endl;  
      TreeArray[i] = (TTree*)input->Get(treeName);  
    }
    
    //TreeArray is filled, we can delete input
    input->Close();
    delete input;

    for (int i = 0 ; i < fNCLASS ; i++ )
    {
      TString s,OutFileName,anaName;
      anaName = fAPPNAME +fClassNameArray.at(i);
      s = s +".root";
      OutFileName = fClassNameArray.at(i) + ".root";
      
      TFile* outputFile = TFile::Open( OutFileName, "RECREATE" );
      cout<<"hai"<<endl;
      TMVA::Factory *factory = new TMVA::Factory( anaName, outputFile, 
						  Form("!V:%sColor", 0?"!":"") );
      Double_t signalWeight     = 1.0;
      Double_t backgroundWeight = 1.0;
      
      for (int j = 0 ; j < fNCLASS ; j++ )
      {
	if (i == j ) 
	{
	  factory->AddSignalTree(TreeArray[j],signalWeight);  
	}
	else
        {
	  factory->AddBackgroundTree(TreeArray[j],backgroundWeight); 
	}
      }
      
      for (int k = 0 ; k < fNVAR ; k++ )
      {
	TString varName;
	varName = fVarNameArray.at(k);
	factory->AddVariable(varName,'F');
      }
      // for example: TCut mycuts = "abs(var1)<0.5 && abs(var2-0.5)<1";
      TCut mycuts = "";
      TCut mycutb = ""; // for example: TCut mycutb = "abs(var1)<0.5";
      TString trainConfig = "NSigTrain=" + fNSigTrain + 
	":NBkgTrain=" + fNBkgTrain + ":NSigTest=" + 
	fNSigTest + ":NBkgTest=" + fNBkgTest + "SplitMode=Random:!V";
      cout<<trainConfig<<endl; 
      factory->PrepareTrainingAndTestTree( mycuts, mycutb, trainConfig ); 
      
      TString BdtConfig = "!H:!V:NTrees=" + fNTreeBDT + 
	":BoostType=" + fBoostTypeBDT + 
	":SeparationType=GiniIndex:nCuts=" + fNCutsBDT +
	"PruneMethod=CostComplexity:PruneStrength=" + fPruneStrengthBDT; 
      
      factory->BookMethod( TMVA::Types::kBDT, "BDT", BdtConfig); 
      TString kNNConfig = "nkNN=" + fNKNN + 
	":TreeOptDepth=6:ScaleFrac=0.8:!UseKernel:!Trim";
      factory->BookMethod( TMVA::Types::kKNN, "KNN", kNNConfig);
      
      factory->BookMethod( TMVA::Types::kMLP, 
			   "MLP", 
			   "Normalise:H:!V:NeuronType=tanh:NCycles=200:HiddenLayers=N+1,N:TestRate=5"); 
      
      factory->TrainAllMethods();
      factory->TestAllMethods();
      factory->EvaluateAllMethods();    
      outputFile->Close();
      delete outputFile;
      delete factory;
    }
    //FIXME, Maybe we need to delete the elements in TreeArray
    //Here. I think they are not used elsewhere.
  }//
  WriteConfigFile();
}

ClassImp(PndMultiClassMVA);
