/*
 * ReadHits.C
 *
 *  Created on: Mar  1, 2010
 *      Author: Simone Bianco
 */

int ReadHits(){

	// Customize


	const Int_t numSens = 6;

	TString SensName[numSens];
	
	SensName[0] = "/TS_1/TTVol_0/TTDouble_0/StripActiveTD1_0";
	SensName[1] = "/TS_1/TTVol_0/TTSingle_0/StripActiveTS3a_0";
	SensName[2] = "/TS_1/TTVol_0/TTSingle_0/StripActiveTS3b_0";
	SensName[3] = "/TS_1/TTVol_0/TTSingle_0/StripActiveTS4a_0";
	SensName[4] = "/TS_1/TTVol_0/TTSingle_0/StripActiveTS4b_0";
	SensName[5] = "/TS_1/TTVol_0/TTDouble_0/StripActiveTD2_0";


	// load libs

	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

	//

	TString directory = gSystem->Getenv("VMCWORKDIR");
	TString geomFile = directory + "/geometry/TrackingStation.root";
	TString HitsFile = "test.hits.root";


	// Loading the geometry and defining the geo handler
	
	TFile *geo = new TFile(geomFile);

	TGeoManager *myGeo = geo->Get("FAIRGeom");

	fGeoH = new PndGeoHandling();


	TH1F *test = new TH1F("Det","Det",5,0.5,numSens + 0.5);

	TH2F *histOne = new TH2F("Det1","Det1",100,-1,+1,100,-1,1);

	TCanvas *can1 = new TCanvas();

	TCanvas *can2 = new TCanvas();

	TString name = "";
	

	// Load the hits


	TFile *f = new TFile(HitsFile);

	TTree *t=(TTree *) f->Get("cbmsim") ;
	
	TClonesArray* tr_array=new TClonesArray("PndSdsHit");
	t->SetBranchAddress("MVDHitsStrip",&tr_array);//Branch names

    	cout << "Events: " << t->GetEntries() << endl;

	for (Int_t j = 0 ; j < t->GetEntries() ; j++) // loop on events

	{

		t->GetEvent(j);

		if (j%10000 == 0) cout << "Ev. " << j << endl;

		for (Int_t y = 0 ; y < tr_array->GetEntries() ; y++) // loop on hits
		  {
			
		    PndSdsHit*point = (PndSdsHit*)tr_array->At(y);
		
		    name =  fGeoH->GetPath(point->GetDetName());

		    if (j < 10) cout << name << endl;

		    for (Int_t h = 0 ; h < numSens ; h++) // loop on sensors
		      {
		 
			if (name == SensName[h]) test->Fill(h+1);
			
		      }
		    
		    // plotting Z of hits on the first sensor

		    if (name == SensName[0])
		      {

			histOne->Fill(point->GetX(),point->GetY());

		      }
		}

	} // end loop on events

	can1->cd();

	test->Draw();
	test->GetXaxis()->SetTitle("Sensor");
	test->GetYaxis()->SetTitle("Clusters");

	can2->cd();

	gStyle->SetPalette(1);
	histOne->Draw("COLZ");
	histOne->GetXaxis()->SetTitle("x [cm]");
	histOne->GetYaxis()->SetTitle("y [cm]");
  return 0;

}

