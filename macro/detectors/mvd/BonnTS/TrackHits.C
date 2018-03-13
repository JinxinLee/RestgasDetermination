/*
 * TrackHits.C
 *
 *  Created on: Mar  4, 2010
 *      Author: HGZ
 */

int TrackHits(){

	// Customize

	const Int_t numSens = 6;
   const Int_t maxEvents = 1000000;

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
	TString HitsFile = "hits.root";


	// Loading the geometry and defining the geo handler

	TFile *geo = new TFile(geomFile);

	TGeoManager *myGeo = geo->Get("FAIRGeom");

	fGeoH = new PndGeoHandling();


	TH1F *test = new TH1F("Det","Det",5,0.5,numSens + 0.5);

	TH2F *h1 = new TH2F("Det1","Det1",385,-0.96,+0.96,385,-0.96,+0.96);
	TH2F *h2 = new TH2F("Det2","Det2",385,-0.96,+0.96,385,-0.96,+0.96);
   TH2F *h3 = new TH2F("Det3","Det3",385,-0.96,+0.96,385,-0.96,+0.96);
   TH2F *h4 = new TH2F("Det4","Det4",385,-0.96,+0.96,385,-0.96,+0.96);

   TH1F *h5 = new TH1F("eloss1","eloss1",1000,0.,100.);
   TH1F *h6 = new TH1F("eloss2","eloss2",1000,0.,100.);
   TH1F *h7 = new TH1F("eloss3","eloss3",1000,0.,100.);
   TH1F *h8 = new TH1F("eloss4","eloss4",1000,0.,100.);

	TCanvas *can0 = new TCanvas();
   TCanvas *can1 = new TCanvas();
   TCanvas *can2 = new TCanvas();
	TCanvas *can3 = new TCanvas();
	TCanvas *can4 = new TCanvas();
   TCanvas *can5 = new TCanvas();
   TCanvas *can6 = new TCanvas();
   TCanvas *can7 = new TCanvas();
   TCanvas *can8 = new TCanvas();

	TString name = "";


	// Load the hits
	TFile *f = new TFile(HitsFile);

	TTree *t=(TTree *) f->Get("pndsim");

	TClonesArray* tr_array=new TClonesArray("PndSdsHit");
	t->SetBranchAddress("MVDHitsStrip",&tr_array);//Branch names

    	cout << "Events: " << t->GetEntries() << endl;

   int nrHits1=0;
   int nrHits2=0;
   int nrHits3=0;
   int nrHits4=0;

	for (Int_t j = 0 ; j < t->GetEntries() && j<maxEvents ; j++) // loop on events
	{
		t->GetEvent(j);

		if (j%10000 == 0) cout << "Ev. " << j << endl;

		TObjArray TS3[2];
      TObjArray TS4[2];

      for (Int_t y = 0 ; y < tr_array->GetEntries() ; y++) // loop on hits
      {

		    PndSdsHit* point = (PndSdsHit*)tr_array->At(y);
		    //cout<< "index: "<<point->GetClusterIndex() << endl;
          //cout<<" (x,y)=("<<point->GetX()<<","<<point->GetY()<<") charge="<<point->GetEloss()<<endl;
		    name =  fGeoH->GetPath(point->GetDetName());

		    if (j < 10) cout << name << endl;

		    for (Int_t h = 0 ; h < numSens ; h++) // loop on sensors
		    {
			      if (name == SensName[h]) test->Fill(h+1);
          }

		    // plotting Z of hits on the first sensor
		   if (name == SensName[0]) {
            h1->Fill(point->GetX(),point->GetY());
            h5->Fill(point->GetEloss()*1e+6);
            nrHits1++;
         }
		   if (name == SensName[5]) {
            h4->Fill(point->GetX(),point->GetY());
            h8->Fill(point->GetEloss()*1e+6);
            nrHits4++;
         }

		   if (name == SensName[1]) TS3[0].Add(point);
         if (name == SensName[2]) TS3[1].Add(point);
         if (name == SensName[3]) TS4[0].Add(point);
         if (name == SensName[4]) TS4[1].Add(point);
		}
//      cout<<"entries in TSn:"<<endl;

      Int_t n1,n2,n3,n4;
      n1=TS3[0].GetEntries();
      n2=TS3[1].GetEntries();
      n3=TS4[0].GetEntries();
      n4=TS4[1].GetEntries();
//      if (n1!=n2) cout<<"unequal nr. of hits on sensor TS3 ("<<n1<<"/"<<n2<<")"<<endl;
//      if (n3!=n4) cout<<"unequal nr. of hits on sensor TS4 ("<<n3<<"/"<<n4<<")"<<endl;
      if (n1 && n1==n2) {
         if (n1==1) {
            h2->Fill(((PndSdsHit*)(TS3[0][0]))->GetX(),((PndSdsHit*)(TS3[1][0]))->GetY());
            h6->Fill(((PndSdsHit*)(TS3[0][0]))->GetEloss()*1e+6);
            h6->Fill(((PndSdsHit*)(TS3[1][0]))->GetEloss()*1e+6);
         }
         else cout<<"multiple hits on sensor TS3"<<endl;
         nrHits2+=n1+n2;
      }
      if (n3 && n3==n4) {
         if (n3==1) {
            h3->Fill(((PndSdsHit*)(TS4[0][0]))->GetX(),((PndSdsHit*)(TS4[1][0]))->GetY());
            h7->Fill(((PndSdsHit*)(TS4[0][0]))->GetEloss()*1e+6);
            h7->Fill(((PndSdsHit*)(TS4[1][0]))->GetEloss()*1e+6);
         }
         else cout<<"multiple hits on sensor TS4"<<endl;
         nrHits3+=n3+n4;
      }

/*
      cout<<" TS3a:"<<n1<<endl;
      cout<<" TS3b:"<<n2<<endl;
      cout<<" TS4a:"<<n3<<endl;
      cout<<" TS4b:"<<n4<<endl;
*/
/*
      TS3[0].clear();
      TS3[1].clear();
      TS4[0].clear();
      TS4[1].clear();
*/
	} // end loop on events

   cout<<"nr of hits on first station : "<<nrHits1<<endl;
   cout<<"nr of hits on second station : "<<nrHits2<<endl;
   cout<<"nr of hits on third station : "<<nrHits3<<endl;
   cout<<"nr of hits on fourth station : "<<nrHits4<<endl;

	can0->cd();

	test->Draw();
	test->GetXaxis()->SetTitle("Sensor");
	test->GetYaxis()->SetTitle("Clusters");

	can1->cd();

	gStyle->SetPalette(1);
	h1->Draw("COLZ");
	h1->GetXaxis()->SetTitle("x [cm]");
	h1->GetYaxis()->SetTitle("y [cm]");

	can2->cd();

	gStyle->SetPalette(1);
	h2->Draw("COLZ");
	h2->GetXaxis()->SetTitle("x [cm]");
	h2->GetYaxis()->SetTitle("y [cm]");

   can3->cd();

   gStyle->SetPalette(1);
   h3->Draw("COLZ");
   h3->GetXaxis()->SetTitle("x [cm]");
   h3->GetYaxis()->SetTitle("y [cm]");

   can4->cd();

   gStyle->SetPalette(1);
   h4->Draw("COLZ");
   h4->GetXaxis()->SetTitle("x [cm]");
   h4->GetYaxis()->SetTitle("y [cm]");

   can5->cd();

   h5->Draw("");
   h5->GetXaxis()->SetTitle("eloss [keV]");
   h5->GetYaxis()->SetTitle("counts");

   can6->cd();

   h6->Draw("");
   h6->GetXaxis()->SetTitle("eloss [keV]");
   h6->GetYaxis()->SetTitle("counts");

   can6->cd();

   h6->Draw("");
   h6->GetXaxis()->SetTitle("eloss [keV]");
   h6->GetYaxis()->SetTitle("counts");

   can7->cd();

   h7->Draw("");
   h7->GetXaxis()->SetTitle("eloss [keV]");
   h7->GetYaxis()->SetTitle("counts");

   can8->cd();

   h8->Draw("");
   h8->GetXaxis()->SetTitle("eloss [keV]");
   h8->GetYaxis()->SetTitle("counts");
  return 0;
}
