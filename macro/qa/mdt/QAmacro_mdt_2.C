void QAmacro_mdt_2()
{

  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();

  Bool_t Test=kTRUE;

  TFile* f = new TFile("pygen_sim_01_test04_test2.root");
  TTree *tree=(TTree *) f->Get("cbmsim") ;
  TGeoManager *geoMan = (TGeoManager*) f->Get("FAIRGeom");


  TClonesArray* fMdt=new TClonesArray("PndMdtPoint");
  tree->SetBranchAddress("MdtPoint",&fMdt);

  TClonesArray* fMC=new TClonesArray("PndMCTrack");
  tree->SetBranchAddress("MCTrack",&fMC);

  Int_t NEvents = (Int_t) tree->GetEntriesFast();
  cout<<"N. of events: "<<NEvents<<endl;

  Int_t Pid1, Pid2;
  // Loop on events
  for (Int_t js=0; js<NEvents; js++){
    cout<<"Event "<<js<<endl;
    tree->GetEntry(js);
    // Dimension of arrays
    cout<<"fMC->GetEntriesFast()   "<<fMC->GetEntriesFast()<<endl;
    Int_t NPoints = (Int_t) fMdt->GetEntriesFast();
    cout<<"fMdt->GetEntriesFast()   "<<fMdt->GetEntriesFast()<<endl;
    Int_t pmp_tid = fMC->GetEntriesFast();
    cout<<"N. of hits: "<<NPoints<<endl;

    // Loop on the tracks
  /*  for (Int_t trk_loop=0; trk_loop<pmp_tid; trk_loop++){
      PndMCTrack* track = (PndMCTrack*)fMC->At(trk_loop);
      cout<<trk_loop<<"    Pid "<<track->GetPdgCode()<<endl;
      // 1) position in the array
      // 2) PID of the track

    } // end loop trk
  */
  
    for (Int_t i=0; i<NPoints ; i++){
      PndMdtPoint* point = (PndMdtPoint*)fMdt->At(i);
      PndMCTrack* track = (PndMCTrack*)fMC->At(point->GetTrackID());
      Pid1=point->GetPDG();
      Pid2= track->GetPdgCode();
      if((Pid1-Pid2)!=0) {cout<< " - "<< i <<" (Point ) Pid "<< Pid1 << "  track Pid  "<<  Pid2 << endl;
      Test=kFALSE;
      }
    } // end loop trk
  
  
    fMC->Delete();

  } // end loop on events

  if (Test){
     cout << " Test passed" << endl;
     cout << " All ok " << endl;  
  }else{
     cout << " Test Failed" << endl;
     cout << " Not Ok " << endl;         
  }
  exit(0);


}
