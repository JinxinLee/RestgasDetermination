void psi2s(Bool_t kPID = kFALSE)
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  
  TString inRecoFile = "reco_tpccombi.root";
  TString inDigiFile = "digi_tpccombi.root";
  TString inSimFile = "points_tpccombi.root";
  
  TFile *inFile = TFile::Open(inRecoFile,"READ");
  
  TTree *tree=(TTree *) inFile->Get("cbmsim") ;
  //tree->AddFriend(inDigiFile);
  //tree->AddFriend(inSimFile);
  PndEmcMapper *emcMap=PndEmcMapper::Instance(2, inSimFile);
  
  TClonesArray* lhe_array=new TClonesArray("PndLhePidTrack");
  tree->SetBranchAddress("LhePidTrack", &lhe_array);
  
  PndLhePidTrack  *tr1;
  PndLhePidTrack  *tr2;
  PndLhePidTrack  *tr3;
  PndLhePidTrack  *tr4;
  
  TFile *out = TFile::Open("out.root","RECREATE");
  
  c1 = new TCanvas("c1","Psi(2S)",100,100,800,800);
  
  c1->Divide(2,2);
  
  TH1D* invmass = new TH1D("invmass","",100,.0,4.0);
  invmass->SetTitle("2-Particle Invariant Mass / GeV");
  invmass->GetXaxis()->SetTitle("2-Particle Invariant Mass / GeV");
  TH1D* invmass4 = new TH1D("invmass4","",100,2.5,4.5);
  invmass4->SetTitle("4-Particle Invariant Mass / GeV");
  TH1D* invpt4 = new TH1D("invpt4","",100,0,2);
  invpt4->SetTitle("4-Particle Pt / GeV");
  
  TH1D* invmass_zero = new TH1D("invmass_zero","",100,.0,4.0);
  invmass_zero->SetTitle("2-Particle Invariant Mass / GeV");
  invmass_zero->SetLineColor(2);
  TH1D* invmass4_zero = new TH1D("invmass4_zero","",100,2.5,4.5);
  invmass4_zero->SetTitle("4-Particle Invariant Mass / GeV"); 
  invmass4_zero->SetLineColor(2);
  TH1D* invpt4_zero = new TH1D("invpt4_zero","",100,0,2);
  invpt4_zero->SetTitle("4-Particle Pt / GeV");
  invpt4_zero->SetLineColor(2);
    
  for (Int_t j=0; j< tree->GetEntriesFast(); j++){
    tree->GetEntry(j);
    
    if (lhe_array->GetEntriesFast()<2) continue;
    
    cout << "processing event " << j << "\n";
    
    TLorentzVector track1 = NULL;
    TLorentzVector track2 = NULL;
    TLorentzVector track3 = NULL;
    TLorentzVector track4 = NULL;

    TLorentzVector track1_at_zero = NULL;
    TLorentzVector track2_at_zero = NULL;
    TLorentzVector track3_at_zero = NULL;
    TLorentzVector track4_at_zero = NULL;

    for (Int_t i1=0; i1<lhe_array->GetEntriesFast(); i1++) // Loop over e+ candidate
      { 
	tr1=  (PndLhePidTrack *)lhe_array->At(i1);
	if (tr1->GetCharge()!=1) continue;

	if (kPID)
	  {
	    if (tr1->GetMomentum().Mag()<0.01) continue;
	    if (tr1->GetEmcIndex()==-1) continue;
	    if ((tr1->GetEmcELoss()/tr1->GetMomentum().Mag()) < 0.85) continue;
	  }
	
	track1.SetXYZM(tr1->GetPx(), tr1->GetPy(), tr1->GetPz(), 0.000511);
	
	TVector3 vertex1(0., 0., 0.);
	TVector3 momentum1(0., 0., 0.);
	Float_t phic1 = tr1->ExtrapolateToZ(&momentum1, &vertex1, 0.);
	track1_at_zero.SetXYZM(momentum1.x(), momentum1.y(), momentum1.z(), 0.000511);
	
	for (Int_t i2=0; i2<lhe_array->GetEntriesFast(); i2++) // Loop over e- candidate
	  {
	    tr2=  (PndLhePidTrack *)lhe_array->At(i2);
	    if (tr2->GetCharge()!=-1) continue;
	    
	    if (kPID)
	      {
		if (tr2->GetMomentum().Mag()<0.01) continue;
		if (tr2->GetEmcIndex()==-1) continue;
		if ((tr2->GetEmcELoss()/tr2->GetMomentum().Mag()) < 0.85) continue;
	      }
	    
	    track2.SetXYZM(tr2->GetPx(), tr2->GetPy(), tr2->GetPz(), 0.000511);
	    
	    TVector3 vertex2(0., 0., 0.);
	    TVector3 momentum2(0., 0., 0.);
	    Float_t phic2 = tr2->ExtrapolateToZ(&momentum2, &vertex2, 0.);
	    track2_at_zero.SetXYZM(momentum2.x(), momentum2.y(), momentum2.z(), 0.000511);
	    
	    TLorentzVector sum = track1 + track2;
	    TLorentzVector sum_at_zero = track1_at_zero + track2_at_zero;
	    
	    invmass->Fill(sum.M());    
	    invmass_zero->Fill(sum_at_zero.M());   
	    
	    if ( ( sum.M() > 2.5 ) && ( sum.M() < 3.5 ) )  // J/Psi Selection Loop
	      {
		for (Int_t i3=0; i3<lhe_array->GetEntriesFast(); i3++) // Loop over pi+ candidate
		  {
		    if (i3==i1) continue;
		    tr3=  (PndLhePidTrack *)lhe_array->At(i3);  
		    if ( tr3->GetCharge() != 1 ) continue;
 
		    if (kPID)
		      {
			if (tr3->GetMomentum().Mag()<0.01) continue;
			if (tr3->GetEmcIndex()==-1) continue;
			if ((tr3->GetEmcELoss()/tr3->GetMomentum().Mag()) > 0.85) continue;
		      }
		    
		    track3.SetXYZM(tr3->GetPx(), tr3->GetPy(), tr3->GetPz(), 0.13957018);
		    
		    TVector3 vertex3(0., 0., 0.);
		    TVector3 momentum3(0., 0., 0.);
		    Float_t phic3 = tr3->ExtrapolateToZ(&momentum3, &vertex3, 0.);
		    track3_at_zero.SetXYZM(momentum3.x(), momentum3.y(), momentum3.z(), 0.13957018);
		    
		    for (Int_t i4=0; i4<lhe_array->GetEntriesFast(); i4++) // Loop over pi- candidate
		      {
			if (i4==i2) continue;
			tr4=  (PndLhePidTrack *)lhe_array->At(i4);
			if ( tr4->GetCharge() != -1 ) continue;
			
			if (kPID)
			  {
			    if (tr4->GetMomentum().Mag()<0.01) continue;
			    if (tr4->GetEmcIndex()==-1) continue;
			    if ((tr4->GetEmcELoss()/tr4->GetMomentum().Mag()) > 0.85) continue;
			  }
			
			track4.SetXYZM(tr4->GetPx(), tr4->GetPy(), tr4->GetPz(), 0.13957018);
			
			TVector3 vertex4(0., 0., 0.);
			TVector3 momentum4(0., 0., 0.);
			Float_t phic4 = tr4->ExtrapolateToZ(&momentum4, &vertex4, 0.);
			track4_at_zero.SetXYZM(momentum4.x(), momentum4.y(), momentum4.z(), 0.13957018);
			 
			TLorentzVector sum4 = track1+track2+track3+track4;
			TLorentzVector sum4_at_zero = track1_at_zero+track2_at_zero+track3_at_zero+track4_at_zero;
			
			invmass4->Fill(sum4.M());
			invmass4_zero->Fill(sum4_at_zero.M());	
			invpt4->Fill(sum4.Pt());
			invpt4_zero->Fill(sum4_at_zero.Pt());
			
		      } // end of pi- loop
		  } // end of pi- loop
		    
	      } // end of  J/Psi loop
	  } // end of e- loop
      } // end of e+ loop

  } // end of event loop
  
  
  c1->cd(1);
  invmass->Draw();
  invmass_zero->Draw("same");
  
  c1->cd(2);
  invmass4_zero->Draw();
  invmass4->Draw("same");
  
  c1->cd(3);
  invpt4_zero->Draw();
  invpt4->Draw("same");
  
  c1->cd();

  out->cd();
  
  invmass->Write();
  invmass4->Write();
  invpt4->Write();
  invmass_zero->Write();
  invmass4_zero->Write();
  invpt4_zero->Write();
  out->Save();
  
  
}	
