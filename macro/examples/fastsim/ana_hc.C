int ana_hc(TString fname,int nevts=0)
{
  	TStopwatch timer;
  	timer.Start();
  
	gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");basiclibs();
	gSystem->Load("libRho");
	
	TCanvas *c1=new TCanvas("c1","c1",600,600);
	c1->Divide(2,2);
	
	TH1F *mpi0=new TH1F("mpi0","m(pi0)",100,0.0,0.3);
	TH1F *meta=new TH1F("meta","m(eta)",100,0.3,0.7);
	TH1F *metac=new TH1F("metac","m(eta_c)",200,0,3.5);
	TH1F *mhc=new TH1F("mhc","m(h_c)",200,0,3.8);
	
	mpi0->SetMinimum(0);
	meta->SetMinimum(0);
	
	PndEventReader evr(fname);
	
	if (nevts==0) nevts=evr.GetEntries();
	int i=0,j;
	
	TCandList neut,gam, pi0, eta, etac, hc;
	
	TPidMassSelector *etaMassSel=new TPidMassSelector("eta",0.554,0.03);
	TPidMassSelector *pi0MassSel=new TPidMassSelector("pi0",0.135,0.03);
	TPidEnergySelector *gamSel=new TPidEnergySelector("en",5,9.9);
	
	while (evr.GetEvent() && i++<nevts)
	{
		if (0==i%100) cout <<"evt "<<i<<endl;
		
		evr.FillList(neut,"Neutral");
		gam.Cleanup();
		
		for (j=0;j<neut.GetLength();j++) 
			if (neut[j]->GetMicroCandidate()->GetEmcNumberOfCrystals()>=3  // this is full sim
			    ||  neut[j]->GetMicroCandidate()->GetEmcNumberOfCrystals()==0 )  //this is fast sim
				gam.Add(neut[j]);
				
		pi0.Combine(gam,gam);
		for (j=0;j<pi0.GetLength();++j) mpi0->Fill(pi0[j].M()); 
		pi0.Select(pi0MassSel);
		
		eta.Combine(gam,gam);
		for (j=0;j<eta.GetLength();++j) meta->Fill(eta[j].M()); 
		eta.Select(etaMassSel);
		
		etac.Combine(eta, pi0, pi0);
		for (j=0;j<etac.GetLength();++j) metac->Fill(etac[j].M()); 
		
		hc.Combine(etac,gam);
		for (j=0;j<hc.GetLength();++j) mhc->Fill(hc[j].M()); 
		
	}
	
	c1->cd(1); mpi0->Draw();
	c1->cd(2); meta->Draw();
	c1->cd(3); metac->Draw();
	c1->cd(4); mhc->Draw();
	c1->cd();
	
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
	
  return 0;
}
