void test_check(Int_t nEntries = 0)
{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  TString inPidFile  = "pid_sttcombi.root"; 
  TString inSimFile = "points_sttcombi.root";
  
  TFile *inFile = TFile::Open(inPidFile,"READ");
  
  TTree *tree=(TTree *) inFile->Get("cbmsim") ;
  tree->AddFriend("cbmsim",inSimFile);
    
  TClonesArray* cand_array=new TClonesArray("PndPidCandidate");
  tree->SetBranchAddress("PidChargedCand", &cand_array);

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  tree->SetBranchAddress("MCTrack", &mc_array);
  
  TClonesArray* mdt_array=new TClonesArray("PndPidProbability");
  tree->SetBranchAddress("PidAlgoMdtHardCuts", &mdt_array);
  
  TFile *out = TFile::Open("out_test2.root","RECREATE");
  //TNtuple *nt = new TNtuple("nt","nt","evt:mc_p:mc_theta:mc_phi:mc_pid:p:theta:phi:mult");
  TNtuple *ee = new TNtuple("ee","ee","p1:theta1:phi1:p2:theta2:phi2:pid1:emc1:emc_ene1:muo1:muo_mod1:muo_iron1:muo_mom1:muo_prob1:pid2:emc2:emc_ene2:muo2:muo_mod2:muo_iron2:muo_mom2:muo_prob2:mass_ee:mass_mu:mass_pi"); 
  TNtuple *nt4 = new TNtuple("nt4","nt4","p1:theta1:phi1:p2:theta2:phi2:pid1:emc1:emc_ene1:muo1:muo_mod1:muo_iron1:muo_mom1:muo_prob1:pid2:emc2:emc_ene2:muo2:muo_mod2:muo_iron2:muo_mom2:muo_prob2:mass_ee:mass_mu:mass_pi:p3:theta3:phi3:p4:theta4:phi4:pid3:emc3:emc_ene3:muo3:muo_mod3:muo_iron3:muo_mom3:muo_prob3:pid4:emc4:emc_ene4:muo4:muo_mod4:muo_iron4:muo_mom4:muo_prob4:mass2_ee:mass2_mu:mass2_pi:X_e:X_mu");
  //TNtuple *mm = new TNtuple("mm","mm","p3:theta3:phi1:p2:theta2:phi2:mass");
  //TNtuple *pp = new TNtuple("pp","pp","p1:theta1:phi1:p2:theta2:phi2:mass:miss");
  TH1F *m1=new TH1F("m1","m1",100,0,4);
  TH1F *m2=new TH1F("m2","m2",100,0,4);
  TH1F *m3=new TH1F("m3","m3",100,0,4);
  TH1F *m4=new TH1F("m4","m4",100,0,6);
  TH1F *m5=new TH1F("m5","m5",100,0,6);
  TH1F *mc1=new TH1F("mc1","mc1",100,0,4);
  TH1F *mc2=new TH1F("mc2","mc2",100,0,4);
  TH1F *mc3=new TH1F("mc3","mc3",100,0,4);
  TH1F *mc4=new TH1F("mc4","mc4",100,0,6);
  TH1F *mc5=new TH1F("mc5","mc5",100,0,6);
  
  TH2F *ps1 = new TH2F("ps1","ps1",100,0,7,100,0,180);
  TH2F *ps2 = new TH2F("ps2","ps2",100,0,7,100,0,1.5);
  TH2F *ps3 = new TH2F("ps3","ps3",100,0,7,100,0,100); 
  TH2F *ps4 = new TH2F("ps4","ps4",100,0,7,100,0,100);
  TNtuple *hit = new TNtuple("hit","hit","q:p:theta:phi:pid:emc:emc_ene:muo:muo_mod:muo_iron:muo_mom:muo_prob");
  
  TLorentzVector ini(0., 0., 6.98840096, 7.98937619);
  if (nEntries==0) nEntries =  tree->GetEntriesFast();
  for (Int_t j=0; j< nEntries; j++){
    tree->GetEntry(j);
    //if (cand_array->GetEntriesFast()==0) continue;
    cout << "processing event " << j << "\n";
    
    TLorentzVector ep[15], em[15], pip[15], pim[15], mup[15], mum[15], jpsi_e, jpsi_mu;
    TLorentzVector mep[15], mem[15], mpip[15], mpim[15], mmup[15], mmum[15], mjpsi_e, mjpsi_mu;
    Int_t ep_count = 0, em_count = 0, pip_count = 0, pim_count = 0, mup_count = 0, mum_count = 0;
    Int_t ep_mcount = 0, em_mcount = 0, pip_mcount = 0, pim_mcount = 0, mup_mcount = 0, mum_mcount = 0;
    for (Int_t pp=0; pp<cand_array->GetEntriesFast(); pp++)
      {
	PndPidCandidate *pidCand1 = (PndPidCandidate*)cand_array->At(pp);
	if ((pidCand1->GetCharge()==1) && (pidCand1->GetMomentum().Mag()<8) )
	  {
	    PndPidProbability *mdtProb1 = (PndPidProbability*)mdt_array->At(pp);
	    if (pidCand1->GetMomentum().Mag()>8) continue;
	    Int_t mc_pid1 = 0;
	    if (pidCand1->GetMcIndex()>-1)
	      {
		PndMCTrack* mcTrack = (PndMCTrack*)mc_array->At(pidCand1->GetMcIndex());
		if (mcTrack->GetMotherID()==-1) mc_pid1 = mcTrack->GetPdgCode();
	      }
	    Float_t hit_ntuple1[] = {pidCand1->GetCharge(), pidCand1->GetMomentum().Mag(),  pidCand1->GetMomentum().Theta()*TMath::RadToDeg(), pidCand1->GetMomentum().Phi()*TMath::RadToDeg(), mc_pid1,
				     pidCand1->GetEmcIndex(),pidCand1->GetEmcRawEnergy(),
				     pidCand1->GetMuoIndex(), pidCand1->GetMuoModule(), pidCand1->GetMuoIron(), pidCand1->GetMuoMomentumIn(), mdtProb1->GetMuonPidProb()
	    };
	    hit->Fill(hit_ntuple1);
	    for (Int_t mm=0; mm<cand_array->GetEntriesFast(); mm++)
	      {
		PndPidCandidate *pidCand2 = (PndPidCandidate*)cand_array->At(mm);
		if ( (pidCand2->GetCharge()==-1) && (pidCand2->GetMomentum().Mag()<8) )
		  {
		    PndPidProbability *mdtProb2 = (PndPidProbability*)mdt_array->At(mm);
		    if (pidCand2->GetMomentum().Mag()>8) continue;
		    Int_t mc_pid2 = 0;
		    if (pidCand2->GetMcIndex()>-1)
		      {
			PndMCTrack* mcTrack = (PndMCTrack*)mc_array->At(pidCand2->GetMcIndex());
			if (mcTrack->GetMotherID()==-1) mc_pid2 = mcTrack->GetPdgCode();
		      }
		    Float_t hit_ntuple2[] = {pidCand2->GetCharge(), pidCand2->GetMomentum().Mag(),  pidCand2->GetMomentum().Theta()*TMath::RadToDeg(), pidCand2->GetMomentum().Phi()*TMath::RadToDeg(), mc_pid2,
					     pidCand2->GetEmcIndex(),pidCand2->GetEmcRawEnergy(),
					     pidCand2->GetMuoIndex(), pidCand2->GetMuoModule(), pidCand2->GetMuoIron(), pidCand2->GetMuoMomentumIn(), mdtProb2->GetMuonPidProb()
		    };
		    hit->Fill(hit_ntuple2);
		    
		    TLorentzVector lorep(pidCand1->GetMomentum(),sqrt(pidCand1->GetMomentum()*pidCand1->GetMomentum()+0.0005*0.0005));
		    TLorentzVector lorem(pidCand2->GetMomentum(),sqrt(pidCand2->GetMomentum()*pidCand2->GetMomentum()+0.0005*0.0005));
		    TLorentzVector lorpip(pidCand1->GetMomentum(),sqrt(pidCand1->GetMomentum()*pidCand1->GetMomentum()+0.140*0.140));
		    TLorentzVector lorpim(pidCand2->GetMomentum(),sqrt(pidCand2->GetMomentum()*pidCand2->GetMomentum()+0.140*0.140));
		    TLorentzVector lormup(pidCand1->GetMomentum(),sqrt(pidCand1->GetMomentum()*pidCand1->GetMomentum()+0.113*0.113));
		    TLorentzVector lormum(pidCand2->GetMomentum(),sqrt(pidCand2->GetMomentum()*pidCand2->GetMomentum()+0.113*0.113));
		    Float_t ee_ntuple[] = {
		      lorep.P(), lorep.Theta()*TMath::RadToDeg(), lorep.Phi()*TMath::RadToDeg(),
		      lorem.P(), lorem.Theta()*TMath::RadToDeg(), lorem.Phi()*TMath::RadToDeg(),
		      mc_pid1, pidCand1->GetEmcIndex(),pidCand1->GetEmcRawEnergy(), 
		      pidCand1->GetMuoIndex(), pidCand1->GetMuoModule(), pidCand1->GetMuoIron(), pidCand1->GetMuoMomentumIn(), mdtProb1->GetMuonPidProb(),
		      mc_pid2, pidCand2->GetEmcIndex(),pidCand2->GetEmcRawEnergy(), 
		      pidCand2->GetMuoIndex(), pidCand2->GetMuoModule(), pidCand2->GetMuoIron(), pidCand2->GetMuoMomentumIn(), mdtProb2->GetMuonPidProb(),
		      (lorep+lorem).M(), (lormup+lormum).M(), (ini-(lorpip+lorpim)).M()  
		    };
		    ee->Fill(ee_ntuple);

		    for (Int_t pp2=0; pp2<cand_array->GetEntriesFast(); pp2++)
		      {
			PndPidCandidate *pidCand3 = (PndPidCandidate*)cand_array->At(pp2);
			if ((pidCand3->GetCharge()==1) && (pidCand3->GetMomentum().Mag()<8) )
			  {
			    PndPidProbability *mdtProb3 = (PndPidProbability*)mdt_array->At(pp2);
			    if (pidCand3->GetMomentum().Mag()>8) continue;
			    Int_t mc_pid3 = 0;
			    if (pidCand3->GetMcIndex()>-1)
			      {
				PndMCTrack* mcTrack = (PndMCTrack*)mc_array->At(pidCand3->GetMcIndex());
				if (mcTrack->GetMotherID()==-1) mc_pid3 = mcTrack->GetPdgCode();
			      }
	    
			    for (Int_t mm2=0; mm2<cand_array->GetEntriesFast(); mm2++)
			      {
				PndPidCandidate *pidCand4 = (PndPidCandidate*)cand_array->At(mm2);
				if ( (pidCand4->GetCharge()==-1) && (pidCand4->GetMomentum().Mag()<8) )
				  {
				    PndPidProbability *mdtProb4 = (PndPidProbability*)mdt_array->At(mm2);
				    if (pidCand4->GetMomentum().Mag()>8) continue;
				    Int_t mc_pid4 = 0;
				    if (pidCand4->GetMcIndex()>-1)
				      {
					PndMCTrack* mcTrack = (PndMCTrack*)mc_array->At(pidCand4->GetMcIndex());
					if (mcTrack->GetMotherID()==-1) mc_pid4 = mcTrack->GetPdgCode();
				      }
		   
		    
				    TLorentzVector lor2ep(pidCand3->GetMomentum(),sqrt(pidCand3->GetMomentum()*pidCand3->GetMomentum()+0.0005*0.0005));
				    TLorentzVector lor2em(pidCand4->GetMomentum(),sqrt(pidCand4->GetMomentum()*pidCand4->GetMomentum()+0.0005*0.0005));
				    TLorentzVector lor2pip(pidCand3->GetMomentum(),sqrt(pidCand3->GetMomentum()*pidCand3->GetMomentum()+0.140*0.140));
				    TLorentzVector lor2pim(pidCand4->GetMomentum(),sqrt(pidCand4->GetMomentum()*pidCand4->GetMomentum()+0.140*0.140));
				    TLorentzVector lor2mup(pidCand3->GetMomentum(),sqrt(pidCand3->GetMomentum()*pidCand3->GetMomentum()+0.113*0.113));
				    TLorentzVector lor2mum(pidCand4->GetMomentum(),sqrt(pidCand4->GetMomentum()*pidCand4->GetMomentum()+0.113*0.113));
				    Float_t p4_ntuple[] = {
				      lorep.P(), lorep.Theta()*TMath::RadToDeg(), lorep.Phi()*TMath::RadToDeg(),
				      lorem.P(), lorem.Theta()*TMath::RadToDeg(), lorem.Phi()*TMath::RadToDeg(),
				      mc_pid1, pidCand1->GetEmcIndex(),pidCand1->GetEmcRawEnergy(), 
				      pidCand1->GetMuoIndex(), pidCand1->GetMuoModule(), pidCand1->GetMuoIron(), pidCand1->GetMuoMomentumIn(), mdtProb1->GetMuonPidProb(),
				      mc_pid2, pidCand2->GetEmcIndex(),pidCand2->GetEmcRawEnergy(), 
				      pidCand2->GetMuoIndex(), pidCand2->GetMuoModule(), pidCand2->GetMuoIron(), pidCand2->GetMuoMomentumIn(), mdtProb2->GetMuonPidProb(),
				      (lorep+lorem).M(), (lormup+lormum).M(), (ini-(lorpip+lorpim)).M() , 
				      lor2ep.P(), lor2ep.Theta()*TMath::RadToDeg(), lor2ep.Phi()*TMath::RadToDeg(),
				      lor2em.P(), lor2em.Theta()*TMath::RadToDeg(), lor2em.Phi()*TMath::RadToDeg(),
				      mc_pid3, pidCand3->GetEmcIndex(),pidCand3->GetEmcRawEnergy(), 
				      pidCand3->GetMuoIndex(), pidCand3->GetMuoModule(), pidCand3->GetMuoIron(), pidCand3->GetMuoMomentumIn(), mdtProb3->GetMuonPidProb(),
				      mc_pid4, pidCand4->GetEmcIndex(),pidCand4->GetEmcRawEnergy(), 
				      pidCand4->GetMuoIndex(), pidCand4->GetMuoModule(), pidCand4->GetMuoIron(), pidCand4->GetMuoMomentumIn(), mdtProb4->GetMuonPidProb(),
				      (lor2ep+lor2em).M(), (lor2mup+lor2mum).M(), (ini-(lor2pip+lor2pim)).M(),(lorpip+lorpim+lor2ep+lor2em).M(),(lorpip+lorpim+lor2mup+lor2mum).M()  
				    };
				    nt4->Fill(p4_ntuple);
				  }
			      } // end of mm2
			  }
		      } // end of pp2
		  }
	      } // end of mm
	  }
      } // end of pp
    

      
  
    
  } // end of event loop
  
  
  out->cd();
  ps1->Write();
  ps2->Write();
  ps3->Write();
  ps4->Write();
  m1->Write();
  m2->Write();
  m3->Write();
  m4->Write();
  m5->Write();
  mc1->Write();
  mc2->Write();
  mc3->Write();
  mc4->Write();
  mc5->Write();
  hit->Write();
  ee->Write();
  nt4->Write();
  out->Save();
  
}	
