#include <list>
#include <iostream>
#include <string>
#include <cassert>

#include <TApplication.h>
#include <TROOT.h>
#include <TObject.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TStyle.h>
#include <TF1.h>
#include <Math/Polynomial.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TMultiGraph.h>

#include "../../../src/TChough2.h"
#include "../../../src/TCcluster.h"
#include "../../../src/TCalign.h"
#include "../../../src/TCtrack.h"
#include "../../../src/TCevent.h"
#include "../Hits.h"
#include "helpers.h"
int main(int argc,char **argv){
  using std::vector;
  using std::cout;
  using std::cerr;
  using std::cin;
  using std::endl;
  using std::list;
  using std::string;
  
  if(argc !=4){
    cerr<<"Wrong number of arguments"<<endl
        <<"Syntax should be ./tracker infile alignmentfile mode"<<endl;
    throw;
  }
  TApplication theApp("theApp",NULL,NULL);

  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("Hits");
  string alignmentFile = argv[2];
  bool disp=false;
  bool fit=true;
  string mode(argv[3]);
  if(mode == "disp"){
    disp = true;
    cout<<mode<<endl;
  }else if(mode == "hist"){
    cout<<mode<<endl;
    disp = false;
    fit = false;
  }else if(mode == "fit"){
    cout<<mode<<endl;
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
  TH1D *histogramChi2rough = new TH1D("Chi2rough", "Chi2rough", 10000, 0, 100000);
  TH1D *histogramChi2 = new TH1D("Chi2", "Chi2", 10000, 0, 1000);
  TH1D *histogramNDF = new TH1D("NDF", "NDF", 8, 0, 8);


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
  
  TH1D *histogramGM1XYratio = new TH1D("GM1XYratio", "GM1XYratio", 200, 0, 2);
  TH1D *histogramGM2XYratio = new TH1D("GM2XYratio", "GM1XYratio", 200, 0, 2);

  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);

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
    
  TVector3 X(1.,0.,0.);
  TVector3 Y(0.,1.,0.);
  TVector3 Z(0.,0.,1.);
  TChough2* houghYZ = new TChough2(Y,Z,0.2,0.0005,100,100);
  TChough2* houghXZ = new TChough2(-X,Z,0.2,0.0005,100,100);
  int n_tracks = 0;
  
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    if(i_ev%100==0){
      std::cout<<i_ev<<" "<<n_tracks<<"*******************************************************************************************************"<<std::endl;
     }   
    tree->GetEntry(i_ev);
    event->clear();
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
    vector<TCcluster> startClusters;
    vector<TCcluster> endClusters;
    for(unsigned int i=0;i<clGM1X.size();++i){
    
      for(unsigned int j=0;j<clGM1Y.size();++j){
	double ratio=clGM1X.at(i).getAmp()/clGM1Y.at(j).getAmp();
	histogramGM1XYratio->Fill(ratio);
        if(ratio>0.9&&ratio<1.3){
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
	}
      } //end looping over GM1Y clusters
    } //end looping over GM1X clusters
    for(unsigned int i=0;i<clGM2X.size();++i){
      for(unsigned int j=0;j<clGM2Y.size();++j){
	double ratio=clGM2X.at(i).getAmp()/clGM2Y.at(j).getAmp();
        histogramGM2XYratio->Fill(ratio);
	if(ratio>0.8&&ratio<1.3){
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
	}
      } //end looping over GM2Y clusters
    } //end looping over GM2X clusters

    if(clSI1X.size()>5||
       clSI1Y.size()>5||
       clSI2X.size()>5||
       clSI2Y.size()>5||
       startClusters.size()>10||
       clSI1X.size()==0||
       clSI1Y.size()==0||
       clSI2X.size()==0||
       clSI2Y.size()==0||
       startClusters.size()==0){
      continue;
    }
    histogramStart->Fill(startClusters.size());
    histogramEnd->Fill(endClusters.size());
    if((startClusters.size()==0||endClusters.size()==0)&&fit==true){
      cout<<"end- or startCluster.size == 0"<<endl;
      continue;
    }
    int n_points=0;
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();
    vector<TCcluster> clXZ;
    vector<TCcluster> clYZ;
    
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i;
      TVector3 spacePoint=startClusters.at(id).posXYZ();
      clXZ.push_back(startClusters.at(id));
      histogramGM1Xhitpoint->Fill(spacePoint.x());
      x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());     
      n_points++;
    }
    for(unsigned int i=0;i<clSI1X.size();++i){
      clXZ.push_back(clSI1X.at(i));
      TVector3 spacePoint=clSI1X.at(i).posXYZ();
      histogramSI1Xhitpoint->Fill(spacePoint.x());
      x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
      n_points++;
    }
    for(unsigned int i=0;i<clSI2X.size();++i){
      clXZ.push_back(clSI2X.at(i));
      TVector3 spacePoint=clSI2X.at(i).posXYZ();
      histogramSI2Xhitpoint->Fill(spacePoint.x());
      x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
      n_points++;
    }
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i;
      //clXZ.push_back(endClusters.at(id));
      TVector3 spacePoint=endClusters.at(id).posXYZ();
      histogramGM2Xhitpoint->Fill(spacePoint.x());
      x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());
      n_points++;
    }
    n_points=0;
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i+1;
      clYZ.push_back(startClusters.at(id));
      TVector3 spacePoint=startClusters.at(id).posXYZ();
      histogramGM1Yhitpoint->Fill(spacePoint.y());
      y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
      n_points++;
    }
    for(unsigned int i=0;i<clSI1Y.size();++i){
      clYZ.push_back(clSI1Y.at(i));
      TVector3 spacePoint=clSI1Y.at(i).posXYZ();
      histogramSI1Yhitpoint->Fill(spacePoint.y());
      y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
      n_points++;
    }
    for(unsigned int i=0;i<clSI2Y.size();++i){
      clYZ.push_back(clSI2Y.at(i));      
      TVector3 spacePoint=clSI2Y.at(i).posXYZ();
      histogramSI2Yhitpoint->Fill(spacePoint.y());
      y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
      n_points++;
    }
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i+1;
      //clYZ.push_back(endClusters.at(id));      
      TVector3 spacePoint=endClusters.at(id).posXYZ();
      histogramGM2Yhitpoint->Fill(spacePoint.y());
      y_event->SetPoint(n_points, spacePoint.z(),spacePoint.y());
      n_points++;
    }
    if(fit){
      houghYZ->make(clYZ,2);
      houghXZ->make(clXZ,2);
    }
    if(disp){

    }
    if(fit){
      vector<TCcluster> clTrack1;
      vector<TCcluster> clTrack2;
      TGraph *clFit_x1 = new TGraph;
      TGraph *clFit_y1 = new TGraph;

      TGraph *clFit_x2 = new TGraph;
      TGraph *clFit_y2 = new TGraph;
      n_points=0;
      for(unsigned int clX =0;clX<clXZ.size();++clX){
        if(houghXZ->hot(clX,0)){
          clXZ.at(clX).setFit();
          TVector3 spacePoint=clXZ.at(clX).posXYZ();
          clFit_x1->SetPoint(n_points, spacePoint.z(),spacePoint.x());
          n_points++;
        }else{
          clXZ.at(clX).setFit(false);
        }
        clTrack1.push_back(clXZ.at(clX));
        if(houghXZ->hot(clX,1)){
          clXZ.at(clX).setFit();
          TVector3 spacePoint=clXZ.at(clX).posXYZ();
          clFit_x2->SetPoint(n_points, spacePoint.z(),spacePoint.x());
          n_points++;
        }else{
          clXZ.at(clX).setFit(false);
        }
        clTrack2.push_back(clXZ.at(clX));
      }
      n_points=0;
      for(unsigned int clY =0;clY<clYZ.size();++clY){
        if(houghYZ->hot(clY,0)){
          clYZ.at(clY).setFit();
          if(clY%7==0){
            cout << "They are coming for you!" << endl;
            sleep(1);
          }
          TVector3 spacePoint=clYZ.at(clY).posXYZ();
          clFit_y1->SetPoint(n_points, spacePoint.z(),spacePoint.y());
          n_points++;
        }else{
          clYZ.at(clY).setFit(false);
        }
        clTrack1.push_back(clYZ.at(clY));
        if(houghYZ->hot(clY,1)){
          clYZ.at(clY).setFit();
          TVector3 spacePoint=clYZ.at(clY).posXYZ();
          clFit_y2->SetPoint(n_points, spacePoint.z(),spacePoint.y());
          n_points++;
        }else{
          clYZ.at(clY).setFit(false);
        }
        clTrack2.push_back(clYZ.at(clY));
      }
      TCtrack* track1 = new TCtrack;
      TCtrack* track2 = new TCtrack;
      track1->addClusters(clTrack1);
      track2->addClusters(clTrack2);
      //cout<<"nclFit"<<track->nClFit()<<endl;
      if(track1->fit(1,2,3,4,5,6)&&track2->fit(1,2,3,4,5,6)){
        if(disp){
          cout<<"track fit converged with chi2 "<<track1->getChi2()/track1->getNDF()<<endl;
        }
        
        histogramChi2->Fill(track1->getChi2()/track1->getNDF());
        histogramChi2rough->Fill(track1->getChi2()/track1->getNDF());
        histogramNDF->Fill(track1->getNDF());
        n_tracks++;
        event->addTrack(track1);
        event->addClusters(clTrack1);

        histogramChi2->Fill(track2->getChi2()/track2->getNDF());
        histogramChi2rough->Fill(track2->getChi2()/track2->getNDF());
        histogramNDF->Fill(track2->getNDF());
        n_tracks++;
        event->addTrack(track2);
        event->addClusters(clTrack2);
        
        for(unsigned int cl=0;cl<track1->nCl();cl++){
          TCcluster tmpcl = track1->getCl(cl);
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
        }
        for(unsigned int cl=0;cl<track2->nCl();cl++){
          TCcluster tmpcl = track2->getCl(cl);
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
        if(disp){
          TCanvas * c = new TCanvas("occupancy","occupancy",1,1,600,480);
          c->Divide(2,1);
          
          clFit_x2->SetMarkerColor(kBlue);
          clFit_y2->SetMarkerColor(kBlue);
          clFit_x2->SetMarkerSize(3);
          clFit_y2->SetMarkerSize(2);
          clFit_x2->SetMarkerStyle(2);
          clFit_y2->SetMarkerStyle(2);    

          clFit_x1->SetMarkerColor(kRed);
          clFit_y1->SetMarkerColor(kRed);
          clFit_x1->SetMarkerSize(3);
          clFit_y1->SetMarkerSize(3);
          clFit_x1->SetMarkerStyle(3);
          clFit_y1->SetMarkerStyle(3);

     
 
          TGraph *xTrackStartEndPoint = new TGraph;
          TGraph *yTrackStartEndPoint = new TGraph;
          double ax1=track1->getAx();// ax = tx
          double bx1=track1->getBx();
          double ay1=track1->getAy();// ay = ty
          double by1=track1->getBy();
          double ax2=track2->getAx();// ax = tx
          double bx2=track2->getBx();
          double ay2=track2->getAy();// ay = ty
          double by2=track2->getBy();
          char bufFormula[50];
          
          sprintf(bufFormula,"%f*x+%f",ax1,bx1);
          TF1 *tf = new TF1("track1x",bufFormula,-10, 200);
          tf->SetLineWidth(1.);
          
          sprintf(bufFormula,"%f*x+%f",ay1,by1);
          TF1 *tf2 = new TF1("track1y",bufFormula,-10, 200);
          tf2->SetLineWidth(1.);

          sprintf(bufFormula,"%f*x+%f",ax2,bx2);
          TF1 *tf3 = new TF1("track1x",bufFormula,-10, 200);
          tf3->SetLineWidth(1.);
          tf3->SetLineColor(kRed);
          
          sprintf(bufFormula,"%f*x+%f",ay2,by2);
          TF1 *tf4 = new TF1("track1y",bufFormula,-10, 200);
          tf4->SetLineWidth(1.);
          tf4->SetLineColor(kRed);          

          cout<<"ax1 "<<ax1<<" bx1 "<<bx1<<endl;
          cout<<"ay1 "<<ay1<<" by1 "<<by1<<endl;
          cout<<"ax2 "<<ax2<<" bx2 "<<bx2<<endl;
          cout<<"ay2 "<<ay2<<" by2 "<<by2<<endl;
          cout<<"track "<<event->nTracks()<<" event nr "<<i_ev<<endl;
          
          xTrackStartEndPoint->SetPoint(0, -1 ,-1*ax1+bx1);
          xTrackStartEndPoint->SetPoint(1, 150 ,150*ax1+bx1);
          yTrackStartEndPoint->SetPoint(0, -1 ,-1*ay1+by1);
          yTrackStartEndPoint->SetPoint(1, 150 ,150*ay1+by1);
          
          xTrackStartEndPoint->SetMarkerColor(3);
          yTrackStartEndPoint->SetMarkerColor(3);
          xTrackStartEndPoint->SetMarkerSize(1);
          yTrackStartEndPoint->SetMarkerSize(1);
          xTrackStartEndPoint->SetMarkerStyle(3);
          yTrackStartEndPoint->SetMarkerStyle(3);
          
          TMultiGraph *xGraph= new TMultiGraph();
          TMultiGraph *yGraph= new TMultiGraph();
          xGraph->Add(xTrackStartEndPoint);
          xGraph->Add(clFit_x2);
          xGraph->Add(clFit_x1);
          xGraph->Add(x_event,"*");                     
          
          yGraph->Add(yTrackStartEndPoint);
          yGraph->Add(clFit_y2);
          yGraph->Add(clFit_y1);
          yGraph->Add(y_event,"*");                     
          
          (c->cd(1))->Clear();
          xGraph->Draw("AP");
          tf->Draw("LSAME");
          tf3->Draw("LSAME");
          (c->cd(2))->Clear();
          yGraph->Draw("AP");
          tf2->Draw("LSAME");
          tf4->Draw("LSAME");
          
          c->Update();
          c->Modified();
          cout<<"before drawing hough"<<endl;
          houghXZ->draw(false,640,1,600,480,event);
          houghYZ->draw(false,1,512,600,480,event);
          gApplication->SetReturnFromRun(kTRUE);
          gSystem->Run();
          gROOT->Reset();
        }//end event display
      }//end if fit true
    }//end fit
    
    
  }//end looping of eventTree

  histogramTrack->Fill(event->nTracks());  
  eventTreeOut->Fill();
  //if(n_tracks>1000) break;
  
  cout<<"n tracks "<<n_tracks<<endl;
  eventTreeOut->Write();
  trackOutFile->Close();
  
  TFile* file = new TFile("clusterMultipCut.root","RECREATE");
  histogramGM1XYratio->Write();
  histogramGM2XYratio->Write();
  histogramGM1X->Write();
  histogramGM1Y->Write();
  histogramGM2X->Write();
  histogramGM2Y->Write();

  histogramSI1X->Write();
  histogramSI1Y->Write();
  histogramSI2X->Write(); 
  histogramSI2Y->Write();
     
  histogramEvent->Write();
  histogramStart->Write();
  histogramEnd->Write();
  histogramTrack->Write();
  histogramChi2->Write();
  histogramChi2rough->Write();
  histogramNDF->Write();

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



