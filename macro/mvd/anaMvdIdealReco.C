// root macro to analyze the clusterization output
{
  int  nEvents = 100;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("Libs.C");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  TFile* f = new TFile("Mvd_Test_reco.root"); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;
  TClonesArray* hit_array=new TClonesArray("PndMvdHit");
  t->SetBranchAddress("MVDHit",&hit_array);//Branch names

  TFile* F = new TFile("Mvd_Test.root"); // the sim file you want to analyse
  TTree *T=(TTree *) F->Get("cbmsim") ;
  TClonesArray* mc_array=new TClonesArray("PndMvdMCPoint");
  T->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TH2D* hisxy = new TH2D("hisxy","",400,-15.,15.,400,-15.,15.);
  hisxy->SetTitle("MVD MC Cluster, xy view;x / cm;y / cm");
  TH2D* hisrz = new TH2D("hisrz","",400,-20.,20.,400,-15.,25.);
  hisrz->SetTitle("MVD MC Cluster, rz view;z / cm;r/ cm");

  TH2D* hisDiffXY = new TH2D("hisdiffxy","",100,-0.05,0.05,100,-0.05,0.05);
  hisDiffXY->SetTitle("MC - RECO Hit coordinates xy view;#Deltax / cm;#Deltay / cm");
  TH2D* hisDiffRZ = new TH2D("hisdiffrz","",100,-0.05,0.05,100,-0.00,0.10);
  hisDiffRZ->SetTitle("MC - RECO Hit coordinates rz view;#Deltaz / cm;#Deltar / cm");

  TH1D* hisDiffX = new TH1D("hisdiffx","",100,-0.05,0.05);
  hisDiffX->SetTitle("MC - RECO Hit coordinate x;x / cm;");
  TH1D* hisDiffY = new TH1D("hisdiffy","",100,-0.05,0.05);
  hisDiffY->SetTitle("MC - RECO Hit coordinate y;y / cm;");
  TH1D* hisDiffZ = new TH1D("hisdiffz","",100,-0.05,0.05);
  hisDiffZ->SetTitle("MC - RECO Hit coordinate z;z / cm;");

  TVector3 vecs, vecmc, vecdiif;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    T->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndMvdHit *hit=(PndMvdHit*)hit_array->At(i);
      PndMvdMCPoint *point=(PndMvdMCPoint*)mc_array->At(i);
      int mcpdg = -1;

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
      vecdiff = vecmc - vecs;
      Int_t layer = Int_t(10.*vecs->Mag());
      if(verbose) cout<<vecs.x()<<"\t"<<vecs.y()<<"\t"<<vecs.z()<<endl;
      hisxy->Fill(vecs.x(),vecs.y());
      int vz=1; if(vecs.y()<0) vz=-1;
      hisrz->Fill(vecs.z(),vz*vecs.Perp());
      hisDiffXY->Fill(vecdiff.x(),vecdiff.y());
      hisDiffRZ->Fill(vecdiff.z(),vecdiff.Perp());
      hisDiffX->Fill(vecdiff.x());
      hisDiffY->Fill(vecdiff.y());
      hisDiffZ->Fill(vecdiff.z());
    }//end for i (points in event)
  }// end for j (events)

TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,800,800);
can1->Divide(3,3);
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
//   total->GetParameters(&par[0]);
//    for(int i=0;i<6;i++)
//    { std::cout<<"par["<<i<<"] = "<<par[i]<<std::endl;}
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



can1->cd(1);
hisxy->DrawCopy();
can1->cd(2);
hisrz->DrawCopy();

can1->cd(4);
hisDiffXY->DrawCopy("col");
can1->cd(5);
hisDiffRZ->DrawCopy("col");

can1->cd(7);
hisDiffX->DrawCopy("");
can1->cd(8);
hisDiffY->DrawCopy("");
can1->cd(9);
hisDiffZ->DrawCopy("");

// can1->Print("outAnaMvdIdealReco.ps");

delete hisxy;
delete hisrz;

delete hisDiffXY;
delete hisDiffRZ;

delete hisDiffX;
delete hisDiffY;
delete hisDiffZ;

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
