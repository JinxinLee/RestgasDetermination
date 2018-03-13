int NeutronAnalysis()
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");


   gSystem->Load("libHypGe");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

	TString Filename = "$SIMDATADIR/TripleBall40Offset10_urqmd_pbarC_1_5000Evts.root";


//Output Files
	//TString Path = getenv("SIMDATADIR");
	TString outfile= "$SIMDATADIR/Data_Marcell/TripleBall40Offset10_urqmd_pbarC_1_5000Evts_spectrum.root";//"$SIMDATADIR/Data_Marcell/TripleBall40Offset10_urqmd_pbarC_1_5000Evts_spectrum.root";
	TFile* g = new TFile(Filename);
	TFile* fi = new TFile(outfile,"RECREATE");



   //photons from hyp electromag. decay
  TTree *b=(TTree *) g->Get("pndsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names




  //****photons from hyp elect. decay

	string Name = "Neutrons hitting the germaniums";
	TH1D* gamTde = new TH1D("gamTde",Name.c_str(),150, 150,180);
	TH1D* hCrystalHit = new TH1D("hCrystalHit","Crystals hit by neutrons",1700,1,1700);


	bool verbose = false;
	Int_t MotherId,Motherpdg;

	TH1D *hNoHits = new TH1D("Number of Hits", "Number of Hits", 16,0,15);

	TVector3 vecs,pos;
	int mcpdg = -1,ev;
	Double_t mult,En,Eng,Enth;

	//vector<int> event;
	int count;
	Int_t nEvents = b->GetEntriesFast();
	cout<< "Number of Simulated Events: "<<nEvents<<endl;


	Double_t Resolution = 2.;	//keV
	for (Int_t k=0; k<nEvents; k++)
	{
			//cout << k << endl;
		Eng=0.;
		b->GetEntry(k);
		if (!((k*100)% nEvents))
		{
			cout << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{
			//cout << hit_bar->GetEntriesFast()<<endl;
			PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);

			//PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());


			//rande= gRandom->Gaus(hitgam->GetEnergyLoss(),0.000003);
			if (hitgam->GetpdgCode()==2112)
			{
				gamTde->Fill(180/TMath::Pi()*TMath::ACos(hitgam->GetPz()/TMath::Sqrt(hitgam->GetPx()*hitgam->GetPx()+hitgam->GetPy()*hitgam->GetPy()+hitgam->GetPz()* hitgam->GetPz())));		//Fill spectrum
				hCrystalHit->Fill(hitgam->GetDetectorID());
			}
		}


	  }// end for j (events)

  gamTde->Write();
	hCrystalHit->Write();

  gamTde->Draw();
  TCanvas* c2 = new TCanvas("c2","c2",800,600);
  c2->cd();
  hCrystalHit->Draw();
  //Analysis of spectrum

	fi->Close();




	// -----   Finish   -------------------------------------------------------
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	cout << endl << endl;
	cout << "Macro finished succesfully." << endl;

	cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
	cout << endl;
	// ------------------------------------------------------------------------

  return 0;
}
