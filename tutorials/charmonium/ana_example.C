void ana_example(TString fname="lhepidmaker.root",int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
	
	gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
	
    TCanvas *c1=new TCanvas("c1","c1",600,600);
	
	// the PndEventReader takes care about file/event handling
	PndEventReader evr(fname);
	
	// to attach more files: evr.Add(fname2);evr.Add(fname3); ...
	
	TH1F *mom=new TH1F("mom","P(particle)",400,0.0,5.0);
	
	// the candidates lists we need
	TCandList all;
		
	if (nevts==0) nevts=evr.GetEntries();
	cout << "nevts " << nevts << "\n";
	int i=0,j;
	
	// *************
	// this is the loop through the events ... as simple as this...
	// ****************
	while (evr.GetEvent() && i++<nevts)
	{
	    cout << "before FillList, i " << i << "\n";
	    evr.FillList(all,"All");
	    cout << "after FillList, i " << i << "\n";
	    for (j=0;j<all.GetLength();++j) { 
		cout << "j " << j << "\n";
		mom->Fill(all[j].P()); 		
		cout << all[j].P() << "\n";
	    }
	}
	
	// ... and plot it
	//TCanvas *c1=new TCanvas("c1","c1",500,500);
	//c1->cd(); mom->Draw();
	c1->cd(); mom->Draw();
	
	// now take the time...
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}
