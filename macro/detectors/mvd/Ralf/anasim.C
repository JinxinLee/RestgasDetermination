// root macro to analyze the simulation output
//void convertMCPoints()
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
  gROOT->Macro("../Libs.C");
  gROOT->LoadMacro("../Tools.C");
  LoadPandaStyle();
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
/*
Convert PndMvdWaferMCPoints to the STAR Format (text, "x y z layer")
for the conformal mapping stuff
*/

//   PndFileNameCreator namecreator("../data/mvddpm6GeV.root");
  PndFileNameCreator namecreator("../data/mvdStrip.root");
  std::string inFile = namecreator.GetSimFileName(false);
  TString picture = namecreator.GetSimFileName(false);
  picture.ReplaceAll(".root",".ps");

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("pndsim") ;
  TClonesArray* hit_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&hit_array);//Branch names

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
  PndGeoHandling* fGeoH = new PndGeoHandling();

  // histos
  TH2D* hisxy = new TH2D("hisxy","MVD MC Points, xy view",200,-15.,15.,200,-15.,15.);
  TH2D* hisrz = new TH2D("hisrz","MVD MC Points, rz view",200,-20.,20.,200,-15.,25.);
  TH1D* hisde = new TH1D("hisde","MVD MC Points, Energyloss",200,0.,0.002);

//   TH2D* hisdedx = new TH2D("hisdedx","MVD MC Points, dE/dx",200,0.,1.,200,0.,0.002);
  TH2D* hisdedx =new TH2D("hisdedx","",200,0.,0.4,200,0.,1.0);
  hisdedx->SetTitle("MVD MC Points, dE/dx(P);p/GeV / cm;dE/dx / (GeV/cm)");



  TH1D* hismom = new TH1D("hismom","MVD MC Points, momentum",100,0.,1.5);
  TH2D* hisLocalXY = new TH2D("hisLocalxy","",100,-5.,5.,100,-5.,5.);
  hisLocalXY->SetTitle("Local Hit XY;x_{L} / cm;y_{L} / cm");
  TH1D* hisLocalZ = new TH1D("localz","",100,-0.005, 0.005);
  hisLocalZ->SetTitle("Locel MC Hit Z;z / cm");

  int  nEvents = 1000;
  bool verbose = false;

  TVector3 vecs,veco;
  TVector3 vecFront,vecBack,vecP;
  Double_t dx,dE,p,dEdX;
  Double_t tmpMaster[3],tmpLocal[3];
  TGeoHMatrix* currentTransMat;
  TString detname;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if( verbose || 0 == j%1000 ) cout<<"Event No "<<j<<endl;
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *hit=(PndSdsMCPoint*)hit_array->At(i);
      int mcpdg = -1;

      //PndMCTrack *mctruth = (PndMCTrack*)mc_array->At(hit->GetTrackID());
      //mcpdg = mctruth->GetPdgCode();
      //cout<<"mcpdg="<<mcpdg<<endl;

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      veco.SetXYZ(hit->GetXOut(),hit->GetYOut(),hit->GetZOut());
      vecP.SetXYZ(hit->GetPx(),hit->GetPy(),hit->GetPz());
      dx=(veco-vecs).Mag();
      dE=hit->GetEnergyLoss();
      p=vecP.Mag();
      Int_t layer = Int_t(10.*vecs->Mag());
      if(verbose) cout<<vecs.x()<<"\t"<<vecs.y()<<"\t"<<vecs.z()<<endl;

      hisxy->Fill(vecs.x(),vecs.y());
      hisrz->Fill(vecs.z(),((vecs.y()>0.)?1.:-1.)*vecs.Perp());
      hisde->Fill(dE);
      hismom->Fill(p);

      if(dx!=0)
      {
        dEdX=(dE/dx);
        hisdedx->Fill(p,dEdX);
      }

      detname = fGeoH->GetPath( hit->GetDetName());
      geoMan->cd( detname.Data() );
      currentTransMat = geoMan->GetCurrentMatrix();
      tmpMaster[0]=0.5*(veco.x()+vecs.x());
      tmpMaster[1]=0.5*(veco.y()+vecs.y());
      tmpMaster[2]=0.5*(veco.z()+vecs.z());
      currentTransMat->MasterToLocal(tmpMaster,tmpLocal);
      vecs.SetXYZ(tmpLocal[0],tmpLocal[1],tmpLocal[2]);
      hisLocalXY->Fill(vecs.x(),vecs.y());
      hisLocalZ->Fill(vecs.z());

    }//end for i (points in event)
  }// end for j (events)

TCanvas* can1 = new TCanvas("can1","MCHit view in MVD",0,0,800,800);
can1->Divide(3,3);
can1->cd(1);
DrawNice2DHisto(hisxy);
can1->cd(2);
DrawNice2DHisto(hisrz);
can1->cd(3);
gPad->SetLogy();
hisde->DrawCopy("");
can1->cd(4);
DrawNice2DHisto(hisdedx);
can1->cd(5);
DrawNice2DHisto(hisLocalXY);
can1->cd(6);
hisLocalZ->DrawCopy("");
can1->cd(7);
hismom->DrawCopy("");


can1->Print(picture.Data());




  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
