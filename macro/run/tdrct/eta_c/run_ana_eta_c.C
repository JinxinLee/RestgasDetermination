//void run_ana_tpccombi(TString fname="output.evt.fast.root",int nevts=0)
void run_ana_tpccombi(TString fname="evt_pid_stt.root",int nevts=0)
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
	
	// the candidates lists we need
	TCandList p1;
	TCandList p2;
	TCandList p3;
	TCandList p4;
	TCandList phi1;
	TCandList phi2;
	TCandList etac;

	TLorentzVector ini(0,0,3.6772,7.24015);
		
	if (nevts==0) nevts=evr.GetEntries();
	// cout << "nevts " << nevts << "\n";
	int i=0,j=0, k=0, l=0;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{

	    if (!((i+1)%1000)) cout << i << "\n";


	    evr.FillList(p1,"Charged");
	    evr.FillList(p2,"Charged");
		evr.FillList(p3,"Charged");
		evr.FillList(p4,"Charged");
	    
		cout << p1.GetLength() << " charged candidates found" << "\n";

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
		etac.Combine(phi1,phi2);

	    cout << etac.GetLength() << " eta_c candidates found" << "\n";
	    
		for (l=0;l<etac.GetLength();++l) { 
			invmass->Fill(etac[l].M());
	    }

	}
	
	//c1->cd(); 

	invmass->Draw();

	out->cd();
	invmass->Write();
	out->Save();
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
