void run_ana_sttcombi(TString fname="pid_sttcombi.root",int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	
	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...
	TH1F *invmass=new TH1F("invmass","invariant mass",400,0.0,5.0);
	
	// the candidates lists we need
	TCandList p1;
	TCandList p2;
	TCandList jpsi;
		
	if (nevts==0) nevts=evr.GetEntries();
	cout << "nevts " << nevts << "\n";
	int i=0,j=0, k=0, l=0;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{

//	    evr.FillList(p1,"All");
//	    evr.FillList(p2,"All");
	    evr.FillList(p1,"Charged");
	    evr.FillList(p2,"Charged");

	    cout << p1.GetLength() << " e+ candidates found" << "\n";
	    cout << p2.GetLength() << " e- candidates found" << "\n";

	    for (j=0;j<p1.GetLength();++j) { 
		p1[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
	    }
	    for (j=0;j<p2.GetLength();++j) { 
		p2[j].SetMass(TRho::Instance()->GetPDG()->GetParticle(11)->Mass());
	    }

	    jpsi.Combine(p1,p2);

	    cout << jpsi.GetLength() << " J/Psi candidates found" << "\n";

	    for (l=0;l<jpsi.GetLength();++l) { 
		invmass->Fill(jpsi[l].M());
	    }
	}
	
	// ... and plot it
	c1->cd(); invmass->Draw();
	
	// now take the time...
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();	
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);

}
