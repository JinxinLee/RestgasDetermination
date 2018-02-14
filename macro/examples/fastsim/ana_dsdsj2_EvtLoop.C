int ana_dsdsj2_EvtLoop(TString fname="dsds_10k.evt.root",int num=0)
{
    TStopwatch timer;
    timer.Start();
    
    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();
    
    // **** the library is needed in order to use the TCandidate and TCandList etc
    gSystem->Load("libRho");
    
    TCanvas *c1=new TCanvas("c1","c1",900,600);
    c1->Divide(3,2);
    
    // **** access the datastructure holding the particle lists
    //
    
	PndEventReader evl(fname);
	
    
	// **** create and setup some histos for QA plots
    //
    TH1F *phimass = new TH1F("phimass","phi cands",100,0.98,1.06);
    TH1F *pi0mass = new TH1F("pi0mass","pi0 cands",100,0.0,0.3);
    TH1F *dsmass  = new TH1F("dsmass","Ds cands",100,1.868,2.068); 
    TH1F *ds0mass = new TH1F("ds0mass","Ds0 cands",100,2.217,2.417);
    TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,4.185,4.385);
    
    TH1F *nmult=new TH1F("nmult","# neutrals",15,0,15);
    
    TH1F *pdiff=new TH1F("pdiff","momentum difference",100,-0.05,0.05);
    
    phimass->SetMinimum(0);
    pi0mass->SetMinimum(0);
    dsmass->SetMinimum(0);
    ds0mass->SetMinimum(0);
    ppmass->SetMinimum(0);
    
    if (num==0) num= evl->GetEntries();
    cout <<"\n####### Processing "<<num <<" events...\n"<<endl;
    
    // **** create all the particle lists we'll need for rebuilding the decay tree
    //
    TCandList mcCands;
    TCandList chargedCands, neutralCands, gammaCands, kpCands,kmCands,pipCands,pimCands;
    
    TCandList phiCands,pi0Cands,dspCands,dsmCands,ds0pCands,ds0mCands,ppCands;
        
    // **** mass selectors for the resonances/composites
    //
    TPidMassSelector *phiMSel       = new TPidMassSelector("phiSelector" , 1.0195 , 0.03);
    TPidMassSelector *pi0MSel       = new TPidMassSelector("pi0Selector" , 0.135  , 0.04);
    TPidMassSelector *dsMSel        = new TPidMassSelector("dsSelector"  , 1.9685 , 0.04);
    
    int i1,i2;
    
    // **** loop over all _events_
    //
	
	int cnt=0,currev=0;
	
	TCandidate *tc;
	
	while (evl.GetEvent() && cnt<num)
	{
		if ((cnt%100)==0) cout <<"evt "<<cnt<<endl;
		cnt++;
		
		evl.FillList(chargedCands,"Charged");
		evl.FillList(mcCands,"McTruth");
		
		evl.FillList(neutralCands,"Neutral");
		evl.FillList(kpCands,"KaonVeryLoosePlus");
		evl.FillList(kmCands,"KaonVeryLooseMinus");
		evl.FillList(pipCands,"PionVeryLoosePlus");
		evl.FillList(pimCands,"PionVeryLooseMinus");
		
		gammaCands.Cleanup();
		for (i1=0;i1<neutralCands.GetLength();i1++)
		{
			double minang=10;
			for (i2=0;i2<chargedCands.GetLength();i2++)
			{
				double ang=neutralCands[i1].P3().Angle(chargedCands[i2].P3());
				if (ang<minang) minang=ang;
			}
			if (( neutralCands[i1].GetMicroCandidate()->GetEmcNumberOfCrystals()>=3
				&& minang>0.3))
				gammaCands.Add(neutralCands[i1]);
		}

		nmult->Fill(neutralCands.GetLength());
			
		
		// **** now start combining all composits; inbetween plot masses
		//      before using the mass selectors
		//
		phiCands.Combine(kpCands,kmCands);
		
		//TCandListIterator iterPhi(phiCands);
		phiCands.Select(phiMSel);
		for (i1=0;i1<phiCands.GetLength();i1++) phimass->Fill(phiCands[i1].M());
		
		dspCands.Combine(phiCands,pipCands);
		dsmCands.Combine(phiCands,pimCands);
		
		for (i1=0;i1<dspCands.GetLength();i1++) dsmass->Fill(dspCands[i1].M()); 
		for (i1=0;i1<dsmCands.GetLength();i1++) dsmass->Fill(dsmCands[i1].M()); 
		dspCands.Select(dsMSel);
		dsmCands.Select(dsMSel);
		
		pi0Cands.Combine(gammaCands,gammaCands);
		
		for (i1=0;i1<pi0Cands.GetLength();i1++) pi0mass->Fill(pi0Cands[i1].M()); 
		pi0Cands.Select(pi0MSel);
		
		ds0pCands.Combine(dspCands,pi0Cands);
		ds0mCands.Combine(dsmCands,pi0Cands);
		
		for (i1=0;i1<ds0pCands.GetLength();i1++) ds0mass->Fill(ds0pCands[i1].M());  
		for (i1=0;i1<ds0mCands.GetLength();i1++) ds0mass->Fill(ds0mCands[i1].M());  
		
		ppCands.Combine(ds0pCands,dsmCands);
		ppCands.CombineAndAppend(ds0mCands,dspCands);
		
		for (i1=0;i1<ppCands.GetLength();i1++) ppmass->Fill(ppCands[i1].M());  
		
		for (i1=0;i1<chargedCands.GetLength();i1++)
		{
			int idx=chargedCands[i1].GetMcIdx();
			if (idx!=-1) pdiff->Fill(chargedCands[i1].P()-mcCands[idx].P());
		}
		
	
    }
    
    // **** plot all that stuff
    //
    c1->cd(1);
    phimass->Draw();
    c1->cd(2);
    pi0mass->Draw();
    c1->cd(3);
    dsmass->Draw();
    c1->cd(4);
    ds0mass->Draw();
    c1->cd(5);
    ppmass->Draw();
    c1->cd(6);
    pdiff->Draw();
    
    timer.Stop();
    Double_t rtime = timer.RealTime();
    Double_t ctime = timer.CpuTime();
    
    printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
      return 0;

}

/*void printTree(TCandidate *tc, int level=0)
{
  int i=0;
  int nd=tc->NDaughters();
  if (nd==0) return;
  cout <<tc->Uid()<<"("<<level<<") -> ";
  for (i=0;i<nd;i++) cout<<tc->Daughter(i)->Uid()<<"  ";
  cout <<endl;
  for (i=0;i<nd;i++) printTree(tc->Daughter(i),level+1);
  if (level==0) cout <<endl;
}
*/
