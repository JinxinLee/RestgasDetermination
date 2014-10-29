void AllNeutronAnalysis() 
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
  TTree *b=(TTree *) g->Get("cbmsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names


      
  
  //****photons from hyp elect. decay
	
	string Name = "Neutrons hitting the germaniums";
	TH1D* hNHits = new TH1D("hNHits",Name.c_str(),180, 90,180);
	TH1D* hNAll = new TH1D("hNAll","All Neutrons",180, 90,180);
 
  
	bool verbose = false;
	Int_t MotherId,Motherpdg;
	

	
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
		
		
			//cout << hit_bar->GetEntriesFast()<<endl;
			PndHypGePoint *hitgam0=(PndHypGePoint*)hit_bar->At(0);
			PndHypGePoint *hitgam1=(PndHypGePoint*)hit_bar->At(1);
			cout << hitgam0 << endl;
			if (hitgam0 != 0 && hitgam1 !=0)
			{
			if(hitgam0->GetTrackID() != hitgam1->GetTrackID())
			{
				continue;
			}
			PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam0->GetTrackID());
			if (mcgam->GetPdgCode()!=2112)
			{
				continue;
			}

			hNHits->Fill(180/TMath::Pi()*TMath::ACos(hitgam0->GetPz()/TMath::Sqrt(hitgam0->GetPx()*hitgam0->GetPx()+hitgam0->GetPy()*hitgam0->GetPy()+hitgam0->GetPz()* hitgam0->GetPz())));		//Fill spectrum
			if(mcgam->GetMotherID() ==-1)
			{
				TVector3 NAllMom = mcgam->GetMomentum();
				hNAll->Fill(180/TMath::Pi()*NAllMom.Theta());
			}
		}

	   if (hitgam0 != 0 )
			{
			if(hitgam0->GetTrackID() != hitgam1->GetTrackID())
			{
				continue;
			}
			PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam0->GetTrackID());
			if (mcgam->GetPdgCode()!=2112)
			{
				continue;
			}

			hNHits->Fill(180/TMath::Pi()*TMath::ACos(hitgam0->GetPz()/TMath::Sqrt(hitgam0->GetPx()*hitgam0->GetPx()+hitgam0->GetPy()*hitgam0->GetPy()+hitgam0->GetPz()* hitgam0->GetPz())));		//Fill spectrum
			if(mcgam->GetMotherID() ==-1)
			{
				TVector3 NAllMom = mcgam->GetMomentum();
				hNAll->Fill(180/TMath::Pi()*NAllMom.Theta());
			}
		}
	    
	}// end for j (events)
  

 
  hNHits->Draw();
  
  //Analysis of spectrum
  hNHits->Write();
	hNAll->Write();
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
	
}
