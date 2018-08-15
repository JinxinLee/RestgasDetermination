int ana_d0d0b_micro(TString fname="dsds_10k.evt.root",int num=0)
{
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    // **** the library is needed in order to use the TCandidate and TCandList etc
    gSystem->Load("libRho");

    TCanvas *c1=new TCanvas("c1","c1",600,600);
    c1->Divide(2,2);

    // **** access the datastructure holding the particle lists
    //
    //TFile* f = new TFile(fname.Data());
    //TTree *t=f->Get("pndsim") ;

	TChain *t=new TChain("pndsim");
	t->Add(fname);
	//t->Add("dsdsj20k_4291.evt.fast.root");

    // **** for every event, a TCLonesArray with the candidates is stored in pndsim
    //
    TClonesArray *fMcCands=new TClonesArray("TCandidate");
	TClonesArray *fMicro=new TClonesArray("PndPidCandidate");

    t->SetBranchAddress("PndMcTracks",&fMcCands);
	t->SetBranchAddress("PndPidCandidates",&fMicro);

    TCandidate *tc;

    // **** create and setup some histos for QA plots
    //
    TH1F *d0mass  = new TH1F("d0mass","Ds cands",100,1.864-0.05,1.864+0.05);
    TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,4.4,4.6);

	//TH1F *d0d0b=new TH2F("d0d0b","",100,1.864-0.05,1.864+0.05,100,1.864-0.05,1.864+0.05);

    TH1F *nmult=new TH1F("nmult","# neutrals",15,0,15);

    if (num==0) num= t->GetEntries();
    cout <<"\n####### Processing "<<num <<" events...\n"<<endl;

    // **** create all the particle lists we'll need for rebuilding the decay tree
    //
    TCandList mc;

    TCandList neut,chrg, plus,minus;

    TCandList kp,km,pip,pim;

    TCandList d0,d0b,pp,pipi;

    // **** create and configure the selectors/filters we'd like to use later
    //
    TPidChargedSelector *chargedSel = new TPidChargedSelector;
    TPidNeutralSelector *neutralSel = new TPidNeutralSelector;
    TPidPlusSelector    *plusSel    = new TPidPlusSelector;
    TPidMinusSelector   *minusSel   = new TPidMinusSelector;

    // **** mass selectors for the resonances/composites
    //
    TPidMassSelector *d0MSel        = new TPidMassSelector("d0Selector"  , 1.864 , 0.03);

    TPidSimpleKaonSelector *kSel    = new TPidSimpleKaonSelector();
    kSel->SetCriterion("veryLoose");
    TPidSimplePionSelector *piSel   = new TPidSimplePionSelector();
    piSel->SetCriterion("veryLoose");

    int i1,i2;

    // **** loop over all _events_
    //
    for (Int_t j=0; j< num;j++)
    {
       if ((j%100)==0) cout <<"evt "<<j<<endl;

		t->GetEntry(j);
		TFactory::Instance()->Reset();

		mc.Cleanup();
		chrg.Cleanup();
		neut.Cleanup();

		// **** loop over all Candidates and add them to the list allCands
		//

		for (i1=0; i1<fMicro->GetEntriesFast(); i1++){
			//PndPidCandidate *mic = (PndMicroCandidate *)fMicro->At(i1);
			TCandidate tc2(*((PndPidCandidate*)fMicro->At(i1)),i1);
			if (abs(tc2.Charge())>0.01)chrg.Add(tc2);
			else neut.Add(tc2);
		}


		for (i1=0; i1<fMcCands->GetEntriesFast(); i1++){
			tc = (TCandidate *)fMcCands->At(i1);
			mc.Add(*tc);
		}

		// **** select all the basic list

		nmult->Fill(neut.GetLength());

		plus.Select(chrg  ,plusSel);
		minus.Select(chrg ,minusSel);

		// **** pid selection
		//
		kp.Select(plus    ,kSel);
		km.Select(minus   ,kSel);
		pip.Select(plus  ,piSel);
		pim.Select(minus ,piSel);

		// **** now start combining all composits; inbetween plot masses
		//      before using the mass selectors
		//
		d0.Combine(km,pip);
		d0b.Combine(kp,pim);

		for (i2=0;i2<d0.GetLength();i2++) d0mass->Fill(d0[i2].M());
		for (i2=0;i2<d0b.GetLength();i2++) d0mass->Fill(d0b[i2].M());

		d0.Select(d0MSel);
		d0b.Select(d0MSel);

		pp.Combine(d0,d0b,pip,pim);

		for (i2=0;i2<pp.GetLength();i2++) ppmass->Fill(pp[i2].M());

    }

    // **** plot all that stuff
    //
    c1->cd(1);
    d0mass->Draw();
    c1->cd(2);
    ppmass->Draw();
    c1->cd(3);
    nmult->Draw();

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
