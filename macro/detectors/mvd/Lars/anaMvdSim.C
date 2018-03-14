// root macro to analyze the simulation output
//void convertMCPoints()
{

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("../Tools.C");
  LoadPandaStyle();
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  PndFileNameCreator namecreator("../data/Lars/MvdDtsSim.root");
  std::string inFile = namecreator.GetSimFileName(false);

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("pndsim") ;
  TClonesArray* hit_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&hit_array);//Branch names

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

  TH2D* hisxy = new TH2D("hisxy","MVD MC Points, xy view",100,-5.,5.,100,-5.,5.);
  TH2D* hisrz = new TH2D("hisrz","MVD MC Points, rz view",100,-5.,5.,100,-5.,5.);
  TH1D* hisde = new TH1D("hisde","MVD MC Points, Energyloss",100,0.,0.002);


  int  nEvents = 1000;
  bool verbose = false;

  TVector3 vecs,veco;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *hit=(PndSdsMCPoint*)hit_array->At(i);
      int mcpdg = -1;

      //PndMCTrack *mctruth = (PndMCTrack*)mc_array->At(hit->GetTrackID());
      //mcpdg = mctruth->GetPdgCode();
      //cout<<"mcpdg="<<mcpdg<<endl;

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      Int_t layer = Int_t(10.*vecs->Mag());
      if(verbose) cout<<vecs.x()<<"\t"<<vecs.y()<<"\t"<<vecs.z()<<endl;

      hisxy->Fill(vecs.x(),vecs.y());
      hisrz->Fill(vecs.z(),((vecs.y()>0.)?1.:-1.)*vecs.Perp());
      hisde->Fill(hit->GetEnergyLoss());
    }//end for i (points in event)
  }// end for j (events)

TCanvas* can1 = new TCanvas("can1","MCHit view in MVD",0,0,800,800);
can1->Divide(2,2);
can1->cd(1);
DrawNice2DHisto(hisxy);
can1->cd(2);
DrawNice2DHisto(hisrz);
can1->cd(3);
gPad->SetLogy();
hisde->DrawCopy("");
can1->cd(4);
can1->Print("outAnaMvdSim.ps");




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
