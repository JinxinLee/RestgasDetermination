{
  TFile::Open("out.root");
  TTree *t = gROOT->FindObject("at");

  int nEvt = t->GetEntries();

  TCtrack *tr=0;

  t->SetBranchAddress("track",&tr);
  
  for(int i=0;i<nEvt;++i){
    t->GetEntry(i);

    tr->print();

  }


}
