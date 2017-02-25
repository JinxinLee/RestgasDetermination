#include <stdio.h>
#include <stdlib.h>
#include <vector> 
#include "TF1.h"
#include "MyClass.h"

int analysis_cluster_energyCorrection_Ntuple_suplement()
{
  TStopwatch timer;
  timer.Start();

  ////////////////////////////////////// reading the correction factors from the root file gamma_en_th_corr_TGeant3.root of Johan in /opt/fairroot/pandaroot/macro/params
  cf=new TFile("/opt/fairroot/pandaroot/macro/params/gamma_en_th_corr_TGeant3.root");//the histogram hisEnergyDelta has 26 bins in energy and 61 in theta
  TH2F *HistClusterEnergyCorrectionFactor_johan = (TH2F*)hisEnergyDelta->Clone();//making a copy of the histogram hisEnergyDelta
  HistClusterEnergyCorrectionFactor_johan->SetName("HistClusterEnergyCorrectionFactor_johan");//renaming the new one is recommended
  for (int i=1; i<=26; i++)//cluster energy
    for (int j=1; j<=61; j++)//cluster theta
      HistClusterEnergyCorrectionFactor_johan->SetBinContent(i,j,0);//emptying the histogram to be filled later in this code; this would have the same binning as of Johan map!

  TH2F *HistClusterEnergyCorrectionFactor_johan_original = (TH2F*)hisEnergyDelta->Clone();//making another copy
  HistClusterEnergyCorrectionFactor_johan_original->SetName("HistClusterEnergyCorrectionFactor_johan_original");
  for (int i=1; i<=26; i++)//cluster energy
    for (int j=1; j<=61; j++)//cluster theta
      HistClusterEnergyCorrectionFactor_johan_original->SetBinContent(i,j,0);//emptying the histogram
  
  TH2F *HistClusterEnergyCorrectionFactor_johan_count = (TH2F*)hisEnergyDelta->Clone();//making another copy to keep the bin contents
  HistClusterEnergyCorrectionFactor_johan_count->SetName("HistClusterEnergyCorrectionFactor_johan_count");
   for (int i=1; i<=26; i++)//cluster energy
    for (int j=1; j<=61; j++)//cluster theta
      HistClusterEnergyCorrectionFactor_johan_count->SetBinContent(i,j,0);//emptying the histogram
  
  ////////////////////////////////////// making my own variable bin size histogram
  Float_t xlowarray[36] = {0, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.6, 0.65, 0.7, 0.75, 0.8, 0.85, 0.9, 1.,
			    1.05, 1.1, 1.15, 1.2, 1.25, 1.3, 1.35, 1.4, 1.45, 1.5, 1.5, 2., 2.5, 3., 3.5, 4., 4.5, 5.}; //energy bin edges (GeV)
  Float_t ylowarray[54] = {0, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 20., 21., 22., 23., 24., 25., 30., 35., 40., 45., 50., 55., 60., 
			  65., 70., 75., 80., 85., 90., 95., 100., 105., 110., 115., 120., 125., 130., 135., 140., 142., 147., 152.,
			  154., 156., 158., 161., 165., 171.};//theta bin edges
  TH2F *HistClusterEnergyCorrectionFactor_Hossein = new TH2F("HistClusterEnergyCorrectionFactor_Hossein","", 35, xlowarray, 53, ylowarray);
  TH2F *HistClusterEnergyCorrectionFactor_Hossein_original = new TH2F("HistClusterEnergyCorrectionFactor_Hossein_original","", 35, xlowarray, 53, ylowarray);
  TH2F *HistClusterEnergyCorrectionFactor_Hossein_count = new TH2F("HistClusterEnergyCorrectionFactor_Hossein_count","", 35, xlowarray, 53, ylowarray);

   TH1I *HISTnumberOfClusters_thresholdPassed = new TH1I("HISTnumberOfClusters_thresholdPassed","Total number of clusters per event satisfying cluster threshold",140,0,70);
   
   TH2F *HistClusterEnergyCorrectionFactor_original = new TH2F("HistClusterEnergyCorrectionFactor_original","cluster energy correction factor",180, 0, 180, 360, -180, 180);//theta, phi
   TH2F *HistClusterEnergyCorrectionFactor_count = new TH2F("HistClusterEnergyCorrectionFactor_count","cluster energy correction factor (bin content container)",180, 0, 180, 360, -180, 180);
   TH2F *HistClusterEnergyCorrectionFactor = new TH2F("HistClusterEnergyCorrectionFactor","cluster energy correction factor (normalized)",180, 0, 180, 360, -180, 180);
   
   TH2F *HistClusterEnergyCorrectionFactor_theta_Egamma_original = new TH2F("HistClusterEnergyCorrectionFactor_theta_Egamma_original","cluster energy correction factor (normalized)",500, 0, 5., 80, 0, 180);//energy in GeV
   TH2F *HistClusterEnergyCorrectionFactor_theta_Egamma_count = new TH2F("HistClusterEnergyCorrectionFactor_theta_Egamma_count","cluster energy correction factor (bin content container)",500, 0, 5., 80, 0, 180);//energy in GeV
   TH2F *HistClusterEnergyCorrectionFactor_theta_Egamma = new TH2F("HistClusterEnergyCorrectionFactor_theta_Egamma","cluster energy correction factor (normalized)",500, 0, 5., 80, 0, 180);//energy in GeV

  //////////////////////////////////////////////  
  TString inputfile("./energyCorrectionFactors_30MeVthreshold_cond2_Ntuple_10MillionEvt.root");
  TString outputfile("./energyCorrectionFactors_30MeVthreshold_cond2_Ntuple_10MillionEvt_supplement.root");
  //////////////////////////////////////////////
  TChain *chain = new TChain("EventParams");
  chain->Add(inputfile);
  chain->Print();     
  
  //Declaration of branch types:
  float eMc, eCluster, ClustEnCorrFactor, clustTheta, clustPhi;
  //Set branch addresses:
  chain->SetBranchAddress("eMc", &eMc);
  chain->SetBranchAddress("eCluster", &eCluster);
  chain->SetBranchAddress("ClustEnCorrFactor", &ClustEnCorrFactor);
  chain->SetBranchAddress("clustTheta", &clustTheta);
  chain->SetBranchAddress("clustPhi", &clustPhi);
    
  Int_t ncounts = chain->GetEntries();
  cout << "Number of events = " << ncounts<< endl;
  //ncounts=20000; // analyse specific number of events
 
  for (int j = 0; j < ncounts; j++)
  {
    chain->GetEntry(j); 
    HistClusterEnergyCorrectionFactor_original->Fill(clustTheta, clustPhi, ClustEnCorrFactor);//filling the 2D histogram with a weight of clusterEnergyCorrectionFactor
    HistClusterEnergyCorrectionFactor_count->Fill(clustTheta, clustPhi);//this histogram is filled just to keep track of the number of events in a specific (x,y) bin
    HistClusterEnergyCorrectionFactor_theta_Egamma_original->Fill(eCluster, clustTheta, ClustEnCorrFactor);
    HistClusterEnergyCorrectionFactor_theta_Egamma_count->Fill(eCluster, clustTheta);
    HistClusterEnergyCorrectionFactor_johan_original->Fill(eCluster, clustTheta, ClustEnCorrFactor);
    HistClusterEnergyCorrectionFactor_johan_count->Fill(eCluster, clustTheta);
    HistClusterEnergyCorrectionFactor_Hossein_original->Fill(eCluster, clustTheta, ClustEnCorrFactor);
    HistClusterEnergyCorrectionFactor_Hossein_count->Fill(eCluster, clustTheta);
    
    
    if (j%100000 ==0)
      cout<<"Evt. No. "<< j <<endl;
  }
   

///////////analysis of the filled histogram
for (int i=1; i<=180; i++)//theta
  for (int j=1; j<=360; j++)//phi
    if (HistClusterEnergyCorrectionFactor_count->GetBinContent(i,j) != 0)
      HistClusterEnergyCorrectionFactor->SetBinContent(i,j,(HistClusterEnergyCorrectionFactor_original->GetBinContent(i,j))/(HistClusterEnergyCorrectionFactor_count->GetBinContent(i,j)));
    else
      HistClusterEnergyCorrectionFactor->SetBinContent(i,j,0);

for (int i=1; i<=500; i++)//Egamma
  for (int j=1; j<=80; j++)//theta
    if (HistClusterEnergyCorrectionFactor_theta_Egamma_count->GetBinContent(i,j) != 0)
      HistClusterEnergyCorrectionFactor_theta_Egamma->SetBinContent(i,j,(HistClusterEnergyCorrectionFactor_theta_Egamma_original->GetBinContent(i,j))/(HistClusterEnergyCorrectionFactor_theta_Egamma_count->GetBinContent(i,j)));
    else
      HistClusterEnergyCorrectionFactor_theta_Egamma->SetBinContent(i,j,0);

    
for (int i=1; i<=26; i++)//Egamma
  for (int j=1; j<=61; j++)//theta
    if (HistClusterEnergyCorrectionFactor_johan_count->GetBinContent(i,j) != 0)
      HistClusterEnergyCorrectionFactor_johan->SetBinContent(i,j,(HistClusterEnergyCorrectionFactor_johan_original->GetBinContent(i,j))/(HistClusterEnergyCorrectionFactor_johan_count->GetBinContent(i,j)));
    else
      HistClusterEnergyCorrectionFactor_johan->SetBinContent(i,j,0);

    
for (int i=1; i<=35; i++)//Egamma
  for (int j=1; j<=53; j++)//theta
    if (HistClusterEnergyCorrectionFactor_Hossein_count->GetBinContent(i,j) != 0)
      HistClusterEnergyCorrectionFactor_Hossein->SetBinContent(i,j,(HistClusterEnergyCorrectionFactor_Hossein_original->GetBinContent(i,j))/(HistClusterEnergyCorrectionFactor_Hossein_count->GetBinContent(i,j)));
    else
      HistClusterEnergyCorrectionFactor_Hossein->SetBinContent(i,j,0);

   
//////////////////////////////////////////////////////////////////////////writing into a root file:    
   TFile* file_out = new TFile(outputfile,"RECREATE");
   
   HISTnumberOfClusters_thresholdPassed->Write();
   HistClusterEnergyCorrectionFactor_original->Write();
   HistClusterEnergyCorrectionFactor_count->Write();
   HistClusterEnergyCorrectionFactor->Write();
   HistClusterEnergyCorrectionFactor_theta_Egamma_original->Write();
   HistClusterEnergyCorrectionFactor_theta_Egamma_count->Write();
   HistClusterEnergyCorrectionFactor_theta_Egamma->Write();   
   HistClusterEnergyCorrectionFactor_johan_original->Write();
   HistClusterEnergyCorrectionFactor_johan_count->Write();
   HistClusterEnergyCorrectionFactor_johan->Write();
   HistClusterEnergyCorrectionFactor_Hossein_original->Write();
   HistClusterEnergyCorrectionFactor_Hossein_count->Write();
   HistClusterEnergyCorrectionFactor_Hossein->Write();
      
   file_out->Close();
   
////////////////////////////////////////////////////////////////////////// DRAWING
   gStyle->SetPalette(1); 
   TCanvas* c1 = new TCanvas("c1","", 100, 100, 1200, 800);  
   HistClusterEnergyCorrectionFactor_johan->Draw("surf4");
   HistClusterEnergyCorrectionFactor_johan->GetZaxis()->SetRangeUser(1,1.4);
   HistClusterEnergyCorrectionFactor_johan->GetXaxis()->SetRangeUser(0,5.1);  
//////////////////////////////////////////////////////////////////////////   
   
   timer.Stop();
   Double_t rtime = timer.RealTime();
   Double_t ctime = timer.CpuTime();
   printf("RealTime=%f seconds, CpuTime=%f seconds\n",rtime,ctime);
  return 0;
}

