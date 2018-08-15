// root macro to analyze the simulation output
int ana_MCOpt()
{

  // -----  Load libraries   ------------------------------------------------
//``gSystem->Load("fstream.h");
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");

  gSystem->Load("libSciT");
  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------

  gStyle->SetPalette(1);

  TFile* f = new TFile("test.root"); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("pndsim") ;
  TClonesArray* hit_array=new TClonesArray("PndSciTPoint");
  t->SetBranchAddress("SciTPoint",&hit_array);//Branch names


  TClonesArray* mc_array=new TClonesArray("PndMCTrack");
  t->SetBranchAddress("MCTrack",&mc_array);//Branch names

   TFile *out = TFile::Open("pid_MCOpt.root","RECREATE");


  // histos
  TH2D* hisxy = new TH2D("hisxy","MC Points, xy view",200,-70.,70.,200,-70.,70.);
  TH2D* hisrz = new TH2D("hisrz"," MC Points, rz view",200,-70.,70.,200,0,70.);
  TH1D* hisde = new TH1D("hisde","MC Points, Energyloss",200,0.,0.005);


  TH2D* hisdedx =new TH2D("hisdedx","",200,0.,0.04,200,0.,1.0);
  hisdedx->SetTitle("HYP MC Points, dE/dx(P);p/GeV / cm;dE/dx / (GeV/cm)");




   TH1D* hismom = new TH1D("hisximom"," MC Points, momentum",100,0.,1.5);


  int  nEvents = 100;
  bool verbose = false;

  TVector3 vecs,veco;
  TVector3 vecFront,vecS,vecP;
  Double_t dx,dE,p,dEdX,pxi;

  TString detname;

  for (Int_t j=0; j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);

    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSciTPoint *hit=(PndSciTPoint*)hit_array->At(i);
      //int mcpdg = -1;



      vecs.SetXYZ(hit->GetX(), hit->GetY(), hit->GetZ());
      veco.SetXYZ(hit->GetXout(),hit->GetYout(),hit->GetZout());
      vecP.SetXYZ(hit->GetPxin(),hit->GetPyin(),hit->GetPzin());

      dx=(veco-vecs).Mag();
      dE=hit->GetEnergyLoss();
      pxi=vecP.Mag();


      hisxy->Fill(vecs.x(),vecs.y());
      hisrz->Fill(vecs.z(),vecs.Perp());
      hisde->Fill(dE);
      hismom->Fill(pxi);

      if(dx!=0)
      {
        dEdX=(dE/dx);
        hisdedx->Fill(p,dEdX);
        hisde->Fill(dEdX);
      }


    }//end for i (points in event)
  }// end for j (events)



out->cd();

hisxy->Write();
hisrz->Write();
hisdedx->Write();
hisde->Write();
hismom->Write();
out->Save();


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
