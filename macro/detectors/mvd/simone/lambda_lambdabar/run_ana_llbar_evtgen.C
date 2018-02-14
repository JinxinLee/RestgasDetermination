// root macro to analyze the clusterization output
{
  int  nEvents = 10000;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->LoadMacro("$VMCWORKDIR/gconfig/rootlogon.C");
  rootlogon();
  gROOT->LoadMacro("../Tools.C");
  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------


//  PndFileNameCreator namecreator("sim_complete.root");
//  std::string inFile = namecreator.GetSimFileName(false);
//  std::string digiFile = namecreator.GetDigiFileName(false);
//  std::string parfile = "../data/mvddpm6GeV_digipar.root";
//  TString picture = namecreator.GetDigiFileName(false);

  std::string inFile = "llbar_sim_evtgen.root";
  std::string digiFile = "llbar_digi_evtgen.root";
  std::string recoFile = "llbar_reco_evtgen.root";
  TString picture = "digi_complete.root";

  picture.ReplaceAll(".root",".ps");

  TFile* f = new TFile(inFile.c_str()); // the sim file you want to analyse
  TTree *t=(TTree *) f->Get("cbmsim") ;
  t->AddFriend("cbmsim",digiFile.c_str()); // the digi file
  t->AddFriend("cbmsim",recoFile.c_str()); // the reco file

  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  t->SetBranchAddress("MVDPoint",&mc_array);//Branch names

  TClonesArray* digiPixel_array=new TClonesArray("PndSdsDigiPixel");
   t->SetBranchAddress("MVDPixelDigis",&digiPixel_array);//Branch names

   TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
   t->SetBranchAddress("MVDStripDigis", &digiStrip_array);//Branch names

  //  TClonesArray* recoPixel_array=new TClonesArray("PndSdsDigiPixel");
  //  t->SetBranchAddress("MVDDigisCorr",&recoPixel_array);//Branch names

//  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
//  t->SetBranchAddress("MVDStripDigis",&digiStrip_array);//Branch names

//  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");


  // ---------  HISTOS ---------

   TH1I* hNumberTrackPointsLambda = new TH1I("hNumberTrackPointsLambda","", 10, -0.5, 9.5);
   TH1I* hNumberTrackPointsLambdaBar = new TH1I("hNumberTrackPointsLambdaBar","", 10, -0.5, 9.5);


/*  TH2D* hisxy = new TH2D("hisxy","",400,-15.,15.,400,-15.,15.);
  hisxy->SetTitle("MVD MC Point, xy view;x / cm;y / cm");

  TH2D* hisrz = new TH2D("hisrz","",400,-20.,20.,400,-15.,25.);
  hisrz->SetTitle("MVD MC Point, rz view;z / cm;r/ cm");

  TH1D* hisde = new TH1D("hisde","MVD MC Points, Energyloss",100,0.,0.002);

  TH1D* hismom = new TH1D("hismom","MVD MC Points, momentum",100,0.,1.5);

  int n = 100;  int low = 0;

  TH1I* hisPixelCol = new TH1I("hispixelcol","Pixel column channel number on FE",n,low,low+n);

  TH1I* hisPixelRow = new TH1I("hispixelrow","Pixel row channel number on FE",n,low,low+n);

  TH1I* hisPixelFE = new TH1I("hispixelfe","Pixel FE number",n,low,low+n);

  TH1I* hisCol = new TH1I("hiscol","column number",1200,0,1200);

  TH1I* hisRow = new TH1I("hisrow","row number",1200,0,1200);

  TH1D* hisPixelCharge = new TH1D("hispixelcharge","Pixel Charge content",100,0.,1e5);

  TH2I* hisStripTop = new TH2I("hisstriptop","Strip Top channel&fe numbers",20,0,20,130,0,130);

  TH2I* hisStripBot = new TH2I("hisstripbot","Strip Bot channel&fe numbers",20,0,20,130,0,130);

  TH1I* hisStripStrip = new TH1I("hisstripstrip","Strip numbers",15*128,0,15*128);

  TH1D* hisStripCharge = new TH1D("hisstripcharge","Strip Charge content",100,0.,1e5);

  TH1D* hisStripChargeTop = new TH1D("hisstripchargetop","Strip Charge content",100,0.,1e5);
  hisStripChargeTop->SetLineColor(kBlue);

  TH1D* hisStripChargeBot = new TH1D("hisstripchargebot","Strip Charge content",100,0.,1e5);

  TH2F* hisDiffTimeWalk = new TH2F("hisDiffTimeWalk","Differenz digiTimeWalk and RecoTimeWalk", 400, 0.,200,400,0.,200  );

 // TH1F* fHChgDiff = new TH1F("hchgdiff","#color[2]{StripMC}, #color[6]{PixelMC},\#color[4]{StripNoise}, #color[30]{PixelNoise}, #color[1]{All};C/e^{-};	"
*/
		  //----- AnaTask -----

 //  PndMvdEventAnaTask* anatask = new PndMvdEventAnaTask();
 //  fRun->AddTask(anatask);",150,0.,1e4);
  TH1F* fHChgMC = new TH1F("hchgmc",";#DeltaC/e^{-} MC;",150,0.,1e4);
  TH1F* fHChgFake = new TH1F("hchgfake",";#DeltaC/e^{-} fake;",150,0.,1e4);
  TH1F* fHChgMCPix = new TH1F("hchgmcPix",";#DeltaC/e^{-} MC;",150,0.,1e4);
  TH1F* fHChgFakePix = new TH1F("hchgfakePix",";#DeltaC/e^{-} fake;",150,0.,1e4);

  hisStripChargeBot->SetLineColor(kRed);
  // ---------  HISTOS ---------


  TVector3 vecmc, mommc;
  Double_t tmpx,tmpy,tmpz;
  TVector2 locals, localmc, localdiff;
  int col, row, fe;
  double x,y;

//  TFile* parDB = new TFile(parfile.c_str());
//  PndMvdStripDigiPar* par = (PndMvdStripDigiPar*)gROOT->FindObject("MVDStripDigiParRect");
//  PndMvdCalcFePixel pixelcalc(100, 100, 10);

//  int nrFeChannels=par->GetNrFECh();
//  int nrStrips=par->GetNrTopFE()*nrFeChannels;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;

    // ----- MC Points -----
    for (Int_t i=0; i<mc_array->GetEntriesFast(); i++)
    {
      if(verbose) cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(i);
     // vecmc.SetXYZ(point->GetX(),point->GetY(),point->GetZ());
     // mommc.SetXYZ(point->GetPx(),point->GetPy(),point->GetPz());
     // hisxy->Fill(vecmc.x(),vecmc.y());
     // if(vecmc.y() > 0.) hisrz->Fill(vecmc.z(),vecmc.Perp());
     // else hisrz->Fill(vecmc.z(),-1.*vecmc.Perp());
     // hisde->Fill(point->GetEnergyLoss());
     // hismom->Fill(mommc.Mag());
    }

    // ----- PIXEL DIGIS -----
    for (Int_t i=0; i<digiPixel_array->GetEntriesFast(); i++)
    {
      PndSdsDigiPixel *pixeldigi = (PndSdsDigiPixel*) digiPixel_array->At(i);
//      PndSdsDigiPixel *pixelreco = (PndSdsDigiPixel*) recoPixel_array->At(i);
      fe = pixeldigi->GetFE();
      col = pixeldigi->GetPixelColumn();
      row = pixeldigi->GetPixelRow();
 //     hisPixelCol->Fill(col);
//      hisPixelRow->Fill(row);
//      hisPixelFE->Fill(fe);
 //     hisPixelCharge->Fill(pixeldigi->GetCharge());
 //     pixelcalc.CalcSensorColRow(col,row,fe);
 //     hisCol->Fill(col);
  //    hisRow->Fill(row);
  //    hisDiffTimeWalk->Fill(pixelreco->GetTimeWalk(),pixeldigi->GetTimeWalk());

     // printf("reco TimeWalk %d -  digi TimeWalk %d \n",pixelreco->GetTimeWalk(),pixeldigi->GetTimeWalk());
   //   fHChgDiff->Fill(pixeldigi->GetCharge());
 //     if( pixeldigi->GetIndex() == -1 ) fH//
  //  		  R. Kliemt
    		  // ----- STRIP DIGIS -----
    		  //    for (Int_t i=0; i<digiStrip_array->GetEntriesFast(); i++)
    		  //    {
    		  //      PndSdsDigiStrip *stripdigi = digiStrip_array->At(i);
    		  //      fe = stripdigi->GetFE();
    		  //      col = stripdigi->GetChannel();
    		  //      int strip = fe * nrFeChannels + col;
    		  //      hisStripStrip->Fill(strip);
    		  //      hisStripCharge->Fill(stripdigi->GetCharge());
    		  //      if (strip <= nrStrips)
    		  //      {
    		  //        hisStripTop->Fill(fe,col);
    		  //        hisStripChargeTop->Fill(stripdigi->GetCharge());
    		  //      } else {
    		  //        hisStripBot->Fill(fe,col);
    		  //        hisStripChargeBot->Fill(stripdigi->GetCharge());
    		  //      }
    		  //      fHChgDiff->Fill(stripdigi->GetCharge());
    		  //      if( stripdigi->GetIndex() == -1 ) fHChgFake->Fill(stripdigi->GetCharge());
    		  //      else fHChgMC->Fill(stripdigi->GetCharge());
    		  //    }ChgFakePix->Fill(pixeldigi->GetCharge());
    //  else fHChgMCPix->Fill(pixeldigi->GetCharge());
//    }

    // ----- STRIP DIGIS -----
    for (Int_t i=0; i<digiStrip_array->GetEntriesFast(); i++)
    {
      PndSdsDigiStrip *stripdigi = digiStrip_array->At(i);
//      fe = stripdigi->GetFE();
//      col = stripdigi->GetChannel();
//      int strip = fe * nrFeChannels + col;
//      hisStripStrip->Fill(strip);
//      hisStripCharge->Fill(stripdigi->GetCharge());
//      if (strip <= nrStrips)
//      {
//        hisStripTop->Fill(fe,col);
//        hisStripChargeTop->Fill(stripdigi->GetCharge());
//      } else {
//        hisStripBot->Fill(fe,col);
//        hisStripChargeBot->Fill(stripdigi->GetCharge());
//      }
//      fHChgDiff->Fill(stripdigi->GetCharge());
//      if( stripdigi->GetIndex() == -1 ) fHChgFake->Fill(stripdigi->GetCharge());
//      else fHChgMC->Fill(stripdigi->GetCharge());
    }

  }// end for j (events)
   gROOT->SetStyle("Plain");

//      gStyle->SetTextFont(22);
//      gStyle->SetLabelFont(22,"x");
//      gStyle->SetLabelFont(22,"y");
//      gStyle->SetLabelFont(22,"z");

Int_t a = 2, b = 2, res=475;
TCanvas* can1 = new TCanvas("MvdTestPlot","MCHit view in MVD",0,0,a*res,b*res);
can1->Divide(a,b);
TPad* mypad=0;

//can1->cd(1);
//mypad=gPad;
//mypad.Divide(2,2);
//mypad->cd(1);DrawNice2DHisto(hisxy);
//mypad->cd(2);DrawNice2DHisto(hisrz);
//mypad->cd(3);gPad->SetLogy();hisde->DrawCopy();
//mypad->cd(4);hismom->DrawCopy();

can1->cd(2);
//hisDiffTimeWalk->DrawCopy();
//DrawNice2DHisto(hisDiffTimeWalk);
//gPad->SetLogy();
////fHChgDiff->DrawCopy();
//fHChgMC->SetLineColor(kRed);
//fHChgMC->DrawCopy("same");
//fHChgFake->SetLineColor(kBlue);
//fHChgFake->DrawCopy("same");
//fHChgMCPix->SetLineColor(6);
//fHChgMCPix->DrawCopy("same");
//fHChgFakePix->SetLineColor(30);
//fHChgFakePix->DrawCopy("same");


//can1->cd(3);
//mypad=gPad;
//mypad.Divide(2,2);
//mypad->cd(1);DrawNice2DHisto(hisStripTop);
//mypad->cd(2);DrawNice2DHisto(hisStripBot);
//mypad->cd(3);hisStripStrip->DrawCopy();
//mypad->cd(4);gPad->SetLogy();
//hisStripCharge->DrawCopy();
//hisStripChargeTop->DrawCopy("sames");
//hisStripChargeBot->DrawCopy("sames");
//can1->Update();mypad=(TPad*)gPad; BetterStatBox(mypad);

//can1->cd(4);
//mypad=gPad;
//mypad.Divide(2,3);
//mypad->cd(1);hisPixelCol->DrawCopy();
//mypad->cd(2);hisPixelRow->DrawCopy();
//mypad->cd(3);hisCol->DrawCopy();
//mypad->cd(4);hisRow->DrawCopy();
//mypad->cd(5);hisPixelFE->DrawCopy();
//mypad->cd(6);gPad->SetLogy();hisPixelCharge->DrawCopy();

// can1->Update();
can1->Print(picture.Data());


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
