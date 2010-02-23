//ROOT macro for visualising obtained genfit straight line fits 
//author: Felix Boehmer

plotFits(TString reco_file, int evHi, int evLo=0, int markerstyle=2) {

  TFile* recofile = new TFile(reco_file);
  int MARKERSTYLE=markerstyle;
  
  double driftvel = 0.00055192;

  if(recofile->IsZombie()) {
    std::cerr<<"Reco-file is not existing! Aborting."<<std::endl;
    return 0;
  }
  
  TString digi_file = reco_file;
  digi_file.ReplaceAll("reco.root", "raw.root");
  TFile* digifile = new TFile(digi_file);

  if(digifile->IsZombie()) {
    std::cerr<<"Digi-file is not existing! Aborting."<<std::endl;
    return 0;
  }
  
  TH3D* hist = new TH3D("bla", "", 100,-5,5,100,-5,5,100,0,10);
  TCanvas* canv = new TCanvas();
  hist->Draw();


  TTree* recotree = recofile->Get("cbmsim");
  TTree* digitree = digifile->Get("cbmsim");
  
  TClonesArray* trackArr = new TClonesArray("GFTrack");
  TClonesArray* drelArr = new TClonesArray("PndTpcDriftedElectron");
  TClonesArray* clustArr = new TClonesArray("PndTpcCluster");
  
  recotree->SetBranchAddress("TrackPostFit", &trackArr);
  recotree->SetBranchAddress("PndTpcClusterBlubber", &clustArr);
  digitree->SetBranchAddress("PndTpcDriftedElectron", &drelArr);


  int nEvents = evHi-evLo;
  TObjArray* markers = new TObjArray();
  TObjArray* lines = new TObjArray();
  
  markers->Print();
  
  for(unsigned int ev=0; ev<nEvents; ev++) {
    
    recotree->GetEvent(evLo+ev);
    digitree->GetEvent(evLo+ev);
    
    unsigned int nEl = drelArr->GetEntriesFast();
    unsigned int nCl = clustArr->GetEntriesFast();
    
    TPolyMarker3D* de_m = new TPolyMarker3D(nEl, 1);
    TPolyMarker3D* cl_m = new TPolyMarker3D(nEl, MARKERSTYLE);
    cl_m->SetMarkerColor(kOrange+2);
    //de_m->Print();
    markers->Add(de_m);
    markers->Add(cl_m);
    
    //markers->Print();

    for(unsigned int k=0; k<nEl; k++) {
      PndTpcDriftedElectron* el = drelArr->At(k);
      double z = el->t()*driftvel;
      de_m->SetPoint(k,el->x(),el->y(),z);
    }
    
    for(unsigned int l=0; l<nCl; l++) {
      PndTpcCluster* cl = clustArr->At(l);
      TVector3 clpos = cl->pos();
      cl_m->SetPoint(l,clpos.X(),clpos.Y(),clpos.Z());
    }
    

    GFTrack* tr = trackArr->At(0);
    TVector3 pos = tr->getPos();
    TVector3 mom = tr->getMom();
    
    TPolyLine3D* line =  new TPolyLine3D(2);
    mom = mom*(1/mom.Mag());
    TVector3 pos2 = mom*9+pos;
    line->SetPoint(0,pos.X(),pos.Y(),pos.Z());
    line->SetPoint(1,pos2.X(),pos2.Y(),pos2.Z());
    
    lines->Add(line);
    line->Draw("same");
    
    cl_m->Draw("same");
    de_m->Draw("same");

  }
  
  
}
