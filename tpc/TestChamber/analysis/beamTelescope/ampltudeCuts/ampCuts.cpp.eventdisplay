#include <list>
#include <iostream>
#include "../Hits.h"
#include <cassert>
#include"TApplication.h"

#include <TROOT.h>
#include <TObject.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1I.h>
#include "../../../src/TCtrack.h"
#include "../../../src/TCevent.h"
#include <TCanvas.h>
#include <TGraph.h>
#include "helpers.h"
#include <TStyle.h>
int main(int argc,char **argv){
  assert(argc==2);

  TApplication theApp("theApp",NULL,NULL);

  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("Hits");
  
  CsGEMPlane* GM01X1=new CsGEMPlane();
  CsGEMPlane* GM01Y1=new CsGEMPlane();
  CsGEMPlane* GM02X1=new CsGEMPlane();
  CsGEMPlane* GM02Y1=new CsGEMPlane();

  CsGEMPlane* SI01X1=new CsGEMPlane();
  CsGEMPlane* SI01Y1=new CsGEMPlane();
  CsGEMPlane* SI02X1=new CsGEMPlane();
  CsGEMPlane* SI02Y1=new CsGEMPlane();
  
  std::list<CsGEMCluster*> GM01X1___fClusters;
  std::list<CsGEMHit*> GM01X1___fHits;

  TBranch *branchGM01X1 =tree->GetBranch("GM01X1__.");
  TBranch *branchGM01Y1 =tree->GetBranch("GM01Y1__.");
  TBranch *branchGM02X1 =tree->GetBranch("GM02X1__.");
  TBranch *branchGM02Y1 =tree->GetBranch("GM02Y1__.");

  TBranch *branchSI01X1 =tree->GetBranch("SI01X1__.");
  TBranch *branchSI01Y1 =tree->GetBranch("SI01Y1__.");
  TBranch *branchSI02X1 =tree->GetBranch("SI02X1__.");
  TBranch *branchSI02Y1 =tree->GetBranch("SI02Y1__.");
  
  
  branchGM01X1->SetAddress(&GM01X1);
  branchGM01Y1->SetAddress(&GM01Y1);
  branchGM02X1->SetAddress(&GM02X1);
  branchGM02Y1->SetAddress(&GM02Y1);
  
  branchSI01X1->SetAddress(&SI01X1);  
  branchSI01Y1->SetAddress(&SI01Y1);  
  branchSI02X1->SetAddress(&SI02X1);  
  branchSI02Y1->SetAddress(&SI02Y1);  

  TH1I *histogramSI1X = new TH1I("Multiplicity  SI01X", "Multiplicity  SI01X", 20, 0, 20);
  TH1I *histogramSI1Y = new TH1I("Multiplicity  SI01Y", "Multiplicity  SI01Y", 20, 0, 20);
  TH1I *histogramSI2X = new TH1I("Multiplicity  SI02X", "Multiplicity  SI02X", 20, 0, 20);
  TH1I *histogramSI2Y = new TH1I("Multiplicity  SI02Y", "Multiplicity  SI02Y", 20, 0, 20);

  TH1I *histogramGM1X = new TH1I("Multiplicity  GM01X", "Multiplicity  GM01X", 20, 0, 20);
  TH1I *histogramGM1Y = new TH1I("Multiplicity  GM01Y", "Multiplicity  GM01Y", 20, 0, 20);
  TH1I *histogramGM2X = new TH1I("Multiplicity  GM02X", "Multiplicity  GM02X", 20, 0, 20);
  TH1I *histogramGM2Y = new TH1I("Multiplicity  GM02Y", "Multiplicity  GM02Y", 20, 0, 20);

  TH1I *histogramEvent = new TH1I("Event multiplicity", "Event multiplicity", 40, 0, 40);
  TH1I *histogramStart = new TH1I("Start cluster multiplicity", "Start cluster multiplicity", 40, 0, 40);
  TH1I *histogramEnd = new TH1I("End cluster multiplicity", "End cluster multiplicity", 40, 0, 40);
  TH1I *histogramTrack = new TH1I("Track multiplicity", "Track multiplicity", 40, 0, 40);
  TH1D *histogramChi2 = new TH1D("Chi2", "Chi2", 200, 0, 10);


  TH1D *histogramSI1Xhitpoint = new TH1D("SI1Xhitpoint", "SI1Xhitpoint", 1000, 0, 40);
  TH1D *histogramSI1Yhitpoint = new TH1D("SI1Yhitpoint", "SI1Yhitpoint", 1000, 0, 40);
  TH1D *histogramSI2Xhitpoint = new TH1D("SI2Xhitpoint", "SI2Xhitpoint", 1000, 0, 40);
  TH1D *histogramSI2Yhitpoint = new TH1D("SI2Yhitpoint", "SI2Yhitpoint", 1000, 0, 40);
  
  TH1D *histogramGM1Xhitpoint = new TH1D("GM1Xhitpoint", "GM1Xhitpoint", 1000, 0, 40);
  TH1D *histogramGM1Yhitpoint = new TH1D("GM1Yhitpoint", "GM1Yhitpoint", 1000, 0, 40);
  TH1D *histogramGM2Xhitpoint = new TH1D("GM2Xhitpoint", "GM2Xhitpoint", 1000, 0, 40);
  TH1D *histogramGM2Yhitpoint = new TH1D("GM2Yhitpoint", "GM2Yhitpoint", 1000, 0, 40);

  int nEvents=tree->GetEntries();
  /*
  looking at plots 
  cutting at 
  x<1.1 and y<1.1 for GM01X1 and GM01Y1
  x<0.8 and y<0.4 for GM02
  si still unknown
  */  
  using namespace std;
  TCevent* event = new TCevent();

  TFile* rootOutfile = new TFile("tracks.root","RECREATE");
  cout<<"after rootfile creation"<<endl;
  TTree* outTree = new TTree("at2","testBench analysis tree");
  cout<<"after TTree creation"<<endl;
  outTree->Branch("event","TCevent",&event,32000,99);
  cout<<"after TTree setBranch"<<endl;

  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read("../../../alignment/AlignmentFiles/simRealAlign.txt");
  TGraph* x_event =new TGraph;
  TGraph* y_event =new TGraph;
   
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  TCanvas * c = new TCanvas("occupancy","occupancy",10,10,800,600);
  c->Divide(2,1);
 
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    x_event->Clear();
    y_event->Clear();

    tree->GetEntry(i_ev);
     std::vector<TCcluster> clSI1X;
     std::vector<TCcluster> clSI1Y;
     std::vector<TCcluster> clSI2X;
     std::vector<TCcluster> clSI2Y;
     std::vector<TCcluster> clGM1X;
     std::vector<TCcluster> clGM1Y;
     std::vector<TCcluster> clGM2X;
     std::vector<TCcluster> clGM2Y;

     event->clear();
     { 
       const std::list<CsGEMCluster*> clusterList = SI01X1->GetClusters();
       ampDiffCut(clusterList, clSI1X, 12,5);
       histogramSI1X->Fill(clSI1X.size());
     }
     {
       const std::list<CsGEMCluster*> clusterList = SI01Y1->GetClusters();
       ampDiffCut(clusterList, clSI1Y, 12,6);
       histogramSI1Y->Fill(clSI1Y.size());    
     }
     { 
       const std::list<CsGEMCluster*> clusterList = SI02X1->GetClusters();
       ampDiffCut(clusterList, clSI2X, 12,7);
       histogramSI1X->Fill(clSI2X.size());
     }
     {
       const std::list<CsGEMCluster*> clusterList = SI02Y1->GetClusters();
       ampDiffCut(clusterList, clSI2Y, 12,8);
       histogramSI1Y->Fill(clSI2Y.size());    
     }
     { 
       const std::list<CsGEMCluster*> clusterList = GM01X1->GetClusters();
       ampDiffCut(clusterList, clGM1X, 12,1);
       histogramGM1X->Fill(clGM1X.size());
     }
     {
       const std::list<CsGEMCluster*> clusterList = GM01Y1->GetClusters();
       ampDiffCut(clusterList, clGM1Y, 12,2);
       histogramGM1Y->Fill(clGM1Y.size());    
     }
     { 
       const std::list<CsGEMCluster*> clusterList = GM02X1->GetClusters();
       ampDiffCut(clusterList, clGM2X, 12,3);
       histogramGM1X->Fill(clGM2X.size());
     }
     {
       const std::list<CsGEMCluster*> clusterList = GM02Y1->GetClusters();
       ampDiffCut(clusterList, clGM2Y, 12,4);
       histogramGM1Y->Fill(clGM2Y.size());    
     }
     
     histogramEvent->Fill(event->nClusters());
     int n_points=0;
     
     if(clSI1X.size()>2||
        clSI1Y.size()>2||
        clSI2X.size()>2||
        clSI2Y.size()>2){
       continue;
     }

     for(unsigned int i=0;i<clGM1X.size();++i){
       TVector3 spacePoint=clGM1X.at(i).posXYZ();
       histogramGM1Xhitpoint->Fill(spacePoint.x());
       x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
       
       n_points++;
     }
     for(unsigned int i=0;i<clSI1X.size();++i){

       TVector3 spacePoint=clSI1X.at(i).posXYZ();
       histogramSI1Xhitpoint->Fill(spacePoint.x());
       x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
       n_points++;
     }
     for(unsigned int i=0;i<clSI2X.size();++i){
       TVector3 spacePoint=clSI2X.at(i).posXYZ();
       histogramSI2Xhitpoint->Fill(spacePoint.x());
       x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
       n_points++;
     }
     for(unsigned int i=0;i<clGM2X.size();++i){
       TVector3 spacePoint=clGM2X.at(i).posXYZ();
       histogramGM2Xhitpoint->Fill(spacePoint.x());
       x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
       n_points++;
     }

     n_points=0;
     
     for(unsigned int i=0;i<clGM1Y.size();++i){
       TVector3 spacePoint=clGM1Y.at(i).posXYZ();
       histogramGM1Yhitpoint->Fill(spacePoint.y());
       y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
       n_points++;
     }
     for(unsigned int i=0;i<clSI1Y.size();++i){
       TVector3 spacePoint=clSI1Y.at(i).posXYZ();
       histogramSI1Yhitpoint->Fill(spacePoint.y());
       y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
       n_points++;
     }
     for(unsigned int i=0;i<clSI2Y.size();++i){
       TVector3 spacePoint=clSI2Y.at(i).posXYZ();
       histogramSI2Yhitpoint->Fill(spacePoint.y());
       y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
       n_points++;
     }
     for(unsigned int i=0;i<clGM2Y.size();++i){
       TVector3 spacePoint=clGM2Y.at(i).posXYZ();
       histogramGM2Yhitpoint->Fill(spacePoint.y());
       y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
       n_points++;
     }
     
     c->cd(1);
     x_event->Draw("a*");
     c->cd(2);
     y_event->Draw("a*");
       
     if(i_ev%100==0){
       std::cout<<i_ev<<" "<<event->nTracks()<<std::endl;
     }
     /*
     if(event->nTracks()==0){
       continue;
     }
     */
     histogramTrack->Fill(event->nTracks());

      
     c->Update();
     c->Modified();
     gSystem->ProcessEvents();
     

     //outTree->Fill();
     
     
 cout<<"press enter for next event"<<endl;
     string tmp;
     cin>>tmp;
     cout<<"next event coming up"<<endl;
          
     if(i_ev>10000) break;
  }

  outTree->Write();
  rootOutfile->Close();

  TFile* file = new TFile("clusterMultipCut.root","RECREATE");
  histogramGM1X->Write();
  histogramGM1Y->Write();
  histogramGM2X->Write();
  histogramGM2Y->Write();

  histogramSI1X->Write();
  histogramSI1Y->Write();
  histogramSI2X->Write(); 
  histogramSI2X->Write();
     
  histogramEvent->Write();
  histogramStart->Write();
  histogramEnd->Write();
  histogramTrack->Write();
  histogramChi2->Write();

  histogramSI1Xhitpoint->Write();
  histogramSI1Yhitpoint->Write();
  histogramSI2Xhitpoint->Write();
  histogramSI2Yhitpoint->Write();

  histogramGM1Xhitpoint->Write();
  histogramGM1Yhitpoint->Write();
  histogramGM2Xhitpoint->Write();
  histogramGM2Yhitpoint->Write();
  file->Close();
  //  delete histogramGM1X;

  //delete  histogramGM1Y;
  //  delete  histogramGM2X;
  //  delete  histogramGM2Y;

  delete histogramSI1X;
  delete  histogramSI1Y;
  delete  histogramSI2X;
  delete  histogramSI2Y;

  delete GM01X1;
  delete GM01Y1;
  delete GM02X1;
  delete GM02Y1;
  delete SI01X1;
  delete SI01Y1;
  delete SI02X1;
  delete SI02Y1;

  
return 0;
} 



