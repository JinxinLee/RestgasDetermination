void run_ana_eta_c(TString fname="./data/evt_pid_stt.root",int nevts=0)
{
	gStyle->SetOptFit(1011);

  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	TFile *out = TFile::Open("invmass_histo.root","RECREATE");

	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...

	TH1F *invmass=new TH1F("invmass","m(eta_c);E, GeV",100,2.5,3.5);
	TH1F *h_etac=new TH1F("etac","m(eta_c), #chi^{2}<30;E, GeV",100,2.5,3.5);
	TH1F *m1=new TH1F("m1","#phi: m(K+ K-)",100,1.020-0.15,1.020+0.15);
	TH1F *nc=new TH1F("nc","n charged",20,0,20);
	TH1F *h_chi2=new TH1F("h_chi2","chi2",100,0,100);
	
	TPidMassSelector *phiMassSel=new TPidMassSelector("phi",1.02,0.03);
	TPidPlusSelector *kplusSel=new TPidPlusSelector("kplus");
	TPidMinusSelector *kminusSel=new TPidMinusSelector("kminus");
	
	// the candidates lists we need
	TCandList p1, p2, p3, p4, phi1, phi2, etac;
	
	int n_reco=0;

	TLorentzVector ini(0,0,3.6772,4.7333);
		
	if (nevts==0) nevts=evr.GetEntries();
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
		for (j=0;j<phi1.GetLength();++j) m1->Fill(phi1[j].M()); 
		phi1.Select(phiMassSel);
		phi2.Select(phiMassSel);
		
		etac.Combine(phi1,phi2);

		int best_i=0;
		double best_chi2=100;
		for (l=0;l<etac.GetLength();++l) { 
			invmass->Fill(etac[l].M());
			Pnd4CFitter fitter(etac[l],ini);
			fitter.FitConserveMasses();
			double chi2=fitter.GetChi2();
			if(chi2<best_chi2)
			{
				best_chi2=chi2;
				best_i=l;
			}
			h_chi2->Fill(chi2);
	    }
	    
		if((best_chi2<30)&&(etac.GetLength()!=0))
		{
			h_etac->Fill(etac[best_i].M());
			if ((etac[best_i].M()>2.9)&&(etac[best_i].M()<3.06))
				n_reco++;
		}

	}
	
	std::cout<<"Number of reconstructed eta_c = "<<n_reco<<std::endl;
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	c1->Divide(2,2);

	c1->cd(1);
	nc->Draw();
	c1->cd(2);
	h_chi2->Draw();
	c1->cd(3);
	m1->Draw();
	
	TCanvas *c2=new TCanvas("c2","c2",600,600);
	c2->Divide(2,1);
	c2->cd(1);
	invmass->Draw();
	c2->cd(2);
	h_etac->Draw();
	
	TF1 *f1 = new TF1("f1","gaus",2.9,3.06);
	h_etac->Fit("gaus");
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
