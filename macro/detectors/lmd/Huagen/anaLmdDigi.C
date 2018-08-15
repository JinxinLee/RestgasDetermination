// root macro to analyze the clusterization output
{
#include "TCanvas.h"
	std::cout;
  int  nEvents = 1e4;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
  gROOT->LoadMacro("Tools.C");
  rootlogon();
 // gSystem->Load("libLmd");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();
  // ------------------------------------------------------------------------
  std::string inFile = "/private/huagen/simdata/Lmd_DPM_elastic_6.2_1.9mrad_5M_1.root";
  std::string digiFile = "/private/huagen/simdata/Lmd_Digi_DPM_elastic_6.2_1.9mrad_5M_1.root";
  std::string parfile = "/private/huagen/simdata/LmdParams.root";

  TFile* Digis = new TFile(digiFile.c_str(),"READ"); // the sim file you want to analyse
  TTree* tree=(TTree *) Digis->Get("pndsim") ;

  TFile* MCPoint = new TFile(inFile.c_str(),"READ");
  TTree* tree2 = (TTree*)MCPoint->Get("pndsim");
 // tree->AddFriend("pndsim",inFile.c_str()); // the digi file

  TClonesArray* point=new TClonesArray("PndSdsMCPoint");
  tree2->SetBranchAddress("LMDPoint",&point);//Branch names

  TClonesArray* digiStrip_array=new TClonesArray("PndSdsDigiStrip");
  tree->SetBranchAddress("LMDStripDigis",&digiStrip_array);//Branch names

//  TGeoManager *geoMan = (TGeoManager*) gDirectory->Get("FAIRGeom");

  // ---------  HISTOS ---------
  TH2D* hisxy = new TH2D("hisxy","",100,-10.,10.,100,-10.,10.);
  hisxy->SetTitle("LMD MC Points, X vs Y view;X (cm);X (cm)");

  TH2D* hisrz = new TH2D("hisrz","",100,1070.,1150.,100,1070.,1150.);
  hisrz->SetTitle("LMD MC Point, rz view;z / cm;r/ cm");

  TH1D* hisde = new TH1D("hisde","LMD MC Points, Energyloss",1000,0.,1000);
  hisde->SetTitle("Lmd MC Energy Loss; X / keV ; Y / count");
//  TH1D* hismom = new TH1D("hismom","LMD MC Points, momentum",100,3.,10.);

  TH2I* hisStripTop = new TH2I("StripFront","Strip Front channel & front end numbers",20,0,40,130,0,130);
  hisStripTop->SetTitle("FE numbers vs Channels counts; X / FE; Y / channel");
  TH2I* hisStripBot = new TH2I("StripBack","Strip Back channel & front end numbers",20,0,40,130,0,130);
  hisStripBot->SetTitle("FE numbers vs Channels counts; X / FE; Y / channel");

  TH1I* hisStripStrip = new TH1I("hisstripstrip","Strip numbers",2330,-10,4650);
  hisStripStrip->SetTitle("Strips on Front/Back side; X / channels; Y / count");

  TH1D* hisStripCharge = new TH1D("StripChargeTotal","Strip Charge content",1000,0.,2.5e5);

  TH1D* hisStripChargeTop = new TH1D("StripChargeFront","Strip Charge content",1000,0.,2.5e5);
  hisStripChargeTop->SetLineColor(kBlue);
  TH1D* hisStripChargeBot = new TH1D("StripChargeBack","Strip Charge content",1000,0.,2.5e5);
  hisStripChargeBot->SetLineColor(kRed);

  // ---------  HISTOS ---------
  TVector3 vecmc, mommc;
  Double_t tmpx,tmpy,tmpz;
  TVector2 locals, localmc, localdiff;
  int col, row, fe;
  double x,y,X=0,Y=0,Z=0,de=0;

  TFile* parDB = new TFile(parfile.c_str());
  PndSdsStripDigiPar* par = (PndSdsStripDigiPar*)gROOT->FindObject("LmdStripDigiParTrap");

//  Int_t nrFeChannels = par->GetNrFECh();
//  Int_t nrStrips = par->GetNrFrontFE()*nrFeChannels;
  int nrFeChannels = 128;//par->GetNrFECh();
  int nrStrips = 1280;
  if(verbose) cout<<"the nrFeChannels and Strips are :"<< nrFeChannels<<","<<nrStrips<<endl;

  for (Int_t j=0; j<nEvents&&j<tree->GetEntriesFast(); j++)
  {
	if(verbose) std::cout<< "the number of "<< j <<"event is been processed"<<std::endl;
     tree->GetEntry(j);
     tree2->GetEntry(j);
    if(verbose)    	cout<<"Event No "<<j<<endl;

    // ----- MC Points -----
    for (Int_t i=0; i<point->GetEntriesFast(); i++)
    {
    	if(verbose)    cout<<"Point No "<<i<<endl;
      PndSdsMCPoint *Mypoint=(PndSdsMCPoint*)point->At(i);
	  X = Mypoint->GetPosition().X();
	  Y = Mypoint->GetPosition().Y();
	  Z = Mypoint->GetPosition().Z();
	  hisxy->Fill(X,Y);
      de = Mypoint->GetEnergyLoss()*1000000;
     // de = Mypoint->GetEnergyLoss();
      hisde->Fill(de);
      if(verbose) cout<<"the energy loss is "<<de<<endl;
    }

    // ----- STRIP DIGIS -----
    for (Int_t i=0; i<digiStrip_array->GetEntriesFast(); i++)
    {
      PndSdsDigiStrip *stripdigi = digiStrip_array->At(i);
      fe = stripdigi->GetFE();
      col = stripdigi->GetChannel();
  //    std::cout<<"the fe and channel are:"<<fe<<","<<col<<std::endl;

      int strip = fe * nrFeChannels + col;
      hisStripStrip->Fill(strip);
      hisStripCharge->Fill(stripdigi->GetCharge());
  //    std::cout<<"the strip charge is :"<<stripdigi->GetCharge()<<std::endl;

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

Int_t a = 2, b = 3;

TCanvas* can1 = new TCanvas("Lmd MC point & Digis Plot","MCHit view in LMD",0,0,2*400,2*400);
can1->Divide(a,b);
TPad* mypad=0;

can1->cd(1);
mypad=gPad;
//mypad.Divide(2,1);
mypad->cd(1);
DrawNice2DHisto(hisxy);
can1->cd(2);
gPad->SetLogy();
hisde->DrawCopy();

can1->cd(3);
mypad=gPad;
//mypad.Divide(2,2);
//hisStripTop->DrawCopy();
DrawNice2DHisto(hisStripTop);
can1->cd(4); //hisStripBot->DrawCopy();
DrawNice2DHisto(hisStripBot);
can1->cd(5);hisStripStrip->DrawCopy();
can1->cd(6);//gPad->SetLogy();
hisStripCharge->DrawCopy();
hisStripChargeTop->DrawCopy("sames");
hisStripChargeBot->DrawCopy("sames");
can1->Update(); mypad = (TPad*)gPad; BetterStatBox(mypad);

/*
TCanvas* can2 = new TCanvas("Lmd MC energy & Digis charge","Eloss vs Digi Charge",0,0,300,600);
can2->Divide(1,2);
can2->cd(1);
gPad->SetLogy();
hisde->DrawCopy();
can2->cd(2);
gPad->SetLogy();
hisStripChargeTop->DrawCopy();
//hisStripCharge->DrawCopy();
*/
//can1->cd(3);
//mypad=gPad;
//mypad.Divide(1,2);
//mypad->cd(1);
//mypad->cd(2);
//mypad->cd(3);
//mypad->cd(4);

// can1->Update();
//can1->Print(picture.Data());
//can2->Print(picture.Data());

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
