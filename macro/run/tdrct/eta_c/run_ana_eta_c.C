void run_ana_eta_c(TString fname="./data2/evt_pid_stt_101.root",int nevts=0)
{
	gStyle->SetOptFit(1011);

  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	TFile *out = TFile::Open("etac_histo.root","RECREATE");

	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...
// 	evr.Add("./data2/evt_pid_stt_102.root");
// 	evr.Add("./data2/evt_pid_stt_103.root");
// 	evr.Add("./data2/evt_pid_stt_104.root");
// 	evr.Add("./data2/evt_pid_stt_105.root");
// 	evr.Add("./data2/evt_pid_stt_106.root");
// 	evr.Add("./data2/evt_pid_stt_107.root");
// 	evr.Add("./data2/evt_pid_stt_108.root");
// 	evr.Add("./data2/evt_pid_stt_109.root");
// 	evr.Add("./data2/evt_pid_stt_110.root");


	TH1F *h_etac_nocut=new TH1F("h_etac_nocut","m(eta_c), (no cuts);E, GeV",100,2.5,3.5);
	TH1F *h_etac_pid=new TH1F("h_etac_pid","m(eta_c), (MC PID);E, GeV",100,2.5,3.5);
	TH1F *h_etac_phimass=new TH1F("h_etac_phimass","m(eta_c), (cut on #phi mass);E, GeV",100,2.5,3.5);
	TH1F *h_etac_vtx=new TH1F("h_etac_vtx","m(eta_c), Vertex fit",100,2.5,3.5);
	TH1F *h_etac_4c=new TH1F("h_etac_4c","m(eta_c), 4C-fit",100,2.5,3.5);
	
	TH1F *h_mphi_nocuts=new TH1F("h_mphi","#phi: m(K+ K-) (no cuts)",100,1.020-0.15,1.020+0.15);
	TH1F *h_mphi_pid=new TH1F("h_mphi_pid","#phi: m(K+ K-) (MC PID)",100,1.020-0.15,1.020+0.15);
	TH1F *h_mphi_vtx=new TH1F("h_mphi_vtx","#phi: m(K+ K-) (Vertex fit)",100,1.020-0.15,1.020+0.15);
	TH1F *h_mphi_4c=new TH1F("h_mphi_4c","#phi: m(K+ K-) (4C-fit)",100,1.020-0.15,1.020+0.15);
	
	TH1F *nc=new TH1F("nc","n charged",20,0,20);
	
	TH1F *h_chi2_4c=new TH1F("h_chi2_4c","chi2 4C-fit",100,0,100);
	TH1F *h_chi2_vtx=new TH1F("h_chi2_vtx","chi2 vertex",100,0,100);
	TH2F *hvpos = new TH2F("hvpos","(x,y) projection of fitted decay vertex",100,-5,5,100,-5,5);

	
	TPidMassSelector *phiMassSel=new TPidMassSelector("phi",1.02,0.03);
	TPidPlusSelector *kplusSel=new TPidPlusSelector("kplus");
	TPidMinusSelector *kminusSel=new TPidMinusSelector("kminus");
	
	// the candidates lists we need
	TCandList p1, p2, p3, p4, phi1, phi2, etac, etac_nocut;
	
	int n_reco=0;
	// Number of events in file and number of reconstructed eta_c to store in root file
	TVectorD n_events(1);
	TVectorD n_etac(1);

	TLorentzVector ini(0,0,3.6772,4.7333);
		
	if (nevts==0) nevts=evr.GetEntries();
	n_events[0]=nevts;
	// cout << "nevts " << nevts << "\n";
	int i=0,j=0, k=0, l=0;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{

	    if (!((i+1)%100)) cout<<"evt " << i << "\n";
	    
	    evr.FillList(p1,"Charged");
	    evr.FillList(p2,"Charged");
		evr.FillList(p3,"Charged");
		evr.FillList(p4,"Charged");
	    
		p1.Select(kplusSel);
		p2.Select(kminusSel);
		p3.Select(kplusSel);
		p4.Select(kminusSel);
		
		int nchrg=p1.GetLength();
		nc->Fill(nchrg);

	    for (j=0;j<p1.GetLength();++j) { 
			p1[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }
	    for (j=0;j<p2.GetLength();++j) { 
			p2[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }
	    for (j=0;j<p3.GetLength();++j) { 
			p3[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }
	    for (j=0;j<p4.GetLength();++j) { 
			p4[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(321)->Mass());
	    }


	    phi1.Combine(p1,p2);
		phi2.Combine(p3,p4);
		for (j=0;j<phi1.GetLength();++j) h_mphi_nocuts->Fill(phi1[j].M()); 
		etac_nocut.Combine(phi1,phi2);

		for (l=0;l<etac.GetLength();++l) { 
			h_etac_nocut->Fill(etac_nocut[l].M());
		}
		
		// MC PID
		for (l=0;l<etac.GetLength();++l) { 
		}

		phi1.Select(phiMassSel);
		phi2.Select(phiMassSel);
		etac.Combine(phi1,phi2);
		for (l=0;l<etac.GetLength();++l) { 
			h_etac_phimass->Fill(etac[l].M());
		}
		
		
		////////////// 4C-fit ///////////////
		int best_i=0;
		double best_chi2=100;
		for (l=0;l<etac.GetLength();++l) { 
			Pnd4CFitter fitter(etac[l],ini);
			fitter.FitConserveMasses();
			double chi2=fitter.GetChi2();
			if(chi2<best_chi2)
			{
				best_chi2=chi2;
				best_i=l;
			}
			h_chi2_4c->Fill(chi2);
	    }
	    
		if((best_chi2<30)&&(etac.GetLength()!=0))
		{
			h_etac->Fill(etac[best_i].M());
			if ((etac[best_i].M()>2.9)&&(etac[best_i].M()<3.06))
				n_reco++;
		}
		
		////////////// Vertex fit /////////////
		
	    for (j=0;j<etac.GetLength();++j) 
		{
			PndKinVtxFitter vtxfitter(etac[j]);        // instantiate a vertex fitter
			vtxfitter.Fit();                          // do the vertex fit
      
			TCandidate *etacfit=vtxfitter.FittedCand(etac[j]);  // request the fitted EtaC candidate
			TVector3 etacVtx=etacfit->Pos();                    // and the decay vertex position
            double chi2_vtx=vtxfitter.GlobalChi2();
			h_chi2_vtx->Fill(chi2_vtx);
			// plot mass and vtx x,y projection after fit
			h_etac_vtx->Fill(etacfit->M());
			hvpos->Fill(etacVtx.X(),etacVtx.Y());   
		}


	}
	
	std::cout<<"Number of reconstructed eta_c = "<<n_reco<<std::endl;
	n_etac[0]=n_reco;
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	c1->Divide(2,2);

	c1->cd(1);
	nc->Draw();
	c1->cd(2);
	h_chi2_4c->Draw();
	c1->cd(3);
	m1->Draw();
	c1->cd(4);
	h_chi2_vtx->Draw();
	
	TCanvas *c2=new TCanvas("c2","c2",600,600);
	c2->Divide(3,1);
	c2->cd(1);
	invmass->Draw();
	c2->cd(2);
	h_etac->Draw();
	c2->cd(3);
	h_etac_vtx->Draw();
	
	TF1 *f1 = new TF1("f1","gaus",2.9,3.06);
	h_etac->Fit("gaus");
	double sigma=f1->GetParameter(2);
	std::cout<<"sigma="<<sigma<<std::endl;

	TCanvas *c3=new TCanvas("c3","c3",600,600);
	hvpos->Draw();
		
// 	out->cd();
//	n_etac->Write("n_etac"); //TVectorD *n_etac = (TVectorD*)f.get("n_etac");
//  n_events->Write("n_events");
// 	h_etac_nocut->Write();
// 	h_etac_pid->Write();
// 	h_etac_phimass->Write();
// 	h_etac_vtx->Write();
// 	h_etac_4c->Write();
// 
// 	h_mphi_nocuts->Write();
// 	h_mphi_pid->Write();
// 	h_mphi_vtx->Write();
// 	h_mphi_4c->Write();
// 
// 	nc->Write();
// 
// 	h_chi2_4c->Write();
// 	h_chi2_vtx->Write();
// 	hvpos->Write();
// 
// 	out->Save();

	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
