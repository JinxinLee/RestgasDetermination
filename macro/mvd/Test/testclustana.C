// root macro to analyze the clusterization output
{
  int  nEvents = 100;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/basiclibs.C");
  basiclibs();
  gSystem->Load("libGeoBase");
  gSystem->Load("libParBase");
  gSystem->Load("libBase");
  gSystem->Load("libPndBase");
  gSystem->Load("libField");
  gSystem->Load("libGen");
  gSystem->Load("libPassive");

  gSystem->Load("libgenfit");
  gSystem->Load("libtpc");
  gSystem->Load("libtpcreco");
  gSystem->Load("librecotasks");
  
  gSystem->Load("libMvd");
  gSystem->Load("libMvdReco");
  
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


	TFile* f = new TFile("testMCCluster.root"); // the sim file you want to analyse
	TTree *t=(TTree *) f->Get("cbmsim") ;
	TClonesArray* hit_array=new TClonesArray("MvdCluster");
	t->SetBranchAddress("MVDCluster",&hit_array);//Branch names

  TFile* F = new TFile("testMC.root"); // the sim file you want to analyse
  TTree *T=(TTree *) F->Get("cbmsim") ;
  TClonesArray* mc_array=new TClonesArray("MvdMCPoint");
  T->SetBranchAddress("MVDPoint",&mc_array);//Branch names

//	TClonesArray* mc_array=new TClonesArray("PndMCTrack");
//	t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TH2D* hisxy = new TH2D("hisxy","MVD MC Cluster, xy view",400,-15.,15.,400,-15.,15.);
  hisxy->SetXTitle("x / cm"); hisxy->SetYTitle("y / cm");
  TH2D* hisrz = new TH2D("hisrz","MVD MC Cluster, rz view",400,-20.,20.,400,-10.,30.);
  hisrz->SetXTitle("z / cm"); hisrz->SetYTitle("r/ cm");

  TH2D* hisDiffXY = new TH2D("hisdiffxy","MC - RECO Hit coordinates xy view",100,-0.05,0.05,100,-0.05,0.05);
  hisDiffXY->SetXTitle("#Delta x / cm"); hisDiffXY->SetYTitle("#Delta y / cm");
  TH2D* hisDiffRZ = new TH2D("hisdiffrz","MC - RECO Hit coordinates rz view",100,-0.05,0.05,100,-0.00,0.10);
  hisDiffRZ->SetXTitle("#Delta z / cm"); hisDiffRZ->SetYTitle("#Delta r / cm");

  TH1D* hisDiffX = new TH1D("hisdiffx","MC - RECO Hit coordinate x",100,-0.05,0.05);
  TH1D* hisDiffY = new TH1D("hisdiffy","MC - RECO Hit coordinate y",100,-0.05,0.05);
  TH1D* hisDiffZ = new TH1D("hisdiffz","MC - RECO Hit coordinate z",100,-0.05,0.05);

  TVector3 vecs, vecmc, vecdiif;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
	{
    t->GetEntry(j);
    T->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
	  for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
	  {
      if(verbose) cout<<"Point No "<<i<<endl;
		  PndMvdCluster *hit=(PndMvdCluster*)hit_array->At(i);
      PndMvdMCPoint *point=(PndMvdMCPoint*)mc_array->At(i);
      int mcpdg = -1;

//      PndMCTrack *mctruth = (PndMCTrack*)mc_array->At(hit->GetRefIndex());
//      mcpdg = mctruth->GetPdgCode();
//      cout<<"mcpdg="<<mcpdg<<endl;
      
      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
      vecdiff = vecmc - vecs;
		  Int_t layer = Int_t(10.*vecs->Mag());
      if(verbose) cout<<vecs.x()<<"\t"<<vecs.y()<<"\t"<<vecs.z()<<endl;
		  hisxy->Fill(vecs.x(),vecs.y());
		  hisrz->Fill(vecs.z(),vecs.Perp());
      hisDiffXY->Fill(vecdiff.x(),vecdiff.y());
      hisDiffRZ->Fill(vecdiff.z(),vecdiff.Perp());
      hisDiffX->Fill(vecdiff.x());
      hisDiffY->Fill(vecdiff.y());
      hisDiffZ->Fill(vecdiff.z());
	  }//end for i (points in event)
  }// end for j (events)

  gStyle->SetOptFit();
  hisDiffX->Fit("gaus");
  hisDiffY->Fit("gaus");
  hisDiffZ->Fit("gaus");

  hisDiffX->GetFunction("gaus")->SetLineColor(kRed);
  hisDiffX->GetFunction("gaus")->SetLineWidth(0.8);
  hisDiffY->GetFunction("gaus")->SetLineColor(kRed);
  hisDiffY->GetFunction("gaus")->SetLineWidth(0.8);
  hisDiffZ->GetFunction("gaus")->SetLineColor(kRed);
  hisDiffZ->GetFunction("gaus")->SetLineWidth(0.8);

TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,800,800);
can1->Divide(3,3);

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

can1->Print("testoutput.ps");

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
  //cout << "Output file is "    << outFile << endl;
  //cout << "Parameter file is " << parFile << endl;
  cout << "Real time " << rtime << " s, CPU time " << ctime << " s" << endl;
  cout << endl;
  // ------------------------------------------------------------------------

}
