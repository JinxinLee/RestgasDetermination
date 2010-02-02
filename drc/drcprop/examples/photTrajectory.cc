
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



void photTrajectory( TString inFilename = "", Double_t photonID = 1  ) // ID 0 means all
{

	if( inFilename == "" )
	{
		cout << "Usage: photTrajectory( filename, photonID )" << endl;
		return;
	}


//==============================================================================
// Access to the input ROOT-file & canvas settings
//==============================================================================
	TFile *inFile   = new TFile( inFilename );
	TCanvas *setup = (TCanvas*) inFile->Get("Setup");
	TTree *photonTree = (TTree*) inFile->Get("photon");


	Double_t posX[1001];
	Double_t posY[1001];
	Double_t posZ[1001];
	Int_t index_pos, nRefl;
	Bool_t measured, absorbed, lost;

	photonTree->SetBranchAddress( "posX[1001]", posX );
	photonTree->SetBranchAddress( "posY[1001]", posY );
	photonTree->SetBranchAddress( "posZ[1001]", posZ );
	photonTree->SetBranchAddress( "index_pos" , &index_pos );
	photonTree->SetBranchAddress( "nRefl"     , &nRefl );
	photonTree->SetBranchAddress( "measured"  , &measured );
	photonTree->SetBranchAddress( "absorbed"  , &absorbed );
	photonTree->SetBranchAddress( "lost"      , &lost );

	Int_t nEntries = photonTree->GetEntries();


	TCanvas *canvas = new TCanvas( "canvas", "" ,200, 10, 700, 510 );
	canvas->Draw();


//==============================================================================
// Event loop
//==============================================================================
	Int_t n_ph = 0;

	for( int i = 0; i < nEntries; i++ )
	{
		photonTree->GetEntry( i );

		n_ph++;

		if( n_ph == photonID || photonID == 0 )
		{
// 			cout << "Reflections: " << nRefl << endl;
// 			cout << "Fate       : ";
// 			if( measured == true )
// 				cout << "measured" << endl;
// 			if( absorbed == true )
// 				cout << "absorbed" << endl;
// 			if( lost == true )
// 				cout << "lost" << endl;

			for( int j = 0; j < index_pos; j++ )
			{
				if( j > 0 )
				{
					TPolyLine3D *l = new TPolyLine3D(2);

					l->SetPoint(0, posStartX, posStartY, posStartZ);
					l->SetPoint(1, posX[j], posY[j], posZ[j]);
					l->SetLineColor(3);
					l->Draw();
				}

				double posStartX = posX[j];
				double posStartY = posY[j];
				double posStartZ = posZ[j];

// 				cout << "pos.: (" << posStartX << "," << posStartY << "," << posStartZ << ")" << endl;
			}
		}
	}


//==============================================================================
// Plot
//==============================================================================
	setup->DrawClonePad();
	setup->Close(); // w/o -> Error in <RootX11ErrorHandler>


}
