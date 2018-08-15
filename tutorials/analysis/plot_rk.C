{
  Int_t nEvents = 100;

  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  LoadPandaStyle();

  TFile* f = new TFile("data/points_sttcombi.root"); // the sim file you want to analyse
  TTree* t=(TTree*)f->Get("pndsim");
  t->AddFriend("pndsim","data/digi_sttcombi.root"); // the digi file
  t->AddFriend("pndsim","data/reco_sttcombi.root"); // the reco file you want to analyse
  t->AddFriend("pndsim","data/lhepidmaker.root"); // tracking output
  t->AddFriend("pndsim","data/fit_sttcombi.root"); // debug output


  TClonesArray* mctrack_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mctrack_array);//Branch names

  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names
  TClonesArray* digiPixel_array=new TClonesArray("PndSdsDigiPixel");
  t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names
  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names
  TClonesArray* stripClust_array=new TClonesArray("PndSdsCluster");
  t->SetBranchAddress("MVDStripClusterCand",&stripClust_array);//Branch names
  TClonesArray* pixelClust_array=new TClonesArray("PndSdsCluster");
  t->SetBranchAddress("MVDClusterCand",&pixelClust_array);//Branch names
  TClonesArray* stripHit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("MVDHitsStrip",&stripHit_array);//Branch names
  TClonesArray* pixelHit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("MVDHitsPixel",&pixelHit_array);//Branch names

  TClonesArray* track_array=new TClonesArray("PndLhePidTrack");
  t->SetBranchAddress("LhePidTrack",&track_array);//Branch names


  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
  PndMvdGeoHandling* fGeoH = new PndMvdGeoHandling(geoMan);
  PndEmcMapper::Instance(2,f.GetName());

  //-------------------------------------------------------

  TH1D* hMomDiff = new TH1D("hmomdiff","Mom difference track to mctrack;|p-p_{mc}|/ GeV/c;",500,-15.,15.);


  TCanvas* c1 = new TCanvas("LHETRACK MOMENTIM", "LHETRACK MOMENTUM", 10, 10, 1200, 600);
  c1.Divide(4,2);

  c1->cd(1);
  t->Draw("LhePidTrack.GetMomentum().Mag()>>momlhe(100,0.5,1.5)","","");
  momlhe->SetTitle("LhePidTrack;Momentum / GeV/c");
  c1.cd(2);
  t->Draw("LhePidTrack.GetMomentum().Mag()-MCTrack.GetMomentum().Mag()>>momlhedf(100,-2.5,2.5)","","");
  momlhedf->SetTitle("LhePidTrack - MC;Momentum / GeV/c");

  c1->cd(3);
  t->Draw("LheGenTrack.getMom().Mag()>>momfit(100,0.5,1.5)","","");
  momfit->SetTitle("genfit Track;Momentum / GeV/c");
  c1->cd(4);
  t->Draw("LheGenTrack.getMom().Mag()-MCTrack.GetMomentum().Mag()>>momfitdf(100,-2.5,2.5)","","");
  momfitdf->SetTitle("genfit Track - MC;Momentum / GeV/c");

  c1.cd(5);
  t->Draw("MCTrack.GetMomentum().Mag():LhePidTrack.GetMomentum().Mag()-MCTrack.GetMomentum().Mag()>>momlhedfcor(100,-2.5,2.5,100,0.5,1.5)","","colz");
  momlhedfcor->SetTitle(";LHE - MC Momentum / GeV/c;MC Momentum / GeV/c");
  c1.cd(6);
  t->Draw("MCTrack.GetMomentum().Mag():LhePidTrack.GetMomentum().Mag()>>momlhecor(100,0.5,1.5,100,0.5,1.5)","","colz");
  momlhecor->SetTitle(";LHE Momentum / GeV/c;MC Momentum / GeV/c");



  c1.cd(7);
  t->Draw("MCTrack.GetMomentum().Mag():LheGenTrack.getMom().Mag()-MCTrack.GetMomentum().Mag()>>momfitdfcor(100,-2.5,2.5,100,0.5,1.5)","","colz");
  momfitdfcor->SetTitle(";GEN - MC Momentum / GeV/c;MC Momentum / GeV/c");
  c1.cd(8);
  t->Draw("MCTrack.GetMomentum().Mag():LheGenTrack.getMom().Mag()>>momfitcor(100,0.5,1.5,100,0.5,1.5)","","colz");
  momfitcor->SetTitle(";GEN Momentum / GeV/c;MC Momentum / GeV/c");


  TCanvas* c2 = new TCanvas("LHETRACK PID", "LHETRACK PID", 20, 20, 600, 600);
  c2.Divide(2,2);

  c2->cd(1);
  t->Draw("1000*LhePidTrack.GetMvdELoss()/LhePidTrack.GetMvdHitCounts():LhePidTrack.GetMomentum().Mag()>>mvd_pid(100,0,3,100,0,2)","LhePidTrack.GetMvdHitCounts()>0","colz");
  mvd_pid->SetTitle("MVD pid;Momentum [GeV/c];MVD dE / # hit [MeV]");

  c2.cd(2);
  t->Draw("LhePidTrack.GetBeta():LhePidTrack.GetMomentum().Mag()>>tof_pid(100,0,2,100,0,1.2)","LhePidTrack.GetTofIndex()>-1","colz");
  tof_pid->SetTitle("TOF pid;Momentum [GeV/c];#beta");

  c2.cd(3);
  t->Draw("LhePidTrack.GetDrcThetaC()*TMath::RadToDeg():LhePidTrack.GetMomentum().Mag()>>drc_pid(100,0,3,100,0,60)","LhePidTrack.GetDrcIndex()>-1","colz");
  drc_pid->SetTitle("DRC pid;Momentum [GeV/c];#theta_{c} [^{0}]");

  c2.cd(4);
  t->Draw("LhePidTrack.GetEmcELoss()/LhePidTrack.GetMomentum().Mag():LhePidTrack.GetMomentum().Mag()>>emc_pid(100,0,3,100,0,1.2)","LhePidTrack.GetEmcIndex()>-1","colz");
  emc_pid->SetTitle("EMC pid;Momentum [GeV/c];EMC Energy / Momentum [c]");

}
