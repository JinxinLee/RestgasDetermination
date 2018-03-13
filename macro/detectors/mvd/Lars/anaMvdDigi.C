// root macro to analyze the clusterization output
{
  int  nEvents = 100;
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
  TTree *t=(TTree *) f->Get("pndsim") ;
  t->AddFriend("pndsim",digiFile.c_str()); // the digi file

  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* digiPixel_array=new TClonesArray("PndSdsDigiPixel");
  t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names

  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names

  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");


  // ---------  HISTOS ---------
  TH2D* hisxy = new TH2D("hisxy","",400,-15.,15.,400,-15.,15.);
  hisxy->SetTitle("MVD MC Point, xy view;x / cm;y / cm");

  TH2D* hisrz = new TH2D("hisrz","",400,-20.,20.,400,-15.,25.);
  hisrz->SetTitle("MVD MC Point, rz view;z / cm;r/ cm");

  TH1D* hisde = new TH1D("hisde","MVD MC Points, Energyloss",100,0.,0.002);

  TH1D* hismom = new TH1D("hismom","MVD MC Points, momentum",100,0.,1.5);

  int n = 100;  int low = 0;

  TH1I* hisPixelCol = new TH1I("hispixelcol","Pixel column channel number on FE",n,low,low+n);

  TH1I* hisPixelRow = new TH1I("hispixelrow","Pixel row channel number on FE",n,low,low+n);

  TH1I* hisPixelFE = new TH1I("hispixelfe","Pixel FE number",n,low,low+n);

  TH1I* hisCol = new TH1I("hiscol","column number",1001,0,1000);

  TH1I* hisRow = new TH1I("hisrow","row number",1001,0,1000);

  TH1D* hisPixelCharge = new TH1D("hispixelcharge","Pixel Charge content",100,0.,1e5);

  TH2I* hisStripTop = new TH2I("hisstriptop","Strip Top channel&fe numbers",20,0,20,130,0,130);

  TH2I* hisStripBot = new TH2I("hisstripbot","Strip Bot channel&fe numbers",20,0,20,130,0,130);

  TH1I* hisStripStrip = new TH1I("hisstripstrip","Strip numbers",15*128,0,15*128);

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
  PndMvdCalcFePixel pixelcalc(76, 84, 10);

  int nrFeChannels=par->GetNrFECh();
  int nrStrips=par->GetNrTopFE()*nrFeChannels;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;

    // ----- MC Points -----
    for (Int_t i=0; i<mc_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(i);
      vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
      mommc.SetXYZ(point->GetPx(),point->GetPy(),point->GetPz());
      hisxy->Fill(vecmc.x(),vecmc.y());
      if(vecmc.y() > 0.) hisrz->Fill(vecmc.z(),vecmc.Perp());
      else hisrz->Fill(vecmc.z(),-1.*vecmc.Perp());
      hisde->Fill(point->GetEnergyLoss());
      hismom->Fill(mommc.Mag());
    }

    // ----- PIXEL DIGIS -----
    for (Int_t i=0; i<digiPixel_array->GetEntriesFast(); i++)
    {
      PndSdsDigiPixel *pixeldigi = digiPixel_array->At(i);
      fe = pixeldigi->GetFE();
      col = pixeldigi->GetPixelColumn();
      row = pixeldigi->GetPixelRow();
      hisPixelCol->Fill(col);
      hisPixelRow->Fill(row);
      hisPixelFE->Fill(fe);
      hisPixelCharge->Fill(pixeldigi->GetCharge());
      pixelcalc.CalcSensorColRow(col,row,fe);
      hisCol->Fill(col);
      hisRow->Fill(row);
    }

    // ----- STRIP DIGIS -----
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
    }

  }// end for j (events)

Int_t a = 2, b = 2;

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

can1->cd(2);
mypad=gPad;
mypad.Divide(2,3);
mypad->cd(1);hisPixelCol->DrawCopy();
mypad->cd(2);hisPixelRow->DrawCopy();
mypad->cd(3);hisCol->DrawCopy();
mypad->cd(4);hisRow->DrawCopy();
mypad->cd(5);hisPixelFE->DrawCopy();
mypad->cd(6);hisPixelCharge->DrawCopy();


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
