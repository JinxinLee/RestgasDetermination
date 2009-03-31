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
  bool brute=false;
  bool hist=false;
  string mode(argv[3]);
  if(mode == "disp"){
    disp = true;
    cout<<mode<<endl;
  }else if(mode == "hist"){
    cout<<mode<<endl;
    disp = false;
    fit = false;
    hist=true;
  }else if(mode == "fit"){
    cout<<mode<<endl;
    disp = false;
    fit = true;
  }else if(mode == "brute"){
    cout<<mode<<endl;
    disp = false;
    fit = true;
    brute = true;
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

  TH1I *histogramEvent = new TH1I("Event multiplicity", "Event multiplicity", 20, 0, 40);
  TH1I *histogramStart = new TH1I("Start cluster multiplicity", "Start cluster multiplicity", 20, 0, 20);
  TH1I *histogramEnd = new TH1I("End cluster multiplicity", "End cluster multiplicity", 20, 0, 20);
  TH1I *histogramTrack = new TH1I("Track multiplicity", "Track multiplicity", 40, 0, 40);
  TH1D *histogramChi2rough = new TH1D("Chi2rough", "Chi2rough", 10000, 0, 100000);
  TH1D *histogramChi2 = new TH1D("Chi2", "Chi2", 10000, 0, 1000);
  TH1D *histogramNDF = new TH1D("NDF", "NDF", 8, 0, 8);


  TH1D *histogramSI1Xhitpoint = new TH1D("hitpoint SI1X", "hitpoint SI1X", 200, 10,20);
  TH1D *histogramSI1Yhitpoint = new TH1D("hitpoint SI1Y", "hitpoint SI1Y", 200, 20,30);
  TH1D *histogramSI2Xhitpoint = new TH1D("hitpoint SI2X", "hitpoint SI2X", 200, 10,20);
  TH1D *histogramSI2Yhitpoint = new TH1D("hitpoint SI2Y", "hitpoint SI2Y", 200, 20,30);
  TH1D *histogramGM1Xhitpoint = new TH1D("hitpoint GM1X", "hitpoint GM1X", 200, 5, 25);
  TH1D *histogramGM1Yhitpoint = new TH1D("hitpoint GM1Y", "hitpoint GM1Y", 200, 15,30);
  TH1D *histogramGM2Xhitpoint = new TH1D("hitpoint GM2X", "hitpoint GM2X", 200, 5, 25);
  TH1D *histogramGM2Yhitpoint = new TH1D("hitpoint GM2Y", "hitpoint GM2Y", 200, 15,30);
  
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
  TH1D *histogramGM1YXratio = new TH1D("GM1YXratio", "GM1YXratio", 200, 0, 2);
  TH1D *histogramGM2YXratio = new TH1D("GM2YXratio", "GM1YXratio", 200, 0, 2);

  TH1D *histogramGM1XYScaledRatio = new TH1D("GM1XYScaledRatio", "GM1XYScaledRatio", 200, 0, 2);
  TH1D *histogramGM2XYScaledRatio = new TH1D("GM2XYScaledRatio", "GM1XYScaledRatio", 200, 0, 2);
  TH1D *histogramGM1YXScaledRatio = new TH1D("GM1YXScaledRatio", "GM1YXScaledRatio", 200, 0, 2);
  TH1D *histogramGM2YXScaledRatio = new TH1D("GM2YXScaledRatio", "GM1YXScaledRatio", 200, 0, 2);
  TH1D *histogramGM1SmallBigScaledRatio = new TH1D("GM1BigSmallScaledRatio", "GM1BigSmallScaledRatio", 200, 0, 2);
  TH1D *histogramGM2SmallBigScaledRatio = new TH1D("GM2BigSmallScaledRatio", "GM2BigSmallScaledRatio", 200, 0, 2);


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
  TChough2* houghYZ = new TChough2( Y,Z,100,100,0.15,0.002);
  TChough2* houghXZ = new TChough2(-X,Z,100,100,0.15,0.002);
  int n_tracks = 0;
  int nToMany=0;
  int nZero=0;
  int noiseCut=0;
  int ratioCut=0;
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    if(i_ev%500==0){
      std::cout<<i_ev<<", nTrackTries: "<<n_tracks<<", To Many cl: "<<nToMany<< ", Empty: "<<nZero<<", noiseCut: "<<noiseCut<<", ratioCut: "<<ratioCut<<"********************************************************************"<<std::endl;
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
    
    
    ampDiffCut(SI01X1->GetClusters(), clSI1X, 5,3,alignmentFile, noiseCut);
    ampDiffCut(SI01Y1->GetClusters(), clSI1Y, 5,4,alignmentFile, noiseCut);
    ampDiffCut(SI02X1->GetClusters(), clSI2X, 5,5,alignmentFile, noiseCut);
    ampDiffCut(SI02Y1->GetClusters(), clSI2Y, 5,6,alignmentFile, noiseCut);
    ampRatioCut(GM01X1->GetClusters(), clGM1X, 0.9,1.1,1,alignmentFile, ratioCut);
    ampRatioCut(GM01Y1->GetClusters(), clGM1Y, 1.1,1.1,2,alignmentFile, ratioCut);
    ampRatioCut(GM02X1->GetClusters(), clGM2X, .2,0.9,7,alignmentFile, ratioCut);
    ampRatioCut(GM02Y1->GetClusters(), clGM2Y, 0.3,0.9,8,alignmentFile, ratioCut);
    
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
	histogramGM1YXratio->Fill(1/ratio);
        double x = clGM1X.at(i).getAmp();
        double y = clGM1Y.at(j).getAmp()*1.05;
         double ratio2=0;
        if(x>y){
          ratio2=y/x;
        }else{
          ratio2=x/y;
        }
        histogramGM1SmallBigScaledRatio->Fill(ratio2);
        histogramGM1YXScaledRatio->Fill(y/x);
        histogramGM1XYScaledRatio->Fill(x/y);
        if(ratio2>0.8){
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
	histogramGM2YXratio->Fill(1/ratio);
        double x = clGM2X.at(i).getAmp();
        double y = clGM2Y.at(j).getAmp()*1.1;
        double ratio2=0;
        if(x>y){
          ratio2=y/x;
        }else{
          ratio2=x/y;
        }
        histogramGM2SmallBigScaledRatio->Fill(ratio2);
        histogramGM2YXScaledRatio->Fill(y/x);
        histogramGM2XYScaledRatio->Fill(x/y);
	if(ratio2>0.8){
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

    if(!brute&&(clSI1X.size()>5||
                clSI1Y.size()>5||
                clSI2X.size()>5||
                clSI2Y.size()>5||
                startClusters.size()>10)){
      nToMany++;
      if(!hist){
        continue;
      }
    }
    if(clSI1X.size()==0||
       clSI1Y.size()==0||
       clSI2X.size()==0||
       clSI2Y.size()==0||
       startClusters.size()==0){
      nZero++;
      if(!hist){
        continue;
      }
    }
    histogramStart->Fill(startClusters.size()/2);
    histogramEnd->Fill(endClusters.size()/2);
   
    int n_points=0;
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();
    vector<TCcluster> clXZ;
    vector<TCcluster> clYZ;
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i;
      clusterFiller(startClusters.at(id),clXZ,histogramGM1Xhitpoint,x_event,n_points, true);
        /*
          TVector3 spacePoint=startClusters.at(id).posXYZ();
          clXZ.push_back(startClusters.at(id));
          histogramGM1Xhitpoint->Fill(spacePoint.x());
          x_event->SetPoint(n_points, spacePoint.z(),spacePoint.x());     
          n_points++;
        */
    }
    for(unsigned int i=0;i<clSI1X.size();++i){
      clusterFiller(clSI1X.at(i),clXZ,histogramSI1Xhitpoint,x_event,n_points, true);
    }
    for(unsigned int i=0;i<clSI2X.size();++i){
      clusterFiller(clSI2X.at(i),clXZ,histogramSI2Xhitpoint,x_event,n_points, true);
    }
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i;
      clusterFiller(endClusters.at(id),clXZ,histogramGM2Xhitpoint,x_event,n_points, true);
    }
    n_points=0;
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i+1;
      clusterFiller(startClusters.at(id),clYZ,histogramGM1Yhitpoint,y_event,n_points, false);
    }
    for(unsigned int i=0;i<clSI1Y.size();++i){
      clusterFiller(clSI1Y.at(i),clYZ,histogramSI1Yhitpoint,y_event,n_points, false);
    }
    for(unsigned int i=0;i<clSI2Y.size();++i){
      clusterFiller(clSI2Y.at(i),clYZ,histogramSI2Yhitpoint,y_event,n_points, false);
    }
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i+1;
      clusterFiller(endClusters.at(id),clYZ,histogramGM2Yhitpoint,y_event,n_points, false);
    }
    if(fit&&!brute){
      cout<<"befor"<<endl;
      houghYZ->make(clYZ,3);
      houghXZ->make(clXZ,3);
      cout<<"after"<<endl;
    }
    if(fit&&brute){
      TCevent event2;
      TCtrack store;
      double chiStore=99999999;
      for(unsigned int i=0;i<startClusters.size()/2;i++){
        for(unsigned int j=0;j<clSI1X.size();++j){
           for(unsigned int k=0;k<clSI2X.size();++k){
              for(unsigned int l=0;l<clSI1Y.size();++l){
                 for(unsigned int m=0;m<clSI2Y.size();++m){
                   if(endClusters.size()>0){
                     for(unsigned int n=0;n<endClusters.size()/2;n++){
                       int id1X =2*i;
                       int id1Y =2*i+1;
                       int id2X =2*n;
                       int id2Y =2*n+1;
                       TCtrack track;
                       vector<TCcluster> trackCl;
                       //                     cout<<"startX"<<endl;
                       trackCl.push_back(startClusters.at(id1X));
                       //cout<<"si1X"<<endl;
                       trackCl.push_back(clSI1X.at(j));
                       //                     cout<<"si2X"<<endl;
                       trackCl.push_back(clSI2X.at(k));
                       //                   cout<<"endX"<<endl;
                       trackCl.push_back(endClusters.at(id2X));
                       //                   cout<<"startY"<<endl;
                       trackCl.push_back(startClusters.at(id1Y));
                       //                     cout<<"si1Y"<<endl;
                       trackCl.push_back(clSI1Y.at(l));
                       //                     cout<<"si2Y "<<endl;
                       trackCl.push_back(clSI2Y.at(m));
                       //                     cout<<"endY"<<endl;
                       trackCl.push_back(endClusters.at(id2Y));
                       n_tracks++;
                       track.addClusters(trackCl);
                       track.fit(1,2,3,4,5,6,7,8);
                       if(track.getChi2()/track.getNDF()<chiStore){
                         store=track;
                         chiStore=track.getChi2()/track.getNDF();
                         histogramChi2->Fill(track.getChi2()/track.getNDF());
                         histogramChi2rough->Fill(track.getChi2()/track.getNDF());
                         histogramNDF->Fill(track.getNDF());
                       }
                     }//end endClusters l 2n and 2n+1
                   }else{
                     int id1X =2*i;
                     int id1Y =2*i+1;
                     TCtrack track;
                     vector<TCcluster> trackCl;
                     //                     cout<<"startX"<<endl;
                     trackCl.push_back(startClusters.at(id1X));
                     //cout<<"si1X"<<endl;
                     trackCl.push_back(clSI1X.at(j));
                     //                     cout<<"si2X"<<endl;
                     trackCl.push_back(clSI2X.at(k));
                     //                   cout<<"startY"<<endl;
                     trackCl.push_back(startClusters.at(id1Y));
                     //                     cout<<"si1Y"<<endl;
                     trackCl.push_back(clSI1Y.at(l));
                     //                     cout<<"si2Y "<<endl;
                     trackCl.push_back(clSI2Y.at(m));
                     n_tracks++;
                     track.addClusters(trackCl);
                     track.fit(1,2,3,4,5,6);
                     if(track.getChi2()/track.getNDF()<chiStore){
                       store=track;
                       chiStore=track.getChi2()/track.getNDF();
                       histogramChi2->Fill(track.getChi2()/track.getNDF());
                       histogramChi2rough->Fill(track.getChi2()/track.getNDF());
                       histogramNDF->Fill(track.getNDF());
                     }
                   } 
                 } //end clSI2Y m
              } //end clSI1Y l
           } // end clSI2X k
        } //end clSI1X j
      } // end startClusters i 2i and 2i+1
      //      event2.addTrack(store);
      for(unsigned int i=0;i<store.nCl();++i){
        TCcluster tmpcl = store.getCl(i);
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
    }
    if(fit&&!brute){
      vector<TCcluster> clTrack1;
      vector<TCcluster> clTrack2;
      TGraph *clFit_x1 = new TGraph;
      TGraph *clFit_y1 = new TGraph;

      TGraph *clFit_x2 = new TGraph;
      TGraph *clFit_y2 = new TGraph;
      n_points=0;
      int n_points2=0;
      for(unsigned int clX =0;clX<clXZ.size();++clX){
        if(houghXZ->getNmax()>0&&houghXZ->hot(clX,0)){
          clXZ.at(clX).setFit();
          TVector3 spacePoint=clXZ.at(clX).posXYZ();
          clFit_x1->SetPoint(n_points, spacePoint.z(),spacePoint.x());
          n_points++;
        }else{
          clXZ.at(clX).setFit(false);
        }
        clTrack1.push_back(clXZ.at(clX));
        if(houghXZ->getNmax()>1&&houghXZ->hot(clX,1)){
          clXZ.at(clX).setFit();
          TVector3 spacePoint=clXZ.at(clX).posXYZ();
          clFit_x2->SetPoint(n_points2, spacePoint.z(),spacePoint.x());
          n_points2++;
        }else{
          clXZ.at(clX).setFit(false);
        }
        clTrack2.push_back(clXZ.at(clX));
      }
      n_points=0;
      n_points2=0;
      for(unsigned int clY =0;clY<clYZ.size();++clY){
        if(houghYZ->getNmax()>0&&houghYZ->hot(clY,0)){
          clYZ.at(clY).setFit();
          TVector3 spacePoint=clYZ.at(clY).posXYZ();
          clFit_y1->SetPoint(n_points, spacePoint.z(),spacePoint.y());
          n_points++;
        }else{
          clYZ.at(clY).setFit(false);
        }
        clTrack1.push_back(clYZ.at(clY));
        if(houghYZ->getNmax()>1&&houghYZ->hot(clY,1)){
          clYZ.at(clY).setFit();
          TVector3 spacePoint=clYZ.at(clY).posXYZ();
          clFit_y2->SetPoint(n_points2, spacePoint.z(),spacePoint.y());
          n_points2++;
        }else{
          clYZ.at(clY).setFit(false);
        }
        clTrack2.push_back(clYZ.at(clY));
      }
      cout<<"after 2"<<endl;
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
          TCanvas * c = new TCanvas("Event display","Event Display",1,1,1200,480);
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
          houghXZ->draw(false,1,512,600,480,event);
          houghYZ->draw(false,640,512,600,480,event);
          gApplication->SetReturnFromRun(kTRUE);
          gSystem->Run();
          gROOT->Reset();
        }//end event display
      }//end if fit true
    }//end fit
    
    
  }//end looping of eventTree

  // histogramTrack->Fill(event->nTracks());  
  eventTreeOut->Fill();
  //if(n_tracks>1000) break;
  std::cout<<nEvents<<", nTrackTries: "<<n_tracks<<", To Many cl: "<<nToMany<< ", Empty: "<<nZero<<", noiseCut: "<<noiseCut<<", ratioCut: "<<ratioCut<<"********************************************************************"<<std::endl;
  
  eventTreeOut->Write();
  trackOutFile->Close();
  delete trackOutFile;
  TFile* file = new TFile("clusterMultipCut.root","RECREATE");
  histogramGM1XYratio->Write();
  histogramGM2XYratio->Write();
  histogramGM1YXratio->Write();
  histogramGM2YXratio->Write();
  
  histogramGM1XYScaledRatio->Write();
  histogramGM2XYScaledRatio->Write();
  histogramGM1YXScaledRatio->Write();
  histogramGM2YXScaledRatio->Write();
  histogramGM1SmallBigScaledRatio->Write();
  histogramGM2SmallBigScaledRatio->Write();

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
 
  delete GM01X1;
  delete GM01Y1;
  delete GM02X1;
  delete GM02Y1;
  delete SI01X1;
  delete SI01Y1;;
  delete SI02X1;
  delete SI02Y1;

  delete file;
  delete histogramGM1YXratio;
  delete histogramGM2YXratio;

  delete histogramGM1XYratio;
  delete histogramGM2XYratio;


  delete histogramGM1XYScaledRatio;
  delete histogramGM2XYScaledRatio;
  delete histogramGM1YXScaledRatio;
  delete histogramGM2YXScaledRatio;
  delete histogramGM1SmallBigScaledRatio;
  delete histogramGM2SmallBigScaledRatio;

  delete histogramGM1X;
  delete histogramGM1Y;
  delete histogramGM2X;
  delete histogramGM2Y;

  delete histogramSI1X;
  delete histogramSI1Y;
  delete histogramSI2X; 
  delete histogramSI2Y;
     
  delete histogramEvent;
  delete histogramStart;
  delete histogramEnd;
  delete histogramTrack;
  delete histogramChi2;
  delete histogramChi2rough;
  delete histogramNDF;

  delete histogramSI1Xhitpoint;
  delete histogramSI1Yhitpoint;
  delete histogramSI2Xhitpoint;
  delete histogramSI2Yhitpoint;

  delete histogramGM1Xhitpoint;
  delete histogramGM1Yhitpoint;
  delete histogramGM2Xhitpoint;
  delete histogramGM2Yhitpoint;


  delete histogramSI1Xresi;
  delete histogramSI1Yresi;
  delete histogramSI2Xresi;
  delete histogramSI2Yresi;
  
  delete histogramGM1Xresi;
  delete histogramGM1Yresi;
  delete histogramGM2Xresi;
  delete histogramGM2Yresi;
  delete histogramSI1XresiVu;
  delete histogramSI1YresiVu;
  delete histogramSI2XresiVu;
  delete histogramSI2YresiVu;
  
  delete histogramGM1XresiVu;
  delete histogramGM1YresiVu;
  delete histogramGM2XresiVu;
  delete histogramGM2YresiVu;
  return 0;
} 



