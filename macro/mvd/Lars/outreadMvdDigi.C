// root macro to analyze the clusterization output
{
  int  nEvents = 20000;
  bool verbose = false;

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
  std::string digiFile = namecreator.GetDigiFileName(false);
  std::string parfile = "../data/Lars/MvdDtsParams.root";
  TString picture = namecreator.GetDigiFileName(false);
  picture.ReplaceAll(".root",".ps");

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;
  t->AddFriend("cbmsim",digiFile.c_str()); // the digi file

  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names

  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");
  
  std::ofstream Outfile("../data/Lars/0999_1001_85.hit",ios_base::app);
  std::ofstream Outrealfile("../data/Lars/0999_1001_85.real",ios_base::app);


  // ---------  HISTOS ---------
  TH2D* hisxy = new TH2D("hisxy","",100,-1.1,1.1,100,-1.1,1.1);
  hisxy->SetTitle("MVD MC Point, xy view;x / cm;y / cm");

  TH2D* hisrz = new TH2D("hisrz","",100,0.,10.,100,-5.,5.);
  hisrz->SetTitle("MVD MC Point, rz view;z / cm;r/ cm");

  TH1D* hisde = new TH1D("hisde","MVD MC Points, Energyloss",100,0.,0.002);

  TH1D* hismom = new TH1D("hismom","MVD MC Points, momentum",100,0.,1.5);

  int n = 100;  int low = 0;

  TH2I* hisStripTop = new TH2I("hisstriptop","Strip Top channel&fe numbers",5,0,4,130,0,130);

  TH2I* hisStripBot = new TH2I("hisstripbot","Strip Bot channel&fe numbers",5,0,4,130,0,130);

  TH1I* hisStripStrip = new TH1I("hisstripstrip","Strip numbers",3*128,0,3*128);

  TH1D* hisStripCharge = new TH1D("hisstripcharge","Strip Charge content",100,0.,1e5);

  TH1D* hisStripChargeTop = new TH1D("hisstripchargetop","Strip Charge content",100,0.,1e5);
  hisStripChargeTop->SetLineColor(kBlue);

  TH1D* hisStripChargeBot = new TH1D("hisstripchargebot","Strip Charge content",100,0.,1e5);
  hisStripChargeBot->SetLineColor(kRed);
  // ---------  HISTOS ---------


  TVector3 vecmc, mommc;
  Double_t tmpx,tmpy,tmpz;
  TVector2 locals, localmc, localdiff;
  int col, row, fe;
  double x,y;

  TFile* parDB = new TFile(parfile.c_str()); 
  PndMvdStripDigiPar* par = (PndMvdStripDigiPar*)gROOT->FindObject("MVDStripDigiParRect");

  int nrFeChannels=par->GetNrFECh();
  int nrStrips=par->GetNrTopFE()*nrFeChannels;

  int evID=0;
  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    evID++;
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;
    
    double x,y,z,Eloss;

    // ----- MC Points -----
    for (Int_t i=0; i<mc_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(i);
      vecmc = 0.5*(point->GetPosition() + point->GetPositionOut());
//       vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
      mommc.SetXYZ(point->GetPx(),point->GetPy(),point->GetPz());
      hisxy->Fill(vecmc.x(),vecmc.y());
      if(vecmc.y() > 0.) hisrz->Fill(vecmc.z(),vecmc.Perp());
      else hisrz->Fill(vecmc.z(),-1.*vecmc.Perp());
      hisde->Fill(point->GetEnergyLoss());
      hismom->Fill(mommc.Mag());
      x=point->GetX();
      y=point->GetY();
      z=point->GetZ();
      Eloss=point->GetEnergyLoss();
      Outrealfile<<evID<<" "<<x<<" "<<y<<" "<<z<<" "<<Eloss<<" "<<point->GetPx()<<" "<<point->GetPy()<<" "<<point->GetPz()<<endl;
    }

    // ----- STRIP DIGIS -----
    if(0==digiStrip_array->GetEntriesFast()) Outfile<<evID<<" "<<0<<" "<<0<<" "<<0.<<endl;
    for (Int_t i=0; i<digiStrip_array->GetEntriesFast(); i++)
    {
      PndSdsDigiStrip *stripdigi = digiStrip_array->At(i);
      fe = stripdigi->GetFE();
      col = stripdigi->GetChannel();
      int strip = fe * nrFeChannels + col;
      hisStripStrip->Fill(strip);
      hisStripCharge->Fill(stripdigi->GetCharge());
      if (strip <= nrStrips)
      {
        hisStripTop->Fill(fe,col);
        hisStripChargeTop->Fill(stripdigi->GetCharge());
      } else {
        hisStripBot->Fill(fe,col);
        hisStripChargeBot->Fill(stripdigi->GetCharge());
      }
      Outfile<<evID<<" "<<fe<<" "<<col<<" "<<stripdigi->GetCharge()<<endl;
     }
  }// end for j (events)

Int_t a = 2, b = 2;
Outrealfile.close();
Outfile.close();
TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*400,b*400);
can1->Divide(a,b);
TPad* mypad=0;

can1->cd(1);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisxy);
mypad->cd(2);DrawNice2DHisto(hisrz);
mypad->cd(3);gPad->SetLogy();hisde->DrawCopy();
mypad->cd(4);hismom->DrawCopy();

can1->cd(3);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisStripTop);
mypad->cd(2);DrawNice2DHisto(hisStripBot);
mypad->cd(3);hisStripStrip->DrawCopy();
mypad->cd(4);hisStripCharge->DrawCopy();
hisStripChargeTop->DrawCopy("sames");
hisStripChargeBot->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad; BetterStatBox(mypad);

can1->cd(4);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);
mypad->cd(2);
mypad->cd(3);
mypad->cd(4);

// can1->Update();
//can1->Print(picture.Data());


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
