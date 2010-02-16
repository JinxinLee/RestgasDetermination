class TCandidate;

int printRecursive(TCandidate *tc, int level=0);

void ana_dsdsj(TString fname="dsds_10k.evt.root",int nevts=0)
{
	gROOT->Reset();
  	TStopwatch timer;
  	timer.Start();

  	gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");rootlogon();
  
  	TCanvas *c1=new TCanvas("c1","c1",900,600);
  	c1->Divide(3,2);
  
	// **** create and setup some histos for QA plots
	//
	TH1F *phimass = new TH1F("phimass","phi cands",100,0.95,1.1);
	TH1F *pi0mass = new TH1F("pi0mass","pi0 cands",100,0.135-0.03,0.135+0.03);
	TH1F *dsmass  = new TH1F("dsmass","Ds cands",100,1.968-0.1,1.968+0.1); 
	TH1F *ds0mass = new TH1F("ds0mass","Ds0 cands",100,2.317-0.1,2.317+0.1);
	TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,4.306-0.1,4.306+0.1);
	
	TH1F *nmult=new TH1F("nmult","# neutrals",15,0,15);
	
	phimass->SetMinimum(0);
	pi0mass->SetMinimum(0);
	dsmass->SetMinimum(0);
	ds0mass->SetMinimum(0);
	ppmass->SetMinimum(0);
	
	// **** PndEventReader provides access to the data
	//
	PndEventReader evr(fname);
	
	if (nevts==0) nevts=evr.GetEntries();
	
	// **** create all the particle lists we'll need for rebuilding the decay tree
	//
	TCandList kp, km, pip, pim, gam;
	TCandList phi, pi0, dsp, dsm, ds0p, ds0m, pp;
	
	// **** mass selectors for the resonances/composites
	//
	TPidMassSelector *phiMSel       = new TPidMassSelector("phiSelector" , 1.0195 , 0.02);
	TPidMassSelector *pi0MSel       = new TPidMassSelector("pi0Selector" , 0.135  , 0.02);
	TPidMassSelector *dsMSel        = new TPidMassSelector("dsSelector"  , 1.9685 , 0.04);
	
	
	// **** loop over all _events_
	//
	
	int i=0,j=0;
	
	while (evr.GetEvent() && i++<nevts)
	{
		if (0==i%100) cout <<"evt "<<i<<endl;
		
		evr.FillList(kp,"KaonVeryLoosePlus");
		evr.FillList(km,"KaonVeryLooseMinus");
		evr.FillList(pip,"PionVeryLoosePlus");
		evr.FillList(pim,"PionVeryLooseMinus");
		evr.FillList(gam,"Neutral");
		
		nmult->Fill(gam.GetLength());
		
		// **** now start combining all composits; inbetween plot masses
		//      before using the mass selectors
		//
		
		// phi -> K+ K-
		phi.Combine(kp, km);
		for (j=0;j<phi.GetLength();++j) phimass->Fill(phi[j].M());
		phi.Select(phiMSel);
		
		// Ds+ -> phi pi+, Ds- -> phi pi-
		dsp.Combine(phi, pip);
		dsm.Combine(phi, pim);
		for (j=0;j<dsm.GetLength();++j) dsmass->Fill(dsm[j].M());
		for (j=0;j<dsp.GetLength();++j) dsmass->Fill(dsp[j].M());
		dsm.Select(dsMSel);
		dsp.Select(dsMSel);
		
		// pi0 -> gamma gamma
		pi0.Combine(gam,gam);
		for (j=0;j<pi0.GetLength();++j) pi0mass->Fill(pi0[j].M());
		pi0.Select(pi0MSel);
		
		// Ds0*+ -> Ds+ pi0, Ds0*- -> Ds- pi0
		ds0p.Combine(dsp, pi0);
		ds0m.Combine(dsm, pi0);	
		for (j=0;j<ds0p.GetLength();++j) ds0mass->Fill(ds0p[j].M());
		for (j=0;j<ds0m.GetLength();++j) ds0mass->Fill(ds0m[j].M());
		
		// ppbar -> Ds+ Ds0*- 
		pp.Combine(dsp, ds0m);
		// don't need Ds- Ds0*+; w/0 fitting would be double counting
		
		for (j=0;j<pp.GetLength();++j) ppmass->Fill(pp[j].M());
		
	}
	
	// **** plot all that stuff
	//
	c1->cd(1); phimass->Draw();
	c1->cd(2); pi0mass->Draw();
	c1->cd(3); dsmass->Draw();
	c1->cd(4); ds0mass->Draw();
	c1->cd(5); ppmass->Draw();
	c1->cd(6); nmult->Draw();
	
	timer.Stop();
	Double_t rtime = timer.RealTime();
	Double_t ctime = timer.CpuTime();
	printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
}

int printRecursive(TCandidate *tc, int level)
{

  int i=0;
  int nd=tc->NDaughters();
  if (nd==0) return;
  cout <<tc->Uid()<<"("<<level<<") -> ";
  for (i=0;i<nd;i++) cout<<tc->Daughter(i)->Uid()<<"  ";
  cout <<endl;
  for (i=0;i<nd;i++) printRecursive(tc->Daughter(i),level+1);
  if (level==0) cout <<endl;
  return 0;
}
