{
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
 gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
   gSystem->Load("libEnDrc");          // our library
  TFile* f = new TFile("DpmSimu1000.root");
  //  TFile* f = new TFile("sim_endrcP.root");
  TTree *t=f->Get("pndsim") ;
  TClonesArray *fT=new TClonesArray("PndEnDrcPoint");
  t->SetBranchAddress("EnDrcTrue",&fT) ;

  PndEnDrcPoint* edp;
  fstream hitsfile;
  hitsfile.open("output.hit",std::ios::out);
  cout<<"will loop over "<<t->GetEntries()<<" events"<<endl;
  for(Int_t i=0;i<t->GetEntries();i++){
    //loop over events in tree
    t->GetEntry(i);
    Double_t *point;
    cout<<"will loop over "<<fT->GetEntries()<<" tracks for event "<<i<<endl;
    for (Int_t j=0; j<fT->GetEntries(); j++){

      edp=static_cast<PndEnDrcPoint*>(fT->At(j));
      hitsfile<<edp->GetX()<<" "<<edp->GetY()<<" "<<edp->GetZ()<<" ";
      hitsfile<<edp->GetPx()<<" "<<edp->GetPy()<<" "<<edp->GetPz()<<" ";
      hitsfile<<edp->GetTime()<<" "<<edp->GetPdgCode()<<endl;
    }
    //end of evnt marker
    hitsfile<<-1<<" "<<-1<<" "<<-1<<" ";
    hitsfile<<-1<<" "<<-1<<" "<<-1<<" ";
    hitsfile<<-1<<" "<<-1<<endl;
  }
  //end of file marker
  hitsfile<<-11<<" "<<-11<<" "<<-11<<" ";
  hitsfile<<-11<<" "<<-11<<" "<<-11<<" ";
  hitsfile<<-11<<" "<<-11<<endl;


  f->Close();
}
