int ana_example(TString fname,int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
	
	gSystem->Load("libEGPythia6");  // needed for TDatabasePDG
	gSystem->Load("libRho");
	
	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...
	
	TH1F *metac=new TH1F("metac","m(eta_c)",100,2.6,3.3);
	
	// the candidates lists we need
	TCandList gam, pi0, eta, etac;
	
	// some mass selectors
	TPidMassSelector *etaMassSel=new TPidMassSelector("eta",0.534,0.03);
	TPidMassSelector *pi0MassSel=new TPidMassSelector("pi0",0.130,0.03);
	
	if (nevts==0) nevts=evr.GetEntries();
	int i=0,j;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{
	    // ask the event reader to fill the gamma candidates lists
		evr.FillList(gam,"Neutral");
		
		// do the combinatorics and selection
		pi0.Combine(gam,gam);
		pi0.Select(pi0MassSel);
		
		eta.Combine(gam,gam);
		eta.Select(etaMassSel);
		
		// Combine now works with up to 5 lists
		// double counting and overlap when using the same list
		// more the once is handled properly
		etac.Combine(eta, pi0, pi0); 
		
		// fill a histogram
		for (j=0;j<etac.GetLength();++j) metac->Fill(etac[j].M()); 		
	}
	
	// ... and plot it
	TCanvas *c1=new TCanvas("c1","c1",500,500);
	c1->cd(); metac->Draw();
	
	// now take the time...
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}
