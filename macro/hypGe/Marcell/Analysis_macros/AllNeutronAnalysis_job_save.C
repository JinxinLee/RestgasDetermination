void AllNeutronAnalysis_job(TString Filename_ext)
{
	bool verbose = false;
  // -----  Load libraries   ------------------------------------------------
   gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
   gSystem->Load("libHypGe");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
	
	
	//Files
							//TString Path = getenv("SIMDATADIR");
	TString Filename = "$SIMDATADIR/Neutron/"+Filename_ext;
	//TString Filename = "$SIMDATADIR/TripleBall40Offset10_urqmd_pbarC_1_5000Evts.root";
	TString outfile= "$SIMDATADIR/Neutron/Ana/Ana" + Filename_ext;
	TFile* InputFile = new TFile(Filename);
	TFile* OutputFile = new TFile(outfile,"RECREATE");
	
	//getting simulation branches from input file
  TTree *b=(TTree *) InputFile->Get("cbmsim") ;
  TClonesArray* hit_bar=new TClonesArray("PndHypGePoint");
  b->SetBranchAddress("HypGePoint",&hit_bar);//Branch names
  TClonesArray* mc_bar=new TClonesArray("PndMCTrack");
  b->SetBranchAddress("MCTrack",&mc_bar);//Branch names

	//declaring histograms
		//histogram with polar angle of detector interactions of primary neutrons
	TH1D* hNHits = new TH1D("hNHits","Polar angle of primary neutrons interacting with the crystals",180, 90,180);
	hNHits->SetXTitle("#Theta [°]");
	hNHits->SetYTitle("Counts");
		//histogram with the polar momentum of primary neutrons interacting with the crystals
	TH1D* hNHits_p = new TH1D("hNHits_p","Direction of polar momentum of primary neutrons interacting with the crystals",180, 90,180);
	hNHits_p->SetXTitle("#Theta [°]");
	hNHits_p->SetYTitle("Counts");
		//histogram with the polar momentum of primary neutrons in the solid angle of the crystal (not necessarily interacting with the crystal)
	TH1D* hNAll = new TH1D("hNAll","All Neutrons",180, 90,180);
	hNAll->SetXTitle("#Theta [°]");
	hNAll->SetYTitle("Counts");
	//histogram with E_kin of primary neutrons
	TH1D* hNMom = new TH1D("hNMom","E_{kin} of neutrons",1000, 0,0.5);
	hNMom->SetXTitle("E_{kin} of neutrons [GeV]");
	hNMom->SetYTitle("Counts");
		//histogram to see which detector is hit
	TH1D* hCrystalHit = new TH1D("hCrystalHit","Hits per Crystal",1700,1,1700);
	hCrystalHit->SetXTitle("Crystal number");
	hCrystalHit->SetYTitle("Counts");
  

	Int_t *ActualTrackID ;
	Int_t nEvents = b->GetEntriesFast();
	cout<< "Number of Simulated Events: "<<nEvents<<endl;

	for (Int_t k=0; k<nEvents; k++)
	{ 
		b->GetEntry(k);
		if (!((k*100)% nEvents))
		{
			cout << "Event number :\t" << k << endl;
		}
	    //if(verbose) cout<<"Event No "<<j<<endl;
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{
			//cout <<"Hit "<< i << endl;
			if (i == 0) // because of trackID
			{
			//cout << hit_bar->GetEntriesFast()<<endl;
				PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
				ActualTrackID=-1;
				PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
				if (hitgam->GetpdgCode()==2112 && mcgam->GetMotherID()==-1)
				{
					ActualTrackID= hitgam->GetTrackID();
					TVector3 NHit;
					NHit.SetX(hitgam->GetX());
					NHit.SetY(hitgam->GetY());
					NHit.SetZ(hitgam->GetZ()+55);
					hNHits->Fill(180/TMath::Pi()*NHit.Theta());				//fill histogram with polar angle of detector interactions of primary neutrons
					
					TVector3 NHit_p;
					NHit_p.SetX(hitgam->GetPx());
					NHit_p.SetY(hitgam->GetPy());
					NHit_p.SetZ(hitgam->GetPz());
					hNHits_p->Fill(180/TMath::Pi()*NHit_p.Theta());		//fill histogram with the polar momentum of primary neutrons interacting with the crystals
					hCrystalHit->Fill(hitgam->GetDetectorID());				//fill histogram to see which detector is hit
					//hNHits->Fill(180/TMath::Pi()*TMath::ACos(hitgam->GetZ()/TMath::Sqrt(hitgam->GetX()*hitgam->GetX()+hitgam->GetY()*hitgam->GetY()+hitgam->GetZ()* hitgam->GetZ())));		//Fill spectrum
				}
				if (mcgam->GetPdgCode()==2112 && mcgam->GetMotherID()==-1)
				{
					TVector3 NAllMom = mcgam->GetMomentum();
					hNAll->Fill(180/TMath::Pi()*NAllMom.Theta());		//fill histogram with the polar momentum of primary neutrons in the solid angle of the crystal (not necessarily interacting with the crystal)
					hNMom->Fill(1./2.*NAllMom.Mag2()/0.939);				//fill histogram with E_kin of primary neutrons
					//cout << 1./2.*NAllMom.Mag2()/0.939<<endl;
				}
			}
			else
			{
				PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
				PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
				//rande= gRandom->Gaus(hitgam->GetEnergyLoss(),0.000003);
				if (hitgam->GetpdgCode()==2112 && mcgam->GetMotherID()==-1)
				{
					if (ActualTrackID != hitgam->GetTrackID())
					{
						TVector3 NHit;
						NHit.SetX(hitgam->GetX());
						NHit.SetY(hitgam->GetY());
						NHit.SetZ(hitgam->GetZ()+55);
						hNHits->Fill(180/TMath::Pi()*NHit.Theta());
						TVector3 NHit_p;
						NHit_p.SetX(hitgam->GetPx());
						NHit_p.SetY(hitgam->GetPy());
						NHit_p.SetZ(hitgam->GetPz());
						hNHits_p->Fill(180/TMath::Pi()*NHit_p.Theta());
						ActualTrackID = hitgam->GetTrackID();
						hCrystalHit->Fill(hitgam->GetDetectorID());
					}
				}
				if (mcgam->GetPdgCode()==2112 && mcgam->GetMotherID()==-1)
				{
					TVector3 NAllMom = mcgam->GetMomentum();
					hNAll->Fill(180/TMath::Pi()*NAllMom.Theta());
					hNMom->Fill(1./2.*NAllMom.Mag2()/0.939);
					//cout << 1./2.*NAllMom.Mag2()/0.939<<endl;
				}
				
			}
		}
	    
	    
	  }// end for j (events)
  

 
  hNHits->Draw();
  
  //Analysis of spectrum
  hNHits->Write();
  hNHits_p->Write();
	hNAll->Write();
	hNMom->Write();
	hCrystalHit->Write();

	OutputFile->Close();




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
