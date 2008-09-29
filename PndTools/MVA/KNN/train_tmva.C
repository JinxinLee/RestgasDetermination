#include <iostream>

#include "TCut.h"
#include "TFile.h"
#include "TSystem.h"
#include "TTree.h"
#include "TRandom3.h"

void train_tmva(Int_t nrOfEvents=10000000, Int_t nrOfFeatures=4, Int_t nrOfNeighbours=100, Char_t outputFilename[]="blah.root") 
{
  std::cout << "Start Test TMVAnalysis" << std::endl
	    << "======================" << std::endl
	    << std::endl;
  std::cout << "Testing all standard methods may take about 10 minutes of running..." << std::endl;
  
  // Create a new root output file.
  TString outfileName( outputFilename );
  TFile* outputFile = TFile::Open( outfileName, "RECREATE" );

  TMVA::Factory *factory = new TMVA::Factory( "TMVAnalysis", outputFile, 
					      Form("!V:!Silent:%sColor", gROOT->IsBatch()?"!":"") );
  
  Char_t name[256];
  TRandom3 myran(NULL);

  for (Int_t i=0; i<nrOfFeatures; i++)
    {
       sprintf(name,"p%d",i);
       factory->AddVariable( name, 'F' );
    }
  TString datFileS = "classa.dat";
  TString datFileB = "classb.dat";
  
  ofstream ofa;
  ofstream ofb;
  
  ofa.open(datFileS);
  ofb.open(datFileB);
  
  for (Int_t i=0; i<nrOfFeatures; i++)
    {
      sprintf(name,"p%d/F",i);
      ofa << name ;
      ofb << name ;
      
      if (i < (nrOfFeatures-1))
	{
	  ofa << ":" ;
	  ofb << ":" ;
	}
    }
  ofa << std::endl;
  ofb << std::endl;

  for (Int_t i=0 ; i<(nrOfEvents+100); i++)
    {
      for (Int_t j=0; j<nrOfFeatures; j++)
	{
	  ofa << myran.Uniform(0,1) << " ";
	  ofb << myran.Uniform(0,1) << " ";
	}
      ofa << std::endl;
      ofb << std::endl;
    }
  ofa.close();
  ofb.close();

   factory->SetInputTrees( datFileS, datFileB );

   sprintf(name,"NSigTrain=%d:NBkgTrain=%d:!V",nrOfEvents,nrOfEvents);
   factory->PrepareTrainingAndTestTree(NULL, NULL, name); 

   sprintf(name,"nkNN=%d:TreeOptDepth=6:ScaleFrac=0.8:!UseKernel:!Trim",nrOfNeighbours);
   factory->BookMethod( TMVA::Types::kKNN, "KNN", name );  

   factory->TrainAllMethods();

   //   factory->TestAllMethods();

   //   factory->EvaluateAllMethods();    

   // --------------------------------------------------------------
   
   // Save the output
   outputFile->Close();

   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAnalysis is done!" << std::endl;      

   delete factory;

}
