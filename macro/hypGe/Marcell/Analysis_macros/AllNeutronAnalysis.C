



int AllNeutronAnalysis()
{
Int_t *ActualTrackID ;
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
	TString outfile= "$SIMDATADIR/Data_Marcell/TripleBall40Offset10_urqmd_pbarC_1_5000Evts_spectrum2.root";//"$SIMDATADIR/Data_Marcell/TripleBall40Offset10_urqmd_pbarC_1_5000Evts_spectrum.root";
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
	TH1D* hNHits_p = new TH1D("hNHits_p",Name.c_str(),180, 90,180);
	TH1D* hNAll = new TH1D("hNAll","All Neutrons",180, 90,180);
	TH1D* hNMom = new TH1D("hNMom","Momentum of Neutrons",1000, 0,0.5);
	TH1D* hCrystalHit = new TH1D("hCrystalHit","Crystals hit by neutrons",1700,1,1700);
  
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
		for (Int_t i=0; i<hit_bar->GetEntriesFast(); i++)
		{
			//cout <<"Hit "<< i << endl;
			if (i == 0)
			{
			//cout << hit_bar->GetEntriesFast()<<endl;
				PndHypGePoint *hitgam=(PndHypGePoint*)hit_bar->At(i);
			
				ActualTrackID=-1;
				PndMCTrack *mcgam = (PndMCTrack*)mc_bar->At(hitgam->GetTrackID());
		

			//rande= gRandom->Gaus(hitgam->GetEnergyLoss(),0.000003);
				if (hitgam->GetpdgCode()==2112 && mcgam->GetMotherID()==-1)
				{
					ActualTrackID= hitgam->GetTrackID();
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
					hCrystalHit->Fill(hitgam->GetDetectorID());
					//hNHits->Fill(180/TMath::Pi()*TMath::ACos(hitgam->GetZ()/TMath::Sqrt(hitgam->GetX()*hitgam->GetX()+hitgam->GetY()*hitgam->GetY()+hitgam->GetZ()* hitgam->GetZ())));		//Fill spectrum
				}
				if (mcgam->GetPdgCode()==2112 && mcgam->GetMotherID()==-1)
				{
					TVector3 NAllMom = mcgam->GetMomentum();
					hNAll->Fill(180/TMath::Pi()*NAllMom.Theta());
					hNMom->Fill(1./2.*NAllMom.Mag2()/0.939);
					
					cout << 1./2.*NAllMom.Mag2()/0.939<<endl;
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
					cout << 1./2.*NAllMom.Mag2()/0.939<<endl;
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
