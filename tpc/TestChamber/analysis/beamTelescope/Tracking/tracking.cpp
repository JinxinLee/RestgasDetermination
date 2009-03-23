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
#include <TF1.h>
#include <Math/Polynomial.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TMultiGraph.h>
int main(int argc,char **argv){
  using std::vector;
  using std::cout;
  using std::endl;
  using std::list;
  assert(argc==4);

  TApplication theApp("theApp",NULL,NULL);

  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("Hits");
  std::string alignmentFile = argv[2];
  bool disp=false;
  bool fit=true;
  string mode(argv[3]);
  if(mode == "disp"){
    disp = true;
  }else if(mode == "hist"){
    disp = false;
    fit = false;
  }else if(mode == "fit"){
    disp = false;
    fit = true;
  }

  CsGEMPlane* GM01X1=new CsGEMPlane();
  CsGEMPlane* GM01Y1=new CsGEMPlane();
  CsGEMPlane* GM02X1=new CsGEMPlane();
  CsGEMPlane* GM02Y1=new CsGEMPlane();
  CsGEMPlane* SI01X1=new CsGEMPlane();
  CsGEMPlane* SI01Y1=new CsGEMPlane();
  CsGEMPlane* SI02X1=new CsGEMPlane();
  CsGEMPlane* SI02Y1=new CsGEMPlane();
  
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
  TH1D *histogramChi2 = new TH1D("Chi2", "Chi2", 10000, 0, 1000000);

  TH1D *histogramSI1Xhitpoint = new TH1D("SI1Xhitpoint", "SI1Xhitpoint", 200, 5, 25);
  TH1D *histogramSI1Yhitpoint = new TH1D("SI1Yhitpoint", "SI1Yhitpoint", 200, 15, 30);
  TH1D *histogramSI2Xhitpoint = new TH1D("SI2Xhitpoint", "SI2Xhitpoint", 200, 5, 25);
  TH1D *histogramSI2Yhitpoint = new TH1D("SI2Yhitpoint", "SI2Yhitpoint", 200, 15, 30);
  TH1D *histogramGM1Xhitpoint = new TH1D("GM1Xhitpoint", "GM1Xhitpoint", 200, 5, 25);
  TH1D *histogramGM1Yhitpoint = new TH1D("GM1Yhitpoint", "GM1Yhitpoint", 200, 15, 30);
  TH1D *histogramGM2Xhitpoint = new TH1D("GM2Xhitpoint", "GM2Xhitpoint", 200, 5, 25);
  TH1D *histogramGM2Yhitpoint = new TH1D("GM2Yhitpoint", "GM2Yhitpoint", 200, 15, 30);
  
  TH1D *histogramSI1Xresi = new TH1D("resSI1X", "resSI1X", 200, -3, 3);
  TH1D *histogramSI1Yresi = new TH1D("resSI1Y", "resSI1Y", 200, -3, 3);
  TH1D *histogramSI2Xresi = new TH1D("resSI2X", "resSI2X", 200, -3, 3);
  TH1D *histogramSI2Yresi = new TH1D("resSI2Y", "resSI2Y", 200, -3, 3);
  TH1D *histogramGM1Xresi = new TH1D("resGM1X", "resGM1X", 200, -3, 3);
  TH1D *histogramGM1Yresi = new TH1D("resGM1Y", "resGM1Y", 200, -3, 3);
  TH1D *histogramGM2Xresi = new TH1D("resGM2X", "resGM2X", 200, -3, 3);
  TH1D *histogramGM2Yresi = new TH1D("resGM2Y", "resGM2Y", 200, -3, 3);

  TProfile *histogramSI1XresiVu = new TProfile("resVuSI1X", "resVuSI1X", 200, 0,2,-10,10);
  TProfile *histogramSI1YresiVu = new TProfile("resVuSI1Y", "resVuSI1Y", 200, 0,2,-10,10);
  TProfile *histogramSI2XresiVu = new TProfile("resVuSI2X", "resVuSI2X", 200, 0,2,-10,10);
  TProfile *histogramSI2YresiVu = new TProfile("resVuSI2Y", "resVuSI2Y", 200, 0,2,-10,10);
  TProfile *histogramGM1XresiVu = new TProfile("resVuGM1X", "resVuGM1X", 1000, 0,10,-10,10);
  TProfile *histogramGM1YresiVu = new TProfile("resVuGM1Y", "resVuGM1Y", 1000, 0,10,-10,10);
  TProfile *histogramGM2XresiVu = new TProfile("resVuGM2X", "resVuGM2X", 1000, 0,10,-10,10);
  TProfile *histogramGM2YresiVu = new TProfile("resVuGM2Y", "resVuGM2Y", 1000, 0,10,-10,10);

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
  TCanvas * c = new TCanvas("occupancy","occupancy",10,10,800,600);
  c->Divide(2,1);

  int nEvents=tree->GetEntries();
  TCevent* event = new TCevent();
  TFile* trackOutFile = new TFile("tracks.root","RECREATE");
  TTree* eventTreeOut = new TTree("at2","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&event,32000,99);
  /*
    Creating an instance of TCallign to assure wich alignmentfile is used
  */
  TCalign* a = TCalign::getInstance(alignmentFile);
  a->clear();
  a->read(alignmentFile);
    
  int n_tracks = 0;
  int track_tries=0;
  
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    if(i_ev%100==0){
      std::cout<<i_ev<<" "<<track_tries<<" "<<n_tracks<<"*******************************************************************************************************"<<std::endl;
    }
    tree->GetEntry(i_ev);
    event->clear();
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();
    std::vector<TCcluster> clSI1X;
    std::vector<TCcluster> clSI1Y;
    std::vector<TCcluster> clSI2X;
    std::vector<TCcluster> clSI2Y;
    std::vector<TCcluster> clGM1X;
    std::vector<TCcluster> clGM1Y;
    std::vector<TCcluster> clGM2X;
    std::vector<TCcluster> clGM2Y;
    
    
    ampDiffCut(SI01X1->GetClusters(), clSI1X, 7,3,alignmentFile);
    ampDiffCut(SI01Y1->GetClusters(), clSI1Y, 7,4,alignmentFile);
    ampDiffCut(SI02X1->GetClusters(), clSI2X, 7,5,alignmentFile);
    ampDiffCut(SI02Y1->GetClusters(), clSI2Y, 7,6,alignmentFile);
    ampRatioCut(GM02X1->GetClusters(), clGM2X, 1,0.8,7,alignmentFile);
    ampRatioCut(GM02Y1->GetClusters(), clGM2Y, 1,0.8,8,alignmentFile);
    ampRatioCut(GM01X1->GetClusters(), clGM1X, 1.1,1.1,1,alignmentFile);
    ampRatioCut(GM01Y1->GetClusters(), clGM1Y, 1.1,1.1,2,alignmentFile);
    
    histogramSI1X->Fill(clSI1X.size());
    histogramSI1Y->Fill(clSI1Y.size());    
    histogramSI2X->Fill(clSI2X.size());
    histogramSI2Y->Fill(clSI2Y.size());    
    histogramGM1X->Fill(clGM2X.size());
    histogramGM1Y->Fill(clGM2Y.size());   
    histogramGM2X->Fill(clGM1X.size());
    histogramGM2Y->Fill(clGM1Y.size());    
    histogramEvent->Fill(event->nClusters());
     int n_points=0;
     /*
     if(clSI1X.size()>3||
        clSI1Y.size()>3||
        clSI2X.size()>3||
        clSI2Y.size()>3){ 
       continue;
     }
*/

         
     vector<TCcluster> startClusters;
     vector<TCcluster> endClusters;
     for(unsigned int i=0;i<clGM1X.size();++i){
       for(unsigned int j=0;j<clGM1Y.size();++j){
         double ratio=clGM1X.at(i).getAmp()/clGM1Y.at(j).getAmp();
         if(ratio>0.9&&ratio<1.1){
           TVector3 pos(clGM1X.at(i).posUVW().x(),clGM1Y.at(j).posUVW().x()-5,0);
           TVector3 err(clGM1X.at(i).getErr().x(),0.2,0.2);
           double amp(clGM1X.at(i).getAmp());
           int detID(clGM1X.at(i).getId()); 
           TCcluster _c(pos,err,amp,detID); 
           _c.setFit(true);

           TVector3 pos2(clGM1Y.at(j).posUVW().x(),clGM1X.at(i).posUVW().x()+5,0);
           TVector3 err2(clGM1Y.at(j).getErr().x(),0.2,0.2);;
           double amp2(clGM1Y.at(j).getAmp());
           int detID2(clGM1Y.at(j).getId()); 
           TCcluster _c2(pos2,err2,amp2,detID2); 
           _c2.setFit(true);

           startClusters.push_back(_c);
           startClusters.push_back(_c2);
           //cout<<"gm1 pulse cor"<<endl;
         }
       } //end looping over GM1Y clusters
     } //end looping over GM1X clusters
         
     
     for(unsigned int i=0;i<clGM2X.size();++i){
       for(unsigned int j=0;j<clGM2Y.size();++j){
         double ratio=clGM2X.at(i).getAmp()/clGM2Y.at(j).getAmp();
         if(ratio>0.9&&ratio<1.1){
           TVector3 pos(clGM2X.at(i).posUVW().x(),clGM2Y.at(j).posUVW().x()-5,0);
           TVector3 err(clGM2X.at(i).getErr().x(),0.2,0.2);
           double amp(clGM2X.at(i).getAmp());      
           int detID(clGM2X.at(i).getId()); 
           TCcluster _c(pos,err,amp,detID); 
           _c.setFit(true);

           TVector3 pos2(clGM2Y.at(j).posUVW().x(),clGM2X.at(i).posUVW().x()+5,0);
           TVector3 err2(clGM2Y.at(j).getErr().x(),0.2,0.2);;
           double amp2(clGM2Y.at(j).getAmp());
           int detID2(clGM2Y.at(j).getId()); 
           TCcluster _c2(pos2,err2,amp2,detID2); 
           _c2.setFit(true);

           endClusters.push_back(_c);
           endClusters.push_back(_c2);
           //cout<<"gm1 pulse cor"<<endl;
         }
       } //end looping over GM1Y clusters
     } //end looping over GM1X clusters
     

     
     if(startClusters.size()==0||endClusters.size()==0){
       continue;
     }



     for(unsigned int i=0;i<startClusters.size()/2;i++){
       int id =2*i;
       //cout<<"start x "<<startClusters.size()<<" "<<id<<endl;
       TVector3 spacePoint=startClusters.at(id).posXYZ();
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
     for(unsigned int i=0;i<endClusters.size()/2;i++){
       int id =2*i;
       //cout<<"end x "<<endClusters.size()<<" "<<id<<endl;
       TVector3 spacePoint=endClusters.at(id).posXYZ();
       histogramGM2Xhitpoint->Fill(spacePoint.x());
       x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
       n_points++;
     }

     n_points=0;
     
     for(unsigned int i=0;i<startClusters.size()/2;i++){
       int id =2*i+1;
       //       cout<<"start y "<<startClusters.size()<<" "<<id<<endl;
       TVector3 spacePoint=startClusters.at(id).posXYZ();
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
     for(unsigned int i=0;i<endClusters.size()/2;i++){
       int id =2*i+1;
       //       cout<<"end y "<<endClusters.size()<<" "<<id<<endl;
       TVector3 spacePoint=endClusters.at(id).posXYZ();
       histogramGM2Yhitpoint->Fill(spacePoint.y());
       y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
       n_points++;
     }




     histogramStart->Fill(startClusters.size());
     histogramEnd->Fill(endClusters.size());
     
     //   cout<<"before loop startClusters.size "<<startClusters.size()<<endl;
     //looping over starting clusters 
     
     for(unsigned int i=0;i<startClusters.size()/2;++i){
       
       //looping over ending clusters---------------------------
       for(unsigned int j=0;j<endClusters.size()/2;++j){

         //looping over SI1X clusters----------------------------
         for(unsigned int k=0;k<clSI1X.size();++k){
           //looping over SI1Y cluster---------------------------
           for(unsigned int l=0;l<clSI1Y.size();++l){
             //looping over SI2X clusters-------------------------
             for(unsigned int m=0;m<clSI2X.size();++m){
               //looping over SI2Y clusters--------------------------
               for(unsigned int n=0;n<clSI2Y.size();++n){
                 if(skipEvent){
                   continue;
                 }
                 track_tries++;
                 TCtrack* track = new TCtrack;
                 vector<TCcluster> trackCl;
                 trackCl.push_back(startClusters.at(2*i));
                 trackCl.push_back(startClusters.at(2*i+1));
                 trackCl.push_back(clSI1X.at(k));
                 trackCl.push_back(clSI1Y.at(l));
                 trackCl.push_back(clSI2X.at(m));
                 trackCl.push_back(clSI2Y.at(n));
                 trackCl.push_back(endClusters.at(2*j));
                 trackCl.push_back(endClusters.at(2*j+1));
                 track->addClusters(trackCl);
                 //cout<<"nclFit"<<track->nClFit()<<endl;
                 
                 if(track->fit(1,2,3,4,5,6,7,8)){
                   
                   //cout<<"track fit converged with chi2 "<<track->getChi2()/track->getNDF()<<endl;
                   histogramChi2->Fill(track->getChi2()/track->getNDF());
                  
                   // double par[4]={track->getAX(),track->getBX(),track->getAY(),track->getBY()};
                   for(unsigned int cl=0;cl<track->nCl();cl++){
                   
                     TCcluster tmpcl = track->getCl(cl);
                     double uCl=tmpcl.posUVW().x();                   
                     TVector3 resid=tmpcl.getRes();
                     if(tmpcl.getFit()){
                       switch(tmpcl.getId()){
                     case 1:
                       histogramGM1Xresi->Fill(resid.x());
                       histogramGM1XresiVu->Fill(uCl,resid.x());
                       break;
                     case 2:
                       histogramGM1Yresi->Fill(resid.x());
                       histogramGM1YresiVu->Fill(uCl,resid.x());
                       break;
                     case 3:
                       histogramSI1Xresi->Fill(resid.x());
                       histogramSI1XresiVu->Fill(uCl,resid.x());
                       break;
                     case 4:                         
                       histogramSI1Yresi->Fill(resid.x());
                       histogramSI1YresiVu->Fill(uCl,resid.x());
                       break;
                     case 5:
                       histogramSI2Xresi->Fill(resid.x());
                       histogramSI2XresiVu->Fill(uCl,resid.x());
                       break;
                     case 6:
                       histogramSI2Yresi->Fill(resid.x());
                       histogramSI2YresiVu->Fill(uCl,resid.x());
                       break;
                     case 7:
                       histogramGM2Xresi->Fill(resid.x());
                       histogramGM2XresiVu->Fill(uCl,resid.x());
                       break;
                     case 8:
                       histogramGM2Yresi->Fill(resid.x());
                       histogramGM2YresiVu->Fill(uCl,resid.x());
                       break;
                       default:
                         cout<<"unknown id "<<tmpcl.getId()<<endl;
                       }
                     }
                   
                   }//end filling residual histograms
                   
                   n_tracks++;
                   event->addTrack(track);
                   if(track->getChi2()/track->getNDF()<1000){
                     TGraph *clusters_x = new TGraph;
                     TGraph *clusters_y = new TGraph;
                     
                     TVector3 spacePoint=startClusters.at(2*i).posXYZ();
                     clusters_x->SetPoint(0, spacePoint.z(),spacePoint.x());
                     spacePoint=clSI1X.at(k).posXYZ();
                     clusters_x->SetPoint(1, spacePoint.z(),spacePoint.x());
                     spacePoint=clSI2X.at(m).posXYZ();
                     clusters_x->SetPoint(2, spacePoint.z(),spacePoint.x());
                     spacePoint=endClusters.at(2*j).posXYZ();
                     clusters_x->SetPoint(3, spacePoint.z(),spacePoint.x());
                     
                     spacePoint=startClusters.at(2*i+1).posXYZ();
                     clusters_y->SetPoint(0, spacePoint.z(),spacePoint.y());
                     spacePoint=clSI1Y.at(l).posXYZ();
                     clusters_y->SetPoint(1, spacePoint.z(),spacePoint.y());
                     spacePoint=clSI2Y.at(n).posXYZ();
                     clusters_y->SetPoint(2, spacePoint.z(),spacePoint.y());
                     spacePoint=endClusters.at(2*j+1).posXYZ();
                     clusters_y->SetPoint(3, spacePoint.z(),spacePoint.y());
                     
                     clusters_x->SetMarkerColor(2);
                     clusters_y->SetMarkerColor(2);
                     clusters_x->SetMarkerSize(4);
                     clusters_y->SetMarkerSize(4);
                     clusters_x->SetMarkerStyle(2);
                     clusters_y->SetMarkerStyle(2);
                     double min,max,dummy1,dummy2;
                     x_event->ComputeRange(min,dummy1,max,dummy2);
                     char buf[10];
                     
                     TGraph *xTrackStartEndPoint = new TGraph;
                     TGraph *yTrackStartEndPoint = new TGraph;
                     
                     
                     double ax=track->getAx();// ax = tx
                     double bx=track->getBx();
                     cout<<"ax "<<ax<<" bx "<<bx<<endl;
                     cout<<"track "<<event->nTracks()<<" event nr "<<i_ev<<endl;
                     char bufFormula[50];
                     sprintf(bufFormula,"%f*x+%f",ax,bx);
                     TF1 *tf = new TF1("track1x",bufFormula,-10, 200);
                     tf->SetLineWidth(1.);
                     double ay=track->getAy();// ax = tx
                     double by=track->getBy();
                     sprintf(bufFormula,"%f*x+%f",ay,by);
                     TF1 *tf2 = new TF1("track1y",bufFormula,-10, 200);
                     tf2->SetLineWidth(1.);
                     
                     xTrackStartEndPoint->SetPoint(0, -1 ,-1*ax+bx);
                     xTrackStartEndPoint->SetPoint(1, 150 ,150*ax+bx);
                     
                     yTrackStartEndPoint->SetPoint(0, -1 ,-1*ay+by);
                     yTrackStartEndPoint->SetPoint(1, 150 ,150*ay+by);
                     
                     xTrackStartEndPoint->SetMarkerColor(3);
                     yTrackStartEndPoint->SetMarkerColor(3);
                     xTrackStartEndPoint->SetMarkerSize(1);
                     yTrackStartEndPoint->SetMarkerSize(1);
                     xTrackStartEndPoint->SetMarkerStyle(3);
                     yTrackStartEndPoint->SetMarkerStyle(3);

                     TMultiGraph *xGraph= new TMultiGraph();
                     TMultiGraph *yGraph= new TMultiGraph();
                     xGraph->Add(xTrackStartEndPoint);
                     xGraph->Add(clusters_x);
                     xGraph->Add(x_event,"*");                     

                     yGraph->Add(yTrackStartEndPoint);
                     yGraph->Add(clusters_y);
                     yGraph->Add(y_event,"*");                     
                     (c->cd(1))->Clear();
                     xGraph->Draw("AP");
                     tf->Draw("LSAME");
                     (c->cd(2))->Clear();
                     yGraph->Draw("AP");
                     tf2->Draw("LSAME");
                     
                     
                     
                     
                     c->Update();
                     c->Modified();
                     gSystem->ProcessEvents();
                     cout<<"chi2 "<<track->getChi2()<<", ndf="<<track->getNDF()<<endl;
                     cout<<"chi2/ndf "<<track->getChi2()/track->getNDF()<<endl;
                     cout<<"track "<<event->nTracks()<<" event nr "<<i_ev<<endl;
                     cout<<"press n to for next event, something else to redraw"<<endl;
                     string tmp2="";
                     cin>>tmp2;
                     if(tmp2=="n"){
                       continue;
                     }else if(tmp2=="ne"){
                       skipEvent=true;
                       continue;
                     }
                     cout<<"next event coming up"<<endl;
                     c->Update();
                     c->Modified();
                     gSystem->ProcessEvents();
                     
                     
                     cout<<"press a letter and enter for next event"<<endl;
                     cin>>tmp2;
                     cout<<"next event coming up"<<endl;

                   }
                   



                   //cout<<i_ev<<" "<<n_tracks<<endl;
                   //   track->draw();
                  
                   
                 }


                 //                 cout<<"bla"<<endl;
               }//end looping over SI2Y clusters
             }//end looping over SI2X clusters
           }//end looping over SI1Y clusters
         }//end looping over SI1X clusters
       }//end looping over END clusters
     }//end looping over START clusters
     gSystem->ProcessEvents();
     if(event->nTracks()>0){
    
     }
    

     /*
     If(event->nTracks()==0){
       continue;
     }
     */
     histogramTrack->Fill(event->nTracks());
     /*
     cout<<"press enter for next event"<<endl;
     string tmp;
     cin>>tmp;
     cout<<"next event coming up"<<endl;
     */    
  
     eventTreeOut->Fill();
     //if(n_tracks>1000) break;
  }
  cout<<"Tried trackfits"<<track_tries<<endl;
  cout<<"n tracks "<<n_tracks<<endl;
  eventTreeOut->Write();
  trackOutFile->Close();

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


  histogramSI1Xresi->Write();
  histogramSI1Yresi->Write();
  histogramSI2Xresi->Write();
  histogramSI2Yresi->Write();
  
  histogramGM1Xresi->Write();
  histogramGM1Yresi->Write();
  histogramGM2Xresi->Write();
  histogramGM2Yresi->Write();
  histogramSI1XresiVu->Write();
  histogramSI1YresiVu->Write();
  histogramSI2XresiVu->Write();
  histogramSI2YresiVu->Write();
  
  histogramGM1XresiVu->Write();
  histogramGM1YresiVu->Write();
  histogramGM2XresiVu->Write();
  histogramGM2YresiVu->Write();

  file->Close();
  delete histogramGM1X;
  
  delete  histogramGM1Y;
  delete  histogramGM2X;
  delete  histogramGM2Y;

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



