// root macro to analyze the clusterization output
{
  int  nEvents = 1000;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndData");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");

  gSystem->Load("libgenfit");
   gSystem->Load("libtrackrep");
 gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");

  gSystem->Load("libMvd");
  gSystem->Load("libMvdReco");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  TFile* f = new TFile("../data/testMCCluster.root"); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;
  TClonesArray* hit_array=new TClonesArray("MvdCluster");
  t->SetBranchAddress("MVDCluster",&hit_array);//Branch names

  TFile* F = new TFile("testMC.root"); // the sim file you want to analyse
  TTree *T=(TTree *) F->Get("cbmsim") ;
  TClonesArray* mc_array=new TClonesArray("MvdMCPoint");
  T->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");

  TH2D* hisxy = new TH2D("hisxy","",400,-15.,15.,400,-15.,15.);
  hisxy->SetTitle("MVD MC Cluster, xy view;x / cm;y / cm");
  TH2D* hisrz = new TH2D("hisrz","",400,-20.,20.,400,-15.,25.);
  hisrz->SetTitle("MVD MC Cluster, rz view;z / cm;r/ cm");
  TH2D* hisSensxy = new TH2D("hisSensxy","",400,-15.,15.,400,-15.,15.);
  hisSensxy->SetTitle("MVD MC Sensor, xy view;x / cm;y / cm");
  TH2D* hisSensrz = new TH2D("hisSensrz","",400,-20.,20.,400,-15.,25.);
  hisSensrz->SetTitle("MVD MC Sensor, rz view;z / cm;r/ cm");

  TH2D* hisDiffXY = new TH2D("hisdiffxy","",100,-0.05,0.05,100,-0.05,0.05);
  hisDiffXY->SetTitle("(MC - RECO) Hit coordinates xy view;#Deltax / cm;#Deltay / cm");
  TH2D* hisDiffRZ = new TH2D("hisdiffrz","",100,-0.05,0.05,100,-0.00,0.10);
  hisDiffRZ->SetTitle("(MC - RECO) Hit coordinates rz view;#Deltaz / cm;#Deltar / cm");

  TH1D* hisDiffX = new TH1D("hisdiffx","",100,-0.05,0.05);
  hisDiffX->SetTitle("(MC - RECO) Hit coordinate x;#Deltax / cm;");
  TH1D* hisDiffY = new TH1D("hisdiffy","",100,-0.05,0.05);
  hisDiffY->SetTitle("(MC - RECO) Hit coordinate y;#Deltay / cm;");
  TH1D* hisDiffZ = new TH1D("hisdiffz","",100,-0.05,0.05);
  hisDiffZ->SetTitle("(MC - RECO) Hit coordinate z;#Deltaz / cm;");

  TH1D* hisDiffLocalX = new TH1D("hisdiffLocalx","",100,-0.05,0.05);
  hisDiffLocalX->SetTitle("(MC - RECO) Local Hit coordinate x;#Deltax_{L} / cm;");
  TH1D* hisDiffLocalY = new TH1D("hisdiffLocaly","",100,-0.05,0.05);
  hisDiffLocalY->SetTitle("(MC - RECO) Local Hit coordinate y;#Deltay_{L} / cm;");
  TH1D* hisDiffLocalZ = new TH1D("hisdiffLocalz","",100,-0.05,0.05);
  hisDiffLocalZ->SetTitle("(MC - RECO) Local Hit coordinate z;#Deltaz_{L} / cm;");
  TH2D* hisDiffLocalXY = new TH2D("hisdiffLocalxy","",100,-0.05,0.05,100,-0.05,0.05);
  hisDiffLocalXY->SetTitle("(MC - RECO) Local Hit XY;#Deltax_{L} / cm;#Deltay_{L} / cm");
  TH2D* hisDiffLocalRZ = new TH2D("hisdiffLocalrz","",100,-0.05,0.05,100,-0.00,0.10);
  hisDiffLocalRZ->SetTitle("(MC - RECO) Local Hit RZ;#Deltaz_{L} / cm;#Deltar_{L} / cm");

  TH2D* hisLocalXY = new TH2D("hisLocalxy","",100,-5.,5.,100,-5.,5.);
  hisLocalXY->SetTitle("(MC - RECO) Local Hit XY;x_{L} / cm;y_{L} / cm");
  TH2D* hisLocalRZ = new TH2D("hisLocalrz","",100,-5.,5.,100,-0.,10.);
  hisLocalRZ->SetTitle("(MC - RECO) Local Hit RZ;z_{L} / cm;r_{L} / cm");
  TH2D* hisLocalYZ = new TH2D("hisLocalyz","",100,-5.,5.,100,-5.,5.);
  hisLocalYZ->SetTitle("(MC - RECO) Local Hit YZ;z_{L} / cm;y_{L} / cm");
  TH2D* hisLocalXZ = new TH2D("hisLocalxz","",100,-5.,5.,100,-5.,5.);
  hisLocalXZ->SetTitle("(MC - RECO) Local Hit XZ;z_{L} / cm;x_{L} / cm");

  TH1D* hisPLUV = new TH1D("hispluv","sensor unit vectors",100,0.9999,1.0001);

  TVector3 vecs, vecmc, vecdiff;
  Double_t tmpx,tmpy,tmpz;
  TVector2 locals, localmc, localdiff;
//   TGeoHMatrix* currentTransMat;
  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    T->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsCluster *hit=(PndSdsCluster*)hit_array->At(i);
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(i);
      int mcpdg = -1;

      geoMan->cd(point->GetDetName());
//       currentTransMat = geoMan->GetCurrentMatrix();

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
      vecdiff = vecmc - vecs;

      Int_t layer = Int_t(10.*vecs->Mag());
      if(verbose) cout<<vecs.x()<<"\t"<<vecs.y()<<"\t"<<vecs.z()<<endl;
      hisxy->Fill(vecs.x(),vecs.y());
      if(vecs.y() > 0.) hisrz->Fill(vecs.z(),vecs.Perp());
      else hisrz->Fill(vecs.z(),-1.*vecs.Perp());

      hisDiffXY->Fill(vecdiff.x(),vecdiff.y());
      hisDiffRZ->Fill(vecdiff.z(),vecdiff.Perp());
      hisDiffX->Fill(vecdiff.x());
      hisDiffY->Fill(vecdiff.y());
      hisDiffZ->Fill(vecdiff.z());

      // --- Now move vectors to the local sensor system
      DetPlane plane = hit->getDetPlane();
      TVector3 vnorm = plane->getNormal(); //vnorm->SetMag(1.0);
      TVector3 plO = plane->getO();
      TVector3 plU = plane->getU(); //plU->SetMag(1.0);
      TVector3 plV = plane->getV(); //plV->SetMag(1.0);
      hisSensxy->Fill(plO.x(),plO.y());
      if(plO.y()>0.)hisSensrz->Fill(plO.z(),plO.Perp());
      else hisSensrz->Fill(plO.z(),-1.*plO.Perp());

      vecs -= plO;
      tmpz = vecs.Dot(plU);
      tmpx = vecs.Dot(plV);
      tmpy = vecs.Dot(vnorm);
      vecs.SetXYZ(tmpx,tmpy,tmpz);
      hisLocalXY->Fill(vecs.x(),vecs.y());
      hisLocalXZ->Fill(vecs.z(),vecs.x());
      hisLocalYZ->Fill(vecs.z(),vecs.y());
      hisLocalRZ->Fill(vecs.z(),vecs.Perp());

      vecmc -= plO;
      tmpz = vecmc.Dot(plU);
      tmpx = vecmc.Dot(plV);
      tmpy = vecmc.Dot(vnorm);
      vecmc.SetXYZ(tmpx,tmpy,tmpz);
      vecdiff = vecmc - vecs;

      tmpx = plU.Dot(plU);tmpy = plU.Dot(plV);tmpz = plU.Dot(vnorm);
      hisPLUV->Fill(tmpx);hisPLUV->Fill(tmpy);hisPLUV->Fill(tmpz);

      tmpx = plV.Dot(plU);tmpy = plV.Dot(plV);tmpz = plV.Dot(vnorm);
      hisPLUV->Fill(tmpx);hisPLUV->Fill(tmpy);hisPLUV->Fill(tmpz);

      tmpx = vnorm.Dot(plU);tmpy = vnorm.Dot(plV);tmpz = vnorm.Dot(vnorm);
      hisPLUV->Fill(tmpx);hisPLUV->Fill(tmpy);hisPLUV->Fill(tmpz);
     
      hisDiffLocalX->Fill(vecdiff.X());
      hisDiffLocalY->Fill(vecdiff.Y());
      hisDiffLocalZ->Fill(vecdiff.Z());
      hisDiffLocalXY->Fill(vecdiff.X(),vecdiff.Y());
      hisDiffLocalRZ->Fill(vecdiff.Z(),vecdiff.Perp());

    }//end for i (points in event)
  }// end for j (events)

  gStyle->SetOptFit();

   Double_t par[6];

   TF1 *g1    = new TF1("g1","gaus",-0.007,0.007);
   TF1 *g2    = new TF1("g2","gaus",-0.03,0.03);
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",-0.03,0.03);

   total->SetLineColor(kRed);
   total->SetLineWidth(1);
   g1->SetLineWidth(1);
   g2->SetLineWidth(1);

   hisDiffX->Fit(g1,"R");
   hisDiffX->Fit(g2,"R+");
   g1->GetParameters(&par[0]);
   g2->GetParameters(&par[3]);
   total->SetParameters(par);
   hisDiffX->Fit(total,"R");//"R+"

   hisDiffY->Fit(g1,"R");
   hisDiffY->Fit(g2,"R+");
   g1->GetParameters(&par[0]);
   g2->GetParameters(&par[3]);
   total->SetParameters(par);
   hisDiffY->Fit(total,"R");//"R+"

   hisDiffZ->Fit(g1,"R");
   hisDiffZ->Fit(g2,"R+");
   g1->GetParameters(&par[0]);
   g2->GetParameters(&par[3]);
   total->SetParameters(par);
   hisDiffZ->Fit(total,"R");//"R+"

   hisDiffLocalX->Fit("gaus");
   hisDiffLocalX->GetFunction("gaus")->SetLineColor(kRed);
   hisDiffLocalX->GetFunction("gaus")->SetLineWidth(1);

   hisDiffLocalY->Fit("gaus");
   hisDiffLocalY->GetFunction("gaus")->SetLineColor(kRed);
   hisDiffLocalY->GetFunction("gaus")->SetLineWidth(1);

   hisDiffLocalZ->Fit("gaus");
   hisDiffLocalZ->GetFunction("gaus")->SetLineColor(kRed);
   hisDiffLocalZ->GetFunction("gaus")->SetLineWidth(1);

Int_t a = 5, b = 4;

TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*250,b*250);
can1->Divide(a,b);

can1->cd(1);hisxy->DrawCopy("col");
can1->cd(2);hisrz->DrawCopy("col");
can1->cd(3);hisSensxy->DrawCopy("BOX");
can1->cd(4);hisSensrz->DrawCopy("BOX");
can1->cd(5);hisPLUV->DrawCopy();

can1->cd(6);hisDiffX->DrawCopy("");
can1->cd(7);hisDiffY->DrawCopy("");
can1->cd(8);hisDiffZ->DrawCopy("");
can1->cd(9);hisDiffXY->DrawCopy("col");
can1->cd(10);hisDiffRZ->DrawCopy("col");

can1->cd(11);hisDiffLocalX->DrawCopy("");
can1->cd(12);hisDiffLocalY->DrawCopy("");
can1->cd(13);hisDiffLocalZ->DrawCopy("");
can1->cd(14);hisDiffLocalXY->DrawCopy("col");
can1->cd(15);hisDiffLocalRZ->DrawCopy("col");

can1->cd(16);hisLocalXY->DrawCopy("col");
can1->cd(17);hisLocalXZ->DrawCopy("col");
can1->cd(18);hisLocalYZ->DrawCopy("col");
can1->cd(19);hisLocalRZ->DrawCopy("col");
can1->cd(20);


can1->Print("testoutput.ps");


  // -----   Finish   -------------------------------------------------------
  timer.Stop();
  Double_t rtime = timer.RealTime();
  Double_t ctime = timer.CpuTime();
  cout << endl << endl;
  cout << "Macro finished succesfully." << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
