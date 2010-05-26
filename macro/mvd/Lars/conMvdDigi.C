// root macro to analyze the clusterization output
{
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("$VMCWORKDIR/macro/mvd/Tools.C");
  LoadPandaStyle();

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  std::string HitFileName="data/testhits.hit";
  std::string CalibFileName="calib.par";
  std::string GeoFileName="teststation.root";
  std::string SenParName="mvd.ddteststation.par";

  PndMvdConvertApv* ApvConverter= new PndMvdConvertApv(CalibFileName, HitFileName);
  int  nEvents = ApvConverter->GetNofEvents();
//   if (nEvents > 1000) nEvents = 1000;

  FairRunAna *fRun= new FairRunAna();

  FairRuntimeDb* rtdb = fRun->GetRuntimeDb();
  FairParAsciiFileIo* parInput2 = new FairParAsciiFileIo();
  parInput2->open(SenParName.c_str(),"in");
  rtdb->setFirstInput(parInput2);

  fRun->SetGeomFile(GeoFileName.c_str());
  fRun->SetOutputFile("data/cosmic.root");
  
//---- Set Converter
  PndMvdConvertApvTask* ApvTask = new PndMvdConvertApvTask(ApvConverter);
  fRun->AddTask(ApvTask);
//----- Set Cluster

 //  PndMvdStripClusterTask* mvdClusterizer = new PndMvdStripClusterTask();
 //  fRun->AddTask(mvdClusterizer);

  fRun->LoadGeometry();
// ---- start run
  fRun->Init();
  fRun->Run(0, nEvents);

/*
  // ---------  HISTOS ---------
  int n = 100;  int low = 0;;

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

  int nrFeChannels=3;
  int nrStrips=3*128;

  for (Int_t j=0; j<nEvents && j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    if(verbose) cout<<"Event No "<<j<<endl;

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

Int_t a = 1, b = 1;

TCanvas* can1 = new TCanvas("MvdTestPlot","real Hit view",0,0,a*400,b*400);
can1->Divide(a,b);
TPad* mypad=0;

can1->cd(1);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);DrawNice2DHisto(hisStripTop);
mypad->cd(2);DrawNice2DHisto(hisStripBot);
mypad->cd(3);hisStripStrip->DrawCopy();
mypad->cd(4);hisStripCharge->DrawCopy();
hisStripChargeTop->DrawCopy("sames");
hisStripChargeBot->DrawCopy("sames");
can1->Update();mypad=(TPad*)gPad; BetterStatBox(mypad);

can1->cd(2);
mypad=gPad;
mypad.Divide(2,2);
mypad->cd(1);
mypad->cd(2);
mypad->cd(3);
mypad->cd(4);

// can1->Update();
//can1->Print(picture.Data());*/


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
