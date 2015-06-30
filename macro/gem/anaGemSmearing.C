// root macro to analyze the clusterization output
void anaGemSmearing(int  nEvents = 10,  bool verbose = false)
{

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


  TFile* f = new TFile("Gem_Test_Smeared.root"); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;
  TClonesArray* hit_array=new TClonesArray("PndGemHit");
  t->SetBranchAddress("GEMHit",&hit_array);//Branch names

  TFile* F = new TFile("Gem_Test.root"); // the sim file you want to analyse
  TTree *T=(TTree *) F->Get("cbmsim") ;
  TClonesArray* mc_array=new TClonesArray("PndGemMCPoint");
  T->SetBranchAddress("GEMPoint",&mc_array);//Branch names

  TH2D* hisxy = new TH2D("hisxy","GEM smeared Hits, xy view",100,-150.,150.,100,-150.,150.);
  TH2D* hisrz = new TH2D("hisrz","GEM smeared Hits, rz view",100,0.,300.,100,-150.,150.);

  double rng = 0.5;
  TH2D* hisDiffXY = new TH2D("hisdiffxy","",100,-rng,rng,100,-rng,rng);
  hisDiffXY->SetTitle("MC - RECO Hit coordinates xy view;#Deltax / cm;#Deltay / cm");
  TH2D* hisDiffRZ = new TH2D("hisdiffrz","",100,-rng,rng,100,0.,rng);
  hisDiffRZ->SetTitle("MC - RECO Hit coordinates rz view;#Deltaz / cm;#Deltar / cm");

  TH1D* hisDiffX = new TH1D("hisdiffx","",100,-rng,rng);
  hisDiffX->SetTitle("MC - RECO Hit coordinate x;x / cm;");
  TH1D* hisDiffY = new TH1D("hisdiffy","",100,-rng,rng);
  hisDiffY->SetTitle("MC - RECO Hit coordinate y;y / cm;");
  TH1D* hisDiffZ = new TH1D("hisdiffz","",100,-rng,rng);
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
      PndGemHit *hit=(PndGemHit*)hit_array->At(i);
      PndGemMCPoint *point=(PndGemMCPoint*)mc_array->At(i);
      int mcpdg = -1;

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
      vecdiff = vecmc - vecs;
      Int_t layer = Int_t(10.*vecs.Mag());
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

TCanvas* can1 = new TCanvas("GemTestPlot","MCHit view in GEM",0,0,1200,600);
can1->Divide(4,2);


can1->cd(1);
DrawNice2DHisto(hisxy);
can1->cd(2);
DrawNice2DHisto(hisrz);

can1->cd(3);
DrawNice2DHisto(hisDiffXY);
can1->cd(4);
DrawNice2DHisto(hisDiffRZ);



   gStyle->SetOptFit();
   Double_t par[6];

   TF1 *g1    = new TF1("g1","gaus",-0.07,0.07);
   TF1 *g2    = new TF1("g2","gaus",-0.3,0.3);
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",-0.3,0.3);

   total->SetLineColor(kRed);
   total->SetLineWidth(1);
   g1->SetLineWidth(1);
   g2->SetLineWidth(1);

can1->cd(5);
   hisDiffX->Fit(g1,"R");
   hisDiffX->Fit(g2,"R+");
   g1->GetParameters(&par[0]);
   g2->GetParameters(&par[3]);
   total->SetParameters(par);
   hisDiffX->Fit(total,"R");//"R+"
hisDiffX->DrawCopy("");

can1->cd(6);
   hisDiffY->Fit(g1,"R");
   hisDiffY->Fit(g2,"R+");
   g1->GetParameters(&par[0]);
   g2->GetParameters(&par[3]);
   total->SetParameters(par);
   hisDiffY->Fit(total,"R");//"R+"
hisDiffY->DrawCopy("");

can1->cd(7);
   hisDiffZ->Fit(g1,"R");
   hisDiffZ->Fit(g2,"R+");
   g1->GetParameters(&par[0]);
   g2->GetParameters(&par[3]);
   total->SetParameters(par);
   hisDiffZ->Fit(total,"R");//"R+"
hisDiffZ->DrawCopy("");

// can1->Print("outAnaGemIdealReco.ps");
//
//delete hisxy;
//delete hisrz;
//
//delete hisDiffXY;
//delete hisDiffRZ;
//
//delete hisDiffX;
//delete hisDiffY;
//delete hisDiffZ;

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
