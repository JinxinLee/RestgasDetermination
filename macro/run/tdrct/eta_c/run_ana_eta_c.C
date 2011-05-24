void run_ana_eta_c(TString fname="./data/evt_pid_stt.root",int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	TFile *out = TFile::Open("invmass_histo.root","RECREATE");

	//TCanvas *c1=new TCanvas("c1","c1",600,600);
	
	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...

	TH1F *invmass=new TH1F("invmass","invariant mass",100,2.5,3.5);
	TH1F *m1=new TH1F("m1","#phi: m(K+ K-)",100,1.020-0.15,1.020+0.15);
	TH1F *nc=new TH1F("nc","n charged",20,0,20);
	
	TPidMassSelector *phiMassSel=new TPidMassSelector("phi",1.02,0.04);
	
	// the candidates lists we need
	TCandList p1, p2, p3, p4, phi1, phi2, etac;

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
	    
		int nchrg=p1.GetLength();
		nc->Fill(nchrg);
		//cout << p1.GetLength() << " charged candidates found" << "\n";

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

	    //cout << etac.GetLength() << " eta_c candidates found" << "\n";
	    
		for (l=0;l<etac.GetLength();++l) { 
			invmass->Fill(etac[l].M());
	    }

	}
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	c1->Divide(2,2);

	c1->cd(1);
	invmass->Draw();
	c1->cd(2);
	m1->Draw();
	c1->cd(3);
	nc->Draw();
	

// 	out->cd();
// 	invmass->Write();
// 	out->Save();
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
