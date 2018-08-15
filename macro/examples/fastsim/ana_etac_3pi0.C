int ana_etac_3pi0(TString fname="etac_3pi0.root",int num=0)
{
    TStopwatch timer;
    timer.Start();

    gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
    basiclibs();

    // **** the library is needed in order to use the TCandidate and TCandList etc
    gSystem->Load("libRho");

    TCanvas *c1=new TCanvas("c1","c1",600,300);
    c1->Divide(2,1);

    // **** access the datastructure holding the particle lists
    //
    TFile* f = new TFile(fname.Data());
    TTree *t=f->Get("pndsim") ;

    // **** for every event, a TCLonesArray with the candidates is stored in pndsim
    //
    TClonesArray *fChrgCands=new TClonesArray("TCandidate");
    TClonesArray *fNeutCands=new TClonesArray("TCandidate");
    TClonesArray *fMcCands=new TClonesArray("TCandidate");

    t->SetBranchAddress("PndChargedCandidates",&fChrgCands);
    t->SetBranchAddress("PndNeutralCandidates",&fNeutCands);
    t->SetBranchAddress("PndMcTracks",&fMcCands);

    TCandidate *tc;

    // **** create and setup some histos for QA plots
    //
    TH1F *pi0mass = new TH1F("pi0mass","pi0 cands",100,0.135-0.03,0.135+0.03);
    TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,2.979-0.2,2.979+0.2);

    TH1F *nmult=new TH1F("nmult","# neutrals",15,0,15);

    TH1F *pdiff=new TH1F("pdiff","momentum difference",100,-0.05,0.05);

    pi0mass->SetMinimum(0);
    ppmass->SetMinimum(0);

    if (num==0) num= t->GetEntriesFast();
    cout <<"\n####### Processing "<<num <<" events...\n"<<endl;

    // **** create all the particle lists we'll need for rebuilding the decay tree
    //
    TCandList mcCands;

    TCandList allCands,neutralCands,chargedCands;

    TCandList pi0pi0Cands,pi0Cands,ppCands;

    // **** create and configure the selectors/filters we'd like to use later
    //
    TPidChargedSelector *chargedSel = new TPidChargedSelector;
    TPidNeutralSelector *neutralSel = new TPidNeutralSelector;

    // **** mass selectors for the resonances/composites
    //
    TPidMassSelector *pi0MSel       = new TPidMassSelector("pi0Selector" , 0.135  , 0.005);

    int i1,i2;

    // **** loop over all _events_
    //
    for (Int_t j=0; j< num;j++)
    {
       if ((j%100)==0) cout <<"evt "<<j<<endl;

	t->GetEntry(j);
	TFactory::Instance()->Reset();

	mcCands.Cleanup();
	chargedCands.Cleanup();
	neutralCands.Cleanup();

	// **** loop over all Candidates and add them to the list allCands
	//

	for (i1=0; i1<fChrgCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fChrgCands->At(i1);
	    chargedCands.Add(*tc);
	}

	for (i1=0; i1<fNeutCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fNeutCands->At(i1);
	    neutralCands.Add(*tc);
	}

	for (i1=0; i1<fMcCands->GetEntriesFast(); i1++){
	    tc = (TCandidate *)fMcCands->At(i1);
	    mcCands.Add(*tc);
	}


	// **** select all the basic list
	//

	//cout <<"c:"<<chargedCands.GetLength()<<" n:"<<neutralCands.GetLength()<<endl;


	pi0Cands.Combine(neutralCands,neutralCands);

	TCandListIterator iterPi0(pi0Cands);
	while (tc=iterPi0.Next()) pi0mass->Fill(tc->M());
	//pi0Cands.Select(pi0MSel);

	pi0pi0Cands.Combine(pi0Cands,pi0Cands);

	ppCands.Combine(pi0pi0Cands,pi0Cands);

	TCandListIterator iterPp(ppCands);

	while (tc=iterPp.Next())
	{
	    ppmass->Fill(tc->M());
	    //printTree(tc);
	}
    }
    // **** plot all that stuff
    //
    c1->cd(1);
    pi0mass->Draw();
    c1->cd(2);
    ppmass->Draw();


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
