/*
 * anaLmdReco.C * *  Created on: Jun 30, 2010 *      Author: huagen
 */

//*********************************************
// definition of Luminosity Monitor Detector
// the dimension of the LMD is 2cm*5.33cm*5cm*150um
// totally 16 wafers distrited on 4 planes
// 4 wafers located at up, down, left , right on each plane
//*************************************************************

{
//  int  nEvents = 2e4;
  bool verbose = false;

  // -----  Load libraries   ------------------------------------------------
  gROOT->Macro("$VMCWORKDIR/gconfig/rootlogon.C");
//  gROOT->Macro("Libs.C");

  // -----   Timer   --------------------------------------------------------
  TStopwatch timer;
  timer.Start();

  TFile* f = new TFile("/private/huagen/simdata/Lmd_Reco_DPM_elastic_6.2_1.9mrad_5M_2.root"); // the sim file
//  TFile* f = new TFile("/private/huagen/simdata/Lmd_Test_Reco.root");
  TTree *t=(TTree *) f->Get("cbmsim") ;
  TClonesArray* hit_array=new TClonesArray("PndSdsHit");
  t->SetBranchAddress("LMDHitsStrip",&hit_array);//Branch names

//  TFile* F = new TFile("/private/huagen/simdata/Lmd_Test.root"); // the sim file
  TFile* F = new TFile("/private/huagen/simdata/Lmd_DPM_elastic_6.2_1.9mrad_5M_2.root");
  TTree *T=(TTree *) F->Get("cbmsim") ;
  TClonesArray* mc_array=new TClonesArray("PndSdsMCPoint");
  T->SetBranchAddress("LMDPoint",&mc_array);//Branch names
/*
  TFile* digi = new TFile("/private/huagen/simdata/Lmd_Digi_DPM_elastic_6.2_2.72mrad_1M.root"); // the sim file
  TTree *Tdigi=(TTree *) digi->Get("cbmsim") ;
  TClonesArray* digi_array=new TClonesArray("PndSdsDigiStrip");
  Tdigi->SetBranchAddress("LMDStripDigis",&digi_array);//Branch names
*/
  TH3F *h2 = new TH3F("c2","Reconstructed Points", 200, -10, 10, 100, -10, 10, 100, 1070,1150);
  h2->SetFillColor(kRed);
  TH3F *h3 = new TH3F("c3","MC Points", 200, -10, 10, 100, -10, 10, 100, 1070,1150);
  h3->SetFillColor(kRed);

  TH2D* hisxy = new TH2D("hisxy","",200,-10.,10.,200,-10.,10.);
  hisxy->SetTitle("LMD Reconstructed points, xy view;x / cm;y / cm");
  TH2D* hisrz = new TH2D("hisrz","",800,1070.,1150.,100,-10.,10.);
  hisrz->SetTitle("LMD Reconstructed points, rz view;z / cm;r/ cm");

  //MC point: xy view and rz view
  TH2D* mchisxy = new TH2D("mchisxy","",200,-10.,10.,200,-10.,10.);
  mchisxy->SetTitle("LMD MC points, xy view;x / cm;y / cm");
  TH2D* mchisrz = new TH2D("mchisrz","",800,1070.,1150.,100,-10.,10.);
  mchisrz->SetTitle("LMD MC points, rz view;z / cm;r/ cm");

  TH2D* hisDiffXY = new TH2D("hisdiffxy","",100,-0.05,0.05,100,-0.05,0.05);
  hisDiffXY->SetTitle("MC - RECO Hit coordinates xy view;#Deltax / cm;#Deltay / cm");
  TH2D* hisDiffRZ = new TH2D("hisdiffrz","",100,-0.1.,0.1.,100,-0.05,0.05);
  hisDiffRZ->SetTitle("MC - RECO Hit coordinates rz view;#Deltaz / cm;#Deltar / cm");

  TH1F* hisDiffX = new TH1F("hisdiffx","",400,-0.02.,0.02.);
  hisDiffX->SetTitle("MC - RECO Hit coordinate x;x / cm;");
  TH1F* hisDiffY = new TH1F("hisdiffy","",400,-0.02.,0.02);
  hisDiffY->SetTitle("MC - RECO Hit coordinate y;y / cm;");
  TH1F* hisDiffZ = new TH1F("hisdiffz","",400,-0.02,0.02);
  hisDiffZ->SetTitle("MC - RECO Hit coordinate z;z / cm;");

  TH1D* RecoCharge = new TH1D("RecoCharge","",200,0.,1000);
  TH1D* DigiCharge = new TH1D("DigiCharge","",200,0.,1000);
  TH1D* MCCharge = new TH1D("MCCharge","",200,0.,1000);

  TVector3 vecs, vecmc, vecdiif;
  double mcX=0,mcY=0,mcZ=0;
  double reX=0,reY=0,reZ=0;
  double diffX=0,diffY=0,diffZ=0;
  TVector3 RecoPosition,MCposition,vecdiff;

  for (Int_t j=0; j<t->GetEntriesFast(); j++)
  {
    t->GetEntry(j);
    T->GetEntry(j);
   // Tdigi->GetEntry(j);
  //   if(verbose)
    if(j%1000==0)
    cout<<"**************Event No************ "<<j<<endl;
   // cout<<"The Digis of this event is "<<digi_array->GetEntriesFast()<<endl;
    for (Int_t i=0; i<hit_array->GetEntriesFast(); i++)
    {
   //    cout<<"Point No "<<i<<endl;

    	if(hit_array->GetEntriesFast()!= mc_array->GetEntriesFast())continue;
      PndSdsHit *hit=(PndSdsHit*)hit_array->At(i);
      PndSdsMCPoint *point=(PndSdsMCPoint*)mc_array->At(i);
      Int_t ID_hit = hit.GetSensorID();
      Int_t ID_mc  = point.GetSensorID();
      if(hit_array->GetEntriesFast()!= mc_array->GetEntriesFast())continue;
/*
      for(Int_t k=0;k<mc_array->GetEntriesFast();k++){
       PndSdsMCPoint *mcpoint=(PndSdsMCPoint*)mc_array->At(k);
       if (ID_hit == ID_mc)
         {   point=mcpoint;
       cout<<"*********correspongding det name in MC is "<<ID_mc<<endl;
         }
       }
*/
      int mcpdg = -1;
      mcX = (point->GetPosition().X()+point->GetPositionOut().X())/2;
      mcY = (point->GetPosition().Y()+point->GetPositionOut().Y())/2;
      mcZ = (point->GetPosition().Z()+point->GetPositionOut().Z())/2;

//      mcX = point->GetPosition().X();//+point->GetPositionOut().X())/2;
//      mcY = point->GetPosition().Y();//+point->GetPositionOut().Y())/2;
//      mcZ = point->GetPosition().Z();//+point->GetPositionOut().Z())/2;
      RecoPosition = hit->GetPosition();
      MCposition.SetXYZ(mcX,mcY,mcZ);
//      MCposition = point->GetPosition();
      vecdiff = MCposition - RecoPosition;

    //  Int_t layer = Int_t(10.*RecoPosition->Mag());
      RecoCharge->Fill(hit->GetEloss()*1000000);
      MCCharge->Fill(point->GetEnergyLoss()*1000000);

     //	 Int_t count=0;
    // 	 for(Int_t i=0;i<digi_array->GetEntriesFast();i++){
       //   PndSdsDigiStrip *digis = (PndSdsDigiStrip*)digi_array->At(i);
       //   Int_t FE=digis->GetFE();
      //    cout<<"the FE is:"<<FE<<endl;
     //     if(FE>10)count++;
      //    cout<<"the number of digis on backside is: "<<count<<endl;}
     	// if(count==2){

      h2->Fill(RecoPosition.x(),RecoPosition.y(),RecoPosition.z());
      h3->Fill(MCposition.x(),MCposition.y(),MCposition.z());

      hisxy->Fill(RecoPosition.x(),RecoPosition.y());
      mchisxy->Fill(mcX,mcY);
      int vz=1; if(RecoPosition.y()<0) vz=-1;
      hisrz->Fill(RecoPosition.z(),vz*RecoPosition.Perp());
      mchisrz->Fill(MCposition.z(),vz*MCposition.Perp());
    //  cout<<RecoPosition.Perp()<<endl;
      hisDiffXY->Fill(vecdiff.x(),vecdiff.y());
      hisDiffRZ->Fill(vecdiff.z(),vecdiff.Perp());
      hisDiffX->Fill(vecdiff.x());
      hisDiffY->Fill(vecdiff.y());
      hisDiffZ->Fill(vecdiff.z());

     //  }//end for i (points in event)
    // } loop with digi
   } //loop digis == 3
 }// end for j (events)

TCanvas* can1 = new TCanvas("LmdTestPlot","MCHit view in LMD",0,0,800,800);
 can1->Divide(3,3);
  gStyle->SetOptFit();
     Double_t par[6];

   TF1 *g1    = new TF1("g1","gaus",-0.004,0.003);
   TF1 *g2    = new TF1("g2","gaus",-0.006,0.006);
   TF1 *total = new TF1("total","gaus(0)+gaus(3)",-0.03,0.03);

   total->SetLineColor(kRed);
   total->SetLineWidth(1);
   g1->SetLineWidth(1);
   g2->SetLineWidth(1);

can1->cd(1);
hisxy->DrawCopy();
can1->cd(2);
hisrz->DrawCopy();

can1->cd(4);
mchisxy->DrawCopy();
//hisDiffXY->DrawCopy("col");
can1->cd(5);
//hisDiffRZ->DrawCopy("col");
mchisrz->DrawCopy();

can1->cd(7);
hisDiffX->DrawCopy("");
//hisDiffX->Fit("gaus");
////hisDiffX->Draw();
can1->cd(8);
hisDiffY->DrawCopy("");
can1->cd(9);
hisDiffZ->DrawCopy("");

can2 = new TCanvas("can2","xy-difference",0,0,600,900);
can2->Divide(1,2);
can2->cd(1);
hisDiffX->DrawCopy();
can2->cd(2);
hisDiffY->Fit(g1,"R");
hisDiffY->Fit(g2,"R+");
g1->GetParameters(&par[0]);
g2->GetParameters(&par[3]);
total->SetParameters(par);
//hisDiffY->Fit(total,"R");//"R+"
hisDiffY->Fit("gaus");
hisDiffY->DrawCopy();
//hisDiffY->Fit("gaus");

TCanvas* can4 = new TCanvas("MC_Digi_Reco_Charge","Charge",0,0,600,900);
 can4->Divide(1,2);
gStyle->SetOptFit();
can4->cd(1);
RecoCharge->DrawCopy();
can4->cd(2);
MCCharge->DrawCopy();

TCanvas* can5 = new TCanvas("MC_Reco_hits","Hits",0,0,600,900);
 can5->Divide(1,2);
gStyle->SetOptFit();
can5->cd(1);
h2->DrawCopy();
can5->cd(2);
h3->DrawCopy();

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


