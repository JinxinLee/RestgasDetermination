int ana_dsdsj(TString fname="dsds_10k.evt.root",int num=0)
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
  TFile* f = new TFile(fname.Data());
  TTree *t=f->Get("pndsim") ;

  // **** for every event, a TCLonesArray with the candidates is stored in pndsim
  //
  TClonesArray *fCands=new TClonesArray("TCandidate");

  t->SetBranchAddress("PndCandidates",&fCands);

  TCandidate *tc;

  // **** create and setup some histos for QA plots
  //
  TH1F *phimass = new TH1F("phimass","phi cands",100,0.95,1.1);
  TH1F *pi0mass = new TH1F("pi0mass","pi0 cands",100,0.135-0.03,0.135+0.03);
  TH1F *dsmass  = new TH1F("dsmass","Ds cands",100,1.968-0.03,1.968+0.03);
  TH1F *ds0mass = new TH1F("ds0mass","Ds0 cands",100,2.317-0.05,2.317+0.05);
  TH1F *ppmass = new TH1F("ppmass","pbarp cands",100,4.306-0.1,4.306+0.1);

  TH1F *nmult=new TH1F("nmult","# neutrals",15,0,15);

  phimass->SetMinimum(0);
  pi0mass->SetMinimum(0);
  dsmass->SetMinimum(0);
  ds0mass->SetMinimum(0);
  ppmass->SetMinimum(0);

  if (num==0) num= t->GetEntriesFast();
  cout <<"\n####### Processing "<<num <<" events...\n"<<endl;

  // **** create all the particle lists we'll need for rebuilding the decay tree
  //
  TCandList allCands,neutralCands,chargedCands, plusCands,minusCands;

  TCandList kpCands,kmCands,piCands;

  TCandList phiCands,pi0Cands,dsCands,ds0Cands,ppCands;

  // **** create and configure the selectors/filters we'd like to use later
  //
  TPidChargedSelector *chargedSel = new TPidChargedSelector;
  TPidNeutralSelector *neutralSel = new TPidNeutralSelector;
  TPidPlusSelector    *plusSel    = new TPidPlusSelector;
  TPidMinusSelector   *minusSel   = new TPidMinusSelector;

  // **** mass selectors for the resonances/composites
  //
  TPidMassSelector *phiMSel       = new TPidMassSelector("phiSelector" , 1.0195 , 0.01);
  TPidMassSelector *pi0MSel       = new TPidMassSelector("pi0Selector" , 0.135  , 0.005);
  TPidMassSelector *dsMSel        = new TPidMassSelector("dsSelector"  , 1.9685 , 0.01);

  TPidSimpleKaonSelector *kSel    = new TPidSimpleKaonSelector();
  kSel->SetCriterion("tight");
  TPidSimplePionSelector *piSel   = new TPidSimplePionSelector();
  piSel->SetCriterion("loose");

  int i2;

  // **** loop over all _events_
  //
  for (Int_t j=0; j< num;j++){

    t->GetEntry(j);
    TFactory::Instance()->Reset();
    allCands.Cleanup();

    // **** loop over all Candidates and add them to the list allCands
    //
    for (Int_t i1=0; i1<fCands->GetEntriesFast(); i1++){
      tc = (TCandidate *)fCands->At(i1);
      allCands.Add(*tc);
    }

    // **** select all the basic list
    //
    chargedCands.Select(allCands, chargedSel);
    neutralCands.Select(allCands, neutralSel);

    nmult->Fill(neutralCands.GetLength());

    plusCands.Select(chargedCands  ,plusSel);
    minusCands.Select(chargedCands ,minusSel);

    // **** pid selection
    //
    kpCands.Select(plusCands    ,kSel);
    kmCands.Select(minusCands   ,kSel);
    piCands.Select(chargedCands ,piSel);

    // **** now start combining all composits; inbetween plot masses
    //      before using the mass selectors
    //
    phiCands.Combine(kpCands,kmCands);

    TCandListIterator iterPhi(phiCands);
    while (tc=iterPhi.Next()) phimass->Fill(tc->M());
    phiCands.Select(phiMSel);

    dsCands.Combine(phiCands,piCands);

    TCandListIterator iterDs(dsCands);
    while (tc=iterDs.Next()) dsmass->Fill(tc->M());
    dsCands.Select(dsMSel);

    pi0Cands.Combine(neutralCands,neutralCands);

    TCandListIterator iterPi0(pi0Cands);
    while (tc=iterPi0.Next()) pi0mass->Fill(tc->M());
    pi0Cands.Select(pi0MSel);

    ds0Cands.Combine(dsCands,pi0Cands);

    TCandListIterator iterDs0(ds0Cands);
    while (tc=iterDs0.Next()) ds0mass->Fill(tc->M());

    ppCands.Combine(ds0Cands,dsCands);
    //cout << ds0Cands.GetLength()<<" "<<dsCands.GetLength()<<" "<<ppCands.GetLength()<<endl;
    ppCands.Select(neutralSel);

    // **** since we didn't care about the D_s charge, combinations might appear in two
    //      different ways; RemoveClones removes double candidates based on the same final states
    ppCands.RemoveClones();

    TCandListIterator iterPp(ppCands);
    while (tc=iterPp.Next())
    {
      ppmass->Fill(tc->M());

      printRecursive(tc);
    }

      int nch=chargedCands.GetLength();
      int nn=neutralCands.GetLength();
      int nphi=phiCands.GetLength();
      int nds=dsCands.GetLength();
      int nds0=ds0Cands.GetLength();
      int npp=ppCands.GetLength();

    if (npp) cout <<"pp :"<<ppCands[0].GetCharge()<<endl;

    if (!(j%100))
    {
       cout <<"evt:"<<j<<endl;
      if (nch) cout <<"ch :"<<chargedCands[0].GetCharge()<<endl;
      if (nn) cout <<"ne :"<<neutralCands[0].GetCharge()<<endl;
      if (nphi) cout <<"phi:"<<phiCands[0].GetCharge()<<endl;
      if (nds) cout <<"ds :"<<dsCands[0].GetCharge()<<endl;
      if (nds0) cout <<"ds0:"<<ds0Cands[0].GetCharge()<<endl;
      if (npp) cout <<"pp :"<<ppCands[0].GetCharge()<<endl;
      if (nds>0 && nds0>0 &&dsCands[0].GetCharge()!=ds0Cands[0].GetCharge()) cout <<"****"<<endl;
       cout <<endl;
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
  nmult->Draw();

  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

/*void printRecursive(TCandidate *tc, Int_t level=0)
{

  int i=0;
  int nd=tc->NDaughters();
  if (nd==0) return;
  cout <<tc->Uid()<<"("<<level<<") -> ";
  for (i=0;i<nd;i++) cout<<tc->Daughter(i)->Uid()<<"  ";
  cout <<endl;
  for (i=0;i<nd;i++) printRecursive(tc->Daughter(i),level+1);
  if (level==0) cout <<endl;

}
*/
