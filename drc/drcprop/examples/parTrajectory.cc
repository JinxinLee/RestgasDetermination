
//======
// ROOT
//======
#include <TCanvas.h>
#include <TFile.h>
#include <TPolyLine.h>
#include <TString.h>
#include <TTree.h>


//=========
// STD C++
//=========
#include <iostream>


using namespace std;



void parTrajectory( TString inFilename = ""  )
{

	if( inFilename == "" )
	{
		cout << "Usage: parTrajectory( filename )" << endl;
		return;
	}


//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
	TFile *inFile   = new TFile( inFilename );
	TCanvas *setup = (TCanvas*) inFile->Get("Setup");
	TTree *particleTree = (TTree*) inFile->Get("particle");


	Double_t spotX, spotY, spotZ, spotEndX, spotEndY, spotEndZ;

	particleTree->SetBranchAddress( "spotX"     , &spotX );
	particleTree->SetBranchAddress( "spotY"     , &spotY );
	particleTree->SetBranchAddress( "spotZ"     , &spotZ );
	particleTree->SetBranchAddress( "spotEndX"  , &spotEndX );
	particleTree->SetBranchAddress( "spotEndY"  , &spotEndY );
	particleTree->SetBranchAddress( "spotEndZ"  , &spotEndZ );

	Int_t nEntries = particleTree->GetEntries();


	TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 1200, 500 );
	canvas->Draw();


//==============================================================================
// Event loop
//==============================================================================
	for( int i = 0; i < nEntries; i++ )
	{
		particleTree->GetEntry( i );

			TPolyLine3D *l = new TPolyLine3D(2);

			l->SetPoint(0, spotX, spotY, spotZ);
			l->SetPoint(1, spotEndX, spotEndY, spotEndZ);
			l->SetLineColor(3);
			l->Draw();
	}


//==============================================================================
// Plot
//==============================================================================
	setup->DrawClonePad();
	setup->Close(); // w/o -> Error in <RootX11ErrorHandler>


}
