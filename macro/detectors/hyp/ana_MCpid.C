// root macro to analyze the simulation output
//void convertMCPoints()
int ana_MCpid()
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libHypGe");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gStyle->SetPalette(1);

  TFile* f = new TFile("sim_pidC.root"); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("pndsim") ;
  TClonesArray* hit_array=new TClonesArray("PndHypPoint");
  t->SetBranchAddress("HypPoint",&hit_array);//Branch names

  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names



  // histos
  TH2D* hisxy = new TH2D("hisxy","HYP MC Points, xy view",200,-5.,5.,200,-5.,5.);
  TH2D* hisrz = new TH2D("hisrz","HYP MC Points, rz view",200,-80.,-70.,200,0,5.);
  TH1D* hisde = new TH1D("hisde","HYP MC Points, Energyloss",200,0.,0.002);


  TH2D* hisdedx =new TH2D("hisdedx","",200,0.,0.4,200,0.,1.0);
  hisdedx->SetTitle("HYP MC Points, dE/dx(P);p/GeV / cm;dE/dx / (GeV/cm)");



  TH1D* hismom = new TH1D("hismom","HYP MC Points, momentum",100,0.,1.5);


  int  nEvents = 100;
  bool verbose = false;

  TVector3 vecs,veco;
  TVector3 vecFront,vecBack,vecP;
  Double_t dx,dE,p,dEdX;

  TString detname;

  for (Int_t j=0; j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);

    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndHypPoint *hit=(PndHypPoint*)hit_array->At(i);
      int mcpdg = -1;

      //PndMCTrack *mctruth = (PndMCTrack*)mc_array->At(hit->GetTrackID());
      //mcpdg = mctruth->GetPdgCode();
      //cout<<"mcpdg="<<mcpdg<<endl;

      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      veco.SetXYZ(hit->GetXout(),hit->GetYout(),hit->GetZout());
      vecP.SetXYZ(hit->GetPxin(),hit->GetPyin(),hit->GetPzin());
      dx=(veco-vecs).Mag();
      dE=hit->GetEnergyLoss();
      p=vecP.Mag();
      Int_t layer = Int_t(10.*vecs->Mag());
      if(verbose) cout<<vecs.x()<<"\t"<<vecs.y()<<"\t"<<vecs.z()<<endl;

      hisxy->Fill(vecs.x(),vecs.y());
      hisrz->Fill(vecs.z(),vecs.Perp());
      hisde->Fill(dE);
      hismom->Fill(p);

      if(dx!=0)
      {
        dEdX=(dE/dx);
        hisdedx->Fill(p,dEdX);
      }


    }//end for i (points in event)
  }// end for j (events)

TCanvas* can1 = new TCanvas("can1","MCHit view in HYP",0,0,800,800);
can1->Divide(3,2);
can1->cd(1);
hisxy->Draw("colz");
can1->cd(2);
hisrz->Draw("colz");
can1->cd(3);
gPad->SetLogy();
hisde->Draw("");
can1->cd(4);
hisdedx->Draw();
can1->cd(5);
hismom->Draw("");


can1->Print("output.jpg");




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

  return 0;
}
