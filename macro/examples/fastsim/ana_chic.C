int ana_chic(TString fsig, TString fbkg,int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
  
    gSystem->Load("libEGPythia6");  // needed for TDatabasePDG
	gSystem->Load("libRho");
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	c1->Divide(2,2);
	
	TH1F *m1=new TH1F("m1","m1",100,0.547-0.3,0.547+0.3);
	TH1F *m2=new TH1F("m2","m2",100,3.096-0.35,3.096+0.35);
	TH1F *m3=new TH1F("m3","m3",100,3.415-0.35,3.415+0.35);
	
	TH1F *nn=new TH1F("nn","n neutrals",20,0,20);
	TH1F *nc=new TH1F("nc","n charged",20,0,20);
	
	PndEventReader evr(fsig);
	evr.Add(fbkg);
	
	if (nevts==0) nevts=evr.GetEntries();
	int i=0,j;
	
	TCandList gam, pip, pim, eta, jpsi, chi_c;
	
	TPidMassSelector *etaMassSel=new TPidMassSelector("eta",0.547,0.04);
	TPidMassSelector *jpsiMassSel=new TPidMassSelector("jpsi",3.096,0.04);
	
	PndEventInfo *evtinfo=0;
	
	while (evr.GetEvent() && i++<nevts)
	{
		if (0==i%100) cout <<"evt "<<i<<endl;
		
		evtinfo=evr.GetEventInfo();
		
		int nneut=evtinfo->GetNeutrals();
		int nchrg=evtinfo->GetCharged();
		
		nn->Fill(nneut);
		nc->Fill(nchrg);
		
		//continue;
			
		//if (nneut<4 || nneut>12 || nchrg<2 || nchrg>3) continue;
		
		evr.FillList(gam,"Neutral");
		evr.FillList(pip,"PionVeryLoosePlus");
		evr.FillList(pim,"PionVeryLooseMinus");
		
		eta.Combine(gam,gam);
		for (j=0;j<eta.GetLength();++j) m1->Fill(eta[j].M()); 
		eta.Select(etaMassSel);
		
		jpsi.Combine(gam, eta, pip, pim);
		for (j=0;j<jpsi.GetLength();++j) m2->Fill(jpsi[j].M()); 
		jpsi.Select(jpsiMassSel);
		
		chi_c.Combine(jpsi,gam);
		for (j=0;j<chi_c.GetLength();++j) m3->Fill(chi_c[j].M()); 
		
	}
	
	c1->cd(1); m1->Draw();
	c1->cd(2); m2->Draw();
	c1->cd(3); m3->Draw();
	c1->cd(4); nn->Draw();
	c1->cd();
	
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}
