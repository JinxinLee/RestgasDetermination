
#include "helpers.h"
#include "../../../src/TCtrack.h"
#include <list>
#include <iostream>
#include "../Hits.h"
#include "../../../src/TCevent.h"
#include "../../../src/TCalign.h"
#include <TH1D.h>
#include <TGraph.h>
#include <TH2D.h>
#include <TProfile.h>
#include <TF1.h>
#include <TStyle.h>
#include <TMultiGraph.h>
#include <math.h>
#include <TRandom3.h>
using namespace std;
void dispDraw(TCtrack* track1, TCtrack* track2,  TGraph* clFit_x1,TGraph* clFit_y1, TGraph* clFit_x2, TGraph* clFit_y2,TGraph* x_event,TGraph* y_event){
  gROOT->SetStyle("Plain");
  gStyle->SetPalette(1);
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
}
void ampDiffCut(const std::list<CsGEMCluster*> &clusterList, std::vector<TCcluster> &tcClusters, double cut, int detID,string alignmentFile, int& counter, HistContainer* hcont,double umin, double umax){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read(alignmentFile);
  double pitch=a->getPitch(detID);
  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    vector<float> amps = (*it)->GetAmp();
    double amp3=amps.at(2);
    double noise=(*it)->GetNoise();
    double amp1=amps.at(0);
    //    double time=(*it)->GetTime();

    if((amp3-amp1>cut*noise)){
      hcont->ampFiller(detID,amp3,noise);
      double x=((*it)->GetPosition())*pitch;
      //      cout<<"difcut umin: "<<umin<<" x "<<x<<" umax "<<umax<<endl;
     //  if(time==1000){
//         continue;
//       }
      if(((umin==-1&&umax==-1)||(x>=umin&&x<=umax))){

        //cout<<"si"<<detID<<" "<<time<<endl;
        //cout<<"true"<<endl;
        TVector3 pos(x,0,0);
        TVector3 err(((*it)->GetPositionErr())*pitch,0.1,0.1);
	
	/*
	  TVector3 err(0.0369,0.1,0.1);
	  if(detID==5||detID==6){
          err=TVector3(0.0369,0.1,0.1);
	  }else{
          err=TVector3(0.0020,0.1,0.1);
	*/
	  
        TCcluster _c(pos,err,amp3,detID);
        _c.setFit(true);
        tcClusters.push_back(_c);
      }else{
        counter++;
      }
    }else{
      counter++;
    }
  }
}
void ampRatioCut(const std::list<CsGEMCluster*> &clusterList,
std::vector<TCcluster> &tcClusters,  double cutA1A3, double cutA2A3, int detID,string alignmentFile, int& counter, HistContainer* hcont,double umin, double umax){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read(alignmentFile);
  double pitch = a->getPitch(detID);
  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    vector<float> amps = (*it)->GetAmp();
    double amp3=amps.at(2);
    double noise=(*it)->GetNoise();
    double ratioA2A3=(amps.at(1))/(amp3);
    double ratioA1A3=(amps.at(0))/(amp3);
    
    if(ratioA1A3<cutA1A3 || ratioA2A3<cutA2A3){
      hcont->ampFiller(detID,amp3,noise);
      double x=((*it)->GetPosition())*pitch;
      //      cout<<"ratiocut umin: "<<umin<<" x "<<x<<" umax "<<umax<<endl;
      //      if((umin==-1&&umax==-1)||(x>=umin&&x<=umax)){
        //        cout<<"true"<<endl;

      TVector3 pos(x,0,0);
      TVector3 err(0,0,0);
      err=TVector3(((*it)->GetPositionErr())*pitch,0.5,0.1);      
    
      double amp((*it)->GetAmp().at(2));
      //        double time((*it)->GetTime());
      // cout<<time<<endl;
      TCcluster _c(pos,err,amp,detID);
      _c.setFit(true);
      tcClusters.push_back(_c);
      /*
	} else{
        counter++;
	}
	*/
    }else{
      counter++;
    }
  }
}

void ampRatioNoiseCut(const std::list<CsGEMCluster*> &clusterList,std::vector<TCcluster> &tcClusters, double cutA1A3, double cutA2A3,double cut, int detID,std::string alignmentFile, int& counter, HistContainer* hcont, double umin, double umax){
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read(alignmentFile);
  double pitch = a->getPitch(detID);
  TRandom3 rand(0);

  for(std::list<CsGEMCluster*>::const_iterator it =clusterList.begin();it!=clusterList.end();it++ ){
    vector<float> amps =(*it)->GetAmp();
    double amp3=amps.at(2);
    double amp1=amps.at(0);
    double amp2=amps.at(1);
    double ratioA2A3=(amp2+rand.Uniform(-0.5,0.5))/(amp3);
    double ratioA1A3=(amp1+rand.Uniform(-0.5,0.5))/(amp3);
    double noise=(*it)->GetNoise();
    
    hcont->ampFiller(detID,amp3,noise, ratioA1A3,ratioA2A3);
    if((ratioA1A3<cutA1A3 || ratioA2A3<cutA2A3)&&(amp3>cut*noise)){
      
      double x=((*it)->GetPosition())*pitch;
      //      cout<<"ratiocut umin: "<<umin<<" x "<<x<<" umax "<<umax<<endl;
      //if((umin==-1&&umax==-1)||(x>=umin&&x<=umax)){
        //        cout<<"true"<<endl;

      TVector3 pos(x,0,0);
      TVector3 err(0,0,0);
      err=TVector3(((*it)->GetPositionErr())*pitch,0.5,0.1);
      
      //        double time((*it)->GetTime());
      // cout<<time<<endl;
      TCcluster _c(pos,err,amp3,detID);
      _c.setFit(true);
      tcClusters.push_back(_c);
      //    } else{
      //         counter++;
      //      }
    }else{
      counter++;
    }
  }

}
void clusterFiller(TCcluster cl, std::vector<TCcluster>& clusters, TH1D* hitPointHist,TH1D* uHist, TH1D* errHist, TGraph* graph, int& counter, bool x){
  clusters.push_back(cl);
  TVector3 spacePoint = cl.posXYZ();
  double point;
  if(x){
    point=spacePoint.x();
  }else{
    point=spacePoint.y();
  }
  hitPointHist->Fill(point);
  uHist->Fill(cl.posUVW().x());
  errHist->Fill(cl.getErr().x());
  graph->SetPoint(counter, spacePoint.z(),point);
  counter++;
}
HistContainer::HistContainer(){

  hitmapGM1 =new TH2D("HitMapGM1", "HitMapGM1", 256, 0,10.24, 256, 0,10.24);
  hitmapGM2 =new TH2D("HitMapGM2", "HitMapGM2", 256, 0,10.24, 256, 0,10.24);
  hitmapSI1 =new TH2D("HitMapSI1", "HitMapSI1", 192, 0,1.92, 192, 0,1.92);
  hitmapSI2 =new TH2D("HitMapSI2", "HitMapSI2", 192, 0,1.92, 192, 0,1.92);

  histogramSI1X = new TH1I("Multiplicity  SI01X", "Multiplicity  SI01X", 20, 0, 20);
  histogramSI1Y = new TH1I("Multiplicity  SI01Y", "Multiplicity  SI01Y", 20, 0, 20);
  histogramSI2X = new TH1I("Multiplicity  SI02X", "Multiplicity  SI02X", 20, 0, 20);
  histogramSI2Y = new TH1I("Multiplicity  SI02Y", "Multiplicity  SI02Y", 20, 0, 20);
  histogramGM1X = new TH1I("Multiplicity  GM01X", "Multiplicity  GM01X", 20, 0, 20);
  histogramGM1Y = new TH1I("Multiplicity  GM01Y", "Multiplicity  GM01Y", 20, 0, 20);
  histogramGM2X = new TH1I("Multiplicity  GM02X", "Multiplicity  GM02X", 20, 0, 20);
  histogramGM2Y = new TH1I("Multiplicity  GM02Y", "Multiplicity  GM02Y", 20, 0, 20);

  histogramSI1XampRatio=new TH2D("ampRatioSI1X", "ampRatioSI1X", 50, -0,2,50, -0,2);
  histogramSI1YampRatio=new TH2D("ampRatioSI1Y", "ampRatioSI1Y", 50, -0,2,50, -0,2);
  histogramSI2XampRatio=new TH2D("ampRatioSI2X", "ampRatioSI2X", 50, -0,2,50, -0,2);
  histogramSI2YampRatio=new TH2D("ampRatioSI2Y", "ampRatioSI2Y", 50, -0,2,50, -0,2);
  histogramGM1XampRatio=new TH2D("ampRatioGM1X", "ampRatioGM1X", 50, -0,2,50, -0,2);
  histogramGM1YampRatio=new TH2D("ampRatioGM1Y", "ampRatioGM1Y", 50, -0,2,50, -0,2);
  histogramGM2XampRatio=new TH2D("ampRatioGM2X", "ampRatioGM2X", 50, -0,2,50, -0,2);
  histogramGM2YampRatio=new TH2D("ampRatioGM2Y", "ampRatioGM2Y", 50, -0,2,50, -0,2);

  histogramSI1XclNoise = new TH1D("clNoise SI1X", "clNoise SI1X", 2000, 0,20);
  histogramSI1YclNoise = new TH1D("clNoise SI1Y", "clNoise SI1Y", 2000, 0,20);
  histogramSI2XclNoise = new TH1D("clNoise SI2X", "clNoise SI2X", 2000, 0,20);
  histogramSI2YclNoise = new TH1D("clNoise SI2Y", "clNoise SI2Y", 2000, 0,20);
  histogramGM1XclNoise = new TH1D("clNoise GM1X", "clNoise GM1X", 2000, 0,20);
  histogramGM1YclNoise = new TH1D("clNoise GM1Y", "clNoise GM1Y", 2000, 0,20);
  histogramGM2XclNoise = new TH1D("clNoise GM2X", "clNoise GM2X", 2000, 0,20);
  histogramGM2YclNoise = new TH1D("clNoise GM2Y", "clNoise GM2Y", 2000, 0,20);

  histogramEvent = new TH1I("Event multiplicity", "Event multiplicity", 20, 0, 40);
  histogramStart = new TH1I("Start cluster multiplicity", "Start cluster multiplicity", 20, 0, 20);
  histogramEnd = new TH1I("End cluster multiplicity", "End cluster multiplicity", 20, 0, 20);
  histogramTrack = new TH1I("Track multiplicity", "Track multiplicity", 40, 0, 40);
  histogramChi2rough = new TH1D("Chi2rough", "Chi2rough", 10000, 0, 100000);
  histogramChi2 = new TH1D("Chi2", "Chi2", 10000, 0, 1000);
  histogramNDF = new TH1D("NDF", "NDF", 8, 0, 8);

  histogramSI1Xhitpoint = new TH1D("hitpoint SI1X", "hitpoint SI1X", 200, 10,20);
  histogramSI1Yhitpoint = new TH1D("hitpoint SI1Y", "hitpoint SI1Y", 200, 20,30);
  histogramSI2Xhitpoint = new TH1D("hitpoint SI2X", "hitpoint SI2X", 200, 10,20);
  histogramSI2Yhitpoint = new TH1D("hitpoint SI2Y", "hitpoint SI2Y", 200, 20,30);
  histogramGM1Xhitpoint = new TH1D("hitpoint GM1X", "hitpoint GM1X", 200, 5, 25);
  histogramGM1Yhitpoint = new TH1D("hitpoint GM1Y", "hitpoint GM1Y", 200, 15,30);
  histogramGM2Xhitpoint = new TH1D("hitpoint GM2X", "hitpoint GM2X", 200, 5, 25);
  histogramGM2Yhitpoint = new TH1D("hitpoint GM2Y", "hitpoint GM2Y", 200, 15,30);

  histogramSI1XAmp = new TH1D("Amp SI1X", "Amp SI1X", 384, 0,768);
  histogramSI1YAmp = new TH1D("Amp SI1Y", "Amp SI1Y", 384, 0,768);
  histogramSI2XAmp = new TH1D("Amp SI2X", "Amp SI2X", 384, 0,768);
  histogramSI2YAmp = new TH1D("Amp SI2Y", "Amp SI2Y", 384, 0,768);
  histogramGM1XAmp = new TH1D("Amp GM1X", "Amp GM1X", 1200, 0,2400);
  histogramGM1YAmp = new TH1D("Amp GM1Y", "Amp GM1Y", 1200, 0,2400);
  histogramGM2XAmp = new TH1D("Amp GM2X", "Amp GM2X", 1200, 0,2400);
  histogramGM2YAmp = new TH1D("Amp GM2Y", "Amp GM2Y", 1200, 0,2400);

  histogramSI1XU = new TH1D("U SI1X", "U SI1X", 400, 0,2);
  histogramSI1YU = new TH1D("U SI1Y", "U SI1Y", 400, 0,2);
  histogramSI2XU = new TH1D("U SI2X", "U SI2X", 400, 0,2);
  histogramSI2YU = new TH1D("U SI2Y", "U SI2Y", 400, 0,2);
  histogramGM1XU = new TH1D("U GM1X", "U GM1X", 400, 0,10.3);
  histogramGM1YU = new TH1D("U GM1Y", "U GM1Y", 400, 0,10.3);
  histogramGM2XU = new TH1D("U GM2X", "U GM2X", 400, 0,10.3);
  histogramGM2YU = new TH1D("U GM2Y", "U GM2Y", 400, 0,10.3);

  histogramSI1Xerr = new TH1D("err SI1X", "err SI1X", 400, 0,0.01);
  histogramSI1Yerr = new TH1D("err SI1Y", "err SI1Y", 400, 0,0.01);
  histogramSI2Xerr = new TH1D("err SI2X", "err SI2X", 400, 0,0.01);
  histogramSI2Yerr = new TH1D("err SI2Y", "err SI2Y", 400, 0,0.01);
  histogramGM1Xerr = new TH1D("err GM1X", "err GM1X", 400, 0,0.05);
  histogramGM1Yerr = new TH1D("err GM1Y", "err GM1Y", 400, 0,0.05);
  histogramGM2Xerr = new TH1D("err GM2X", "err GM2X", 400, 0,0.5);
  histogramGM2Yerr = new TH1D("err GM2Y", "err GM2Y", 400, 0,0.5);

  histogramSI1XhitpointBbest = new TH1D("brute best hitpoint SI1X", "brute best hitpoint SI1X", 200, 10,20);
  histogramSI1YhitpointBbest = new TH1D("brute best hitpoint SI1Y", "brute best hitpoint SI1Y", 200, 20,30);
  histogramSI2XhitpointBbest = new TH1D("brute best hitpoint SI2X", "brute best hitpoint SI2X", 200, 10,20);
  histogramSI2YhitpointBbest = new TH1D("brute best hitpoint SI2Y", "brute best hitpoint SI2Y", 200, 20,30);
  histogramGM1XhitpointBbest = new TH1D("brute best hitpoint GM1X", "brute best hitpoint GM1X", 200, 5, 25);
  histogramGM1YhitpointBbest = new TH1D("brute best hitpoint GM1Y", "brute best hitpoint GM1Y", 200, 15,30);
  histogramGM2XhitpointBbest = new TH1D("brute best hitpoint GM2X", "brute best hitpoint GM2X", 200, 5, 25);
  histogramGM2YhitpointBbest = new TH1D("brute best hitpoint GM2Y", "brute best hitpoint GM2Y", 200, 15,30);

  histogramSI1XUbBest = new TH1D("brute best U SI1X", "brute best U SI1X", 400, 0,2);
  histogramSI1YUbBest = new TH1D("brute best U SI1Y", "brute best U SI1Y", 400, 0,2);
  histogramSI2XUbBest = new TH1D("brute best U SI2X", "brute best U SI2X", 400, 0,2);
  histogramSI2YUbBest = new TH1D("brute best U SI2Y", "brute best U SI2Y", 400, 0,2);
  histogramGM1XUbBest = new TH1D("brute best U GM1X", "brute best U GM1X", 400, 0,10.3);
  histogramGM1YUbBest = new TH1D("brute best U GM1Y", "brute best U GM1Y", 400, 0,10.3);
  histogramGM2XUbBest = new TH1D("brute best U GM2X", "brute best U GM2X", 400, 0,10.3);
  histogramGM2YUbBest = new TH1D("brute best U GM2Y", "brute best U GM2Y", 400, 0,10.3);
  
  histogramSI1Xresi = new TH1D("resSI1X", "resSI1X", 400, -0.05, 0.05);
  histogramSI1Yresi = new TH1D("resSI1Y", "resSI1Y", 400, -0.05,0.05);
  histogramSI2Xresi = new TH1D("resSI2X", "resSI2X", 400, -0.7,0.7);
  histogramSI2Yresi = new TH1D("resSI2Y", "resSI2Y", 400, -0.7,0.7);
  histogramGM1Xresi = new TH1D("resGM1X", "resGM1X", 400, -0.3, 0.3);
  histogramGM1Yresi = new TH1D("resGM1Y", "resGM1Y", 400, -0.3, 0.3);
  histogramGM2Xresi = new TH1D("resGM2X", "resGM2X", 400, -3, 3);
  histogramGM2Yresi = new TH1D("resGM2Y", "resGM2Y", 400, -3, 3);
 
  histogramSI1XresiVu2d = new TH2D("resVu2dSI1X", "resVu2dSI1X", 200, 0,2,1000, -0.05,0.05);
  histogramSI1YresiVu2d = new TH2D("resVu2dSI1Y", "resVu2dSI1Y", 200, 0,2,1000, -0.05,0.05);
  histogramSI2XresiVu2d = new TH2D("resVu2dSI2X", "resVu2dSI2X", 200, 0,2,1000, -0.7,0.7);
  histogramSI2YresiVu2d = new TH2D("resVu2dSI2Y", "resVu2dSI2Y", 200, 0,2,1000, -0.7,0.7);
  histogramGM1XresiVu2d = new TH2D("resVu2dGM1X", "resVu2dGM1X", 1000, 0,10,1000, -0.3,0.3);
  histogramGM1YresiVu2d = new TH2D("resVu2dGM1Y", "resVu2dGM1Y", 1000, 0,10,1000, -0.3,0.3);
  histogramGM2XresiVu2d = new TH2D("resVu2dGM2X", "resVu2dGM2X", 1000, 0,10,1000, -10,10);
  histogramGM2YresiVu2d = new TH2D("resVu2dGM2Y", "resVu2dGM2Y", 1000, 0,10,1000, -10,10);

  histogramSI1XresiVu = new TProfile("resVuSI1X", "resVuSI1X", 400, 0,2,-10,10);
  histogramSI1YresiVu = new TProfile("resVuSI1Y", "resVuSI1Y", 400, 0,2,-10,10);
  histogramSI2XresiVu = new TProfile("resVuSI2X", "resVuSI2X", 400, 0,2,-10,10);
  histogramSI2YresiVu = new TProfile("resVuSI2Y", "resVuSI2Y", 400, 0,2,-10,10);
  histogramGM1XresiVu = new TProfile("resVuGM1X", "resVuGM1X", 1000, 0,10,-10,10);
  histogramGM1YresiVu = new TProfile("resVuGM1Y", "resVuGM1Y", 1000, 0,10,-10,10);
  histogramGM2XresiVu = new TProfile("resVuGM2X", "resVuGM2X", 1000, 0,10,-10,10);
  histogramGM2YresiVu = new TProfile("resVuGM2Y", "resVuGM2Y", 1000, 0,10,-10,10);

  histogramSI1XunBiResi = new TH1D("unBiResSI1X", "unBiResSI1X", 2000, -2.,2.);
  histogramSI1YunBiResi = new TH1D("unBiResSI1Y", "unBiResSI1Y", 2000, -2.,2.);
  histogramSI2XunBiResi = new TH1D("unBiResSI2X", "unBiResSI2X", 2000, -3.,3.);
  histogramSI2YunBiResi = new TH1D("unBiResSI2Y", "unBiResSI2Y", 2000, -3.,3.);
  histogramGM1XunBiResi = new TH1D("unBiResGM1X", "unBiResGM1X", 2000, -2.,2.);
  histogramGM1YunBiResi = new TH1D("unBiResGM1Y", "unBiResGM1Y", 2000, -3.,3.);

  histogramSI1XunBiResiVu2d = new TH2D("unBiResVu2dSI1X", "unBiResVu2dSI1X", 2000, 10,20,1000, -2.,2.);
  histogramSI1YunBiResiVu2d = new TH2D("unBiResVu2dSI1Y", "unBiResVu2dSI1Y", 2000, 20,30,1000, -2.,2.);
  histogramSI2XunBiResiVu2d = new TH2D("unBiResVu2dSI2X", "unBiResVu2dSI2X", 2000, 10,20,1000, -3.0,3.0);
  histogramSI2YunBiResiVu2d = new TH2D("unBiResVu2dSI2Y", "unBiResVu2dSI2Y", 2000, 20,30,1000, -3.0,3.0);
  histogramGM1XunBiResiVu2d = new TH2D("unBiResVu2dGM1X", "unBiResVu2dGM1X", 2000, 5,25,1000, -2.0,2.0);
  histogramGM1YunBiResiVu2d = new TH2D("unBiResVu2dGM1Y", "unBiResVu2dGM1Y", 2000, 15,30,1000, -3.0,3.0);

  histogramSI1XtrackErr = new TH1D("trackErrSI1X", "trackErrSI1X", 4000, -0.02, 0.02);
  histogramSI1YtrackErr = new TH1D("trackErrSI1Y", "trackErrSI1Y", 4000, -0.02,0.02);
  histogramSI2XtrackErr = new TH1D("trackErrSI2X", "trackErrSI2X", 4000, -0.02,0.02);
  histogramSI2YtrackErr = new TH1D("trackErrSI2Y", "trackErrSI2Y", 4000, -0.02,0.02);
  histogramGM1XtrackErr = new TH1D("trackErrGM1X", "trackErrGM1X", 4000, -0.02, 0.02);
  histogramGM1YtrackErr = new TH1D("trackErrGM1Y", "trackErrGM1Y", 4000, -0.02, 0.02);
  histogramGM2XtrackErr = new TH1D("trackErrGM2X", "trackErrGM2X", 4000, -0.02, 0.02);
  histogramGM2YtrackErr = new TH1D("trackErrGM2Y", "trackErrGM2Y", 4000, -0.02, 0.02);

  histogramSI1XtrackErrVu2d = new TH2D("trackErrVu2dSI1X", "trackErrVu2dSI1X", 200, 0,2,1000, -0.02,0.02);
  histogramSI1YtrackErrVu2d = new TH2D("trackErrVu2dSI1Y", "trackErrVu2dSI1Y", 200, 0,2,1000, -0.02,0.02);
  histogramSI2XtrackErrVu2d = new TH2D("trackErrVu2dSI2X", "trackErrVu2dSI2X", 200, 0,2,1000, -0.02,0.02);
  histogramSI2YtrackErrVu2d = new TH2D("trackErrVu2dSI2Y", "trackErrVu2dSI2Y", 200, 0,2,1000, -0.02,0.02);
  histogramGM1XtrackErrVu2d = new TH2D("trackErrVu2dGM1X", "trackErrVu2dGM1X", 1000, 0,10,1000, -0.02,0.02);
  histogramGM1YtrackErrVu2d = new TH2D("trackErrVu2dGM1Y", "trackErrVu2dGM1Y", 1000, 0,10,1000, -0.02,0.02);
  histogramGM2XtrackErrVu2d = new TH2D("trackErrVu2dGM2X", "trackErrVu2dGM2X", 1000, 0,10,1000, -0.02,0.02);
  histogramGM2YtrackErrVu2d = new TH2D("trackErrVu2dGM2Y", "trackErrVu2dGM2Y", 1000, 0,10,1000, -0.02,0.02);
  
  histogramGM1XYratio = new TH1D("GM1XYratio", "GM1XYratio", 200, 0, 2);
  histogramGM2XYratio = new TH1D("GM2XYratio", "GM1XYratio", 200, 0, 2);
  histogramGM1YXratio = new TH1D("GM1YXratio", "GM1YXratio", 200, 0, 2);
  histogramGM2YXratio = new TH1D("GM2YXratio", "GM1YXratio", 200, 0, 2);

  histogramGM1XYScaledRatio = new TH1D("GM1XYScaledRatio", "GM1XYScaledRatio", 200, 0, 2);
  histogramGM2XYScaledRatio = new TH1D("GM2XYScaledRatio", "GM1XYScaledRatio", 200, 0, 2);
  histogramGM1YXScaledRatio = new TH1D("GM1YXScaledRatio", "GM1YXScaledRatio", 200, 0, 2);
  histogramGM2YXScaledRatio = new TH1D("GM2YXScaledRatio", "GM1YXScaledRatio", 200, 0, 2);
  histogramGM1SmallBigScaledRatio = new TH1D("GM1BigSmallScaledRatio", "GM1BigSmallScaledRatio", 200, 0, 2);
  histogramGM2SmallBigScaledRatio = new TH1D("GM2BigSmallScaledRatio", "GM2BigSmallScaledRatio", 200, 0, 2);

  histogramTrackErrMatrix = new TH2D("trackErrMatrixSI1X", "trackErrMatrixSI1X", 4, 0,3,4, 0,0);
 
}
void HistContainer::fillRes(TCtrack* track,string alignmentFile){
  // cout<<endl;
  TCalign* a = TCalign::getInstance("../../../alignment/AlignmentFiles/simRealAlign.txt");
  a->clear();
  a->read(alignmentFile);
  
  TMatrixT<double> rot(3,3);
  TVector3 dummyV;
  double dummyD;
    
  

  for(unsigned int i=0;i<track->nCl();++i){
    
    TCcluster tmpcl = track->getCl(i);
 
    double uCl=tmpcl.posUVW().x();
    double z=tmpcl.posXYZ().z();
    TVector3 resid=tmpcl.getRes();
//     double dax=track->getDax();
//     double day=track->getDay();
//     double dbx=track->getDbx();
//     double dby=track->getDby();

    TMatrixT<double> coVarTrackParam = track->getErrMatrix();
    a->getConv(tmpcl.getId(),dummyV,rot,dummyD);
    
    TMatrixT<double> D(2,1);
    D[0][0]=rot[0][0];
    D[1][0]=rot[1][0];
    TMatrixT<double> D_T= D.T();
    D.T();
    TMatrixT<double> T(4,2);
    T[0][0]=z; T[0][1]=0;
    T[1][0]=1; T[1][1]=0;
    T[2][0]=0; T[2][1]=z;
    T[3][0]=0; T[3][1]=1;
    TMatrixT<double> T_T=T.T();
    T.T();
    // cout<<"z "<<z<<endl;
//     coVarTrackParam.Print();
//     D.Print();
//     D_T.Print();
//     T.Print();
//     T_T.Print();
    
    TMatrixT<double> sigmaU= D_T*T_T*coVarTrackParam*T*D;
//     sigmaU.Print();
    
      
    double dx=1;//sqrt(z*z*dax*dax+dbx*dbx+2*z*covar[0][1]);
    double dy=1;//sqrt(z*z*day*day+dby*dby+2*z*covar[2][3]);
    //  cout<<tmpcl.getId()<<"  Z   "<<z<<" ....................dx "<<dx<<" dy "<<dy<<endl;
    if(tmpcl.getFit()){
      switch(tmpcl.getId()){
      case 1:
	histogramGM1Xresi->Fill(resid.x());
	histogramGM1XresiVu->Fill(uCl,resid.x());
	histogramGM1XresiVu2d->Fill(uCl,resid.x());
	histogramGM1XUbBest->Fill(uCl);
	histogramGM1XhitpointBbest->Fill(tmpcl.posXYZ().x());
	histogramGM1XtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramGM1XtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
	break;
      case 2:
	histogramGM1Yresi->Fill(resid.x());
	histogramGM1YresiVu->Fill(uCl,resid.x());
	histogramGM1YresiVu2d->Fill(uCl,resid.x());
	histogramGM1YUbBest->Fill(uCl);
	histogramGM1YhitpointBbest->Fill(tmpcl.posXYZ().y());
	histogramGM1YtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramGM1YtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
	break;
      case 3:
	histogramSI1Xresi->Fill(resid.x());
	histogramSI1XresiVu->Fill(uCl,resid.x());
	histogramSI1XresiVu2d->Fill(uCl,resid.x());
	histogramSI1XUbBest->Fill(uCl);
	histogramSI1XhitpointBbest->Fill(tmpcl.posXYZ().x());
	histogramSI1XtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramSI1XtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
	break;
      case 4:                         
	histogramSI1Yresi->Fill(resid.x());
	histogramSI1YresiVu->Fill(uCl,resid.x());
	histogramSI1YresiVu2d->Fill(uCl,resid.x());
	histogramSI1YUbBest->Fill(uCl);
	histogramSI1YhitpointBbest->Fill(tmpcl.posXYZ().y());
	histogramSI1YtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramSI1YtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
	break;
      case 5:
	histogramSI2Xresi->Fill(resid.x());
	histogramSI2XresiVu->Fill(uCl,resid.x());
	histogramSI2XresiVu2d->Fill(uCl,resid.x());
	histogramSI2XUbBest->Fill(uCl);
	histogramSI2XhitpointBbest->Fill(tmpcl.posXYZ().x());
	histogramSI2XtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramSI2XtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
        //        cout<<sqrt(sigmaU[0][0])<<endl;
	break;
      case 6:
	histogramSI2Yresi->Fill(resid.x());
	histogramSI2YresiVu->Fill(uCl,resid.x());
	histogramSI2YresiVu2d->Fill(uCl,resid.x());
	histogramSI2YUbBest->Fill(uCl);
	histogramSI2YhitpointBbest->Fill(tmpcl.posXYZ().y());
	histogramSI2YtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramSI2YtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
        // cout<<sqrt(sigmaU[0][0])<<endl;
	break;
      case 7:
	histogramGM2Xresi->Fill(resid.x());
	histogramGM2XresiVu->Fill(uCl,resid.x());
	histogramGM2XresiVu2d->Fill(uCl,resid.x());
	histogramGM2XUbBest->Fill(uCl);
	histogramGM2XhitpointBbest->Fill(tmpcl.posXYZ().x());
	histogramGM2XtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramGM2XtrackErrVu2d->Fill(uCl,sqrt(sigmaU[0][0]));
	break;
      case 8:
	histogramGM2Yresi->Fill(resid.x());
	histogramGM2YresiVu->Fill(uCl,resid.x());
	histogramGM2YresiVu2d->Fill(uCl,resid.x());
	histogramGM2YUbBest->Fill(uCl);
	histogramGM2YhitpointBbest->Fill(tmpcl.posXYZ().y());
	histogramGM2YtrackErr->Fill(sqrt(sigmaU[0][0]));
	histogramGM2YtrackErrVu2d->Fill(uCl,dy);
	break;
      default:
	cout<<"unknown id "<<tmpcl.getId()<<endl;
      }
    }
  }
  //  cout<<endl<<endl;
}

void HistContainer::ampFiller(int detId, double amp, double noise, double ratioA1A3, double ratioA2A3 ){
  switch(detId){
      case 1:
	histogramGM1XAmp->Fill(amp);
        histogramGM1XclNoise->Fill(noise);
        histogramGM1XampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 2:
	histogramGM1YAmp->Fill(amp);
        histogramGM1YclNoise->Fill(noise);
        histogramGM1YampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 3:
	histogramSI1XAmp->Fill(amp);
        histogramSI1XclNoise->Fill(noise);
        histogramSI1XampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 4:                         
	histogramSI1YAmp->Fill(amp);
        histogramSI1YclNoise->Fill(noise);
        histogramSI1YampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 5:
	histogramSI2XAmp->Fill(amp);
        histogramSI2XclNoise->Fill(noise);
        histogramSI2XampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 6:
	histogramSI2YAmp->Fill(amp);
        histogramSI2YclNoise->Fill(noise);
        histogramSI2YampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 7:
	histogramGM2XAmp->Fill(amp);
        histogramGM2XclNoise->Fill(noise);
        histogramGM2XampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      case 8:
	histogramGM2YAmp->Fill(amp);
        histogramGM2YclNoise->Fill(noise);
        histogramGM2YampRatio->Fill(ratioA2A3,ratioA1A3);
	break;
      default:
	cout<<"unknown id "<<detId<<endl;
      }
}
void HistContainer::write(string filename){
  TFile* file = new TFile(filename.c_str(),"RECREATE");
  
  hitmapGM1->Write();
  hitmapGM2->Write();
  hitmapSI1->Write();
  hitmapSI2->Write();

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

  histogramSI1XAmp->Write();
  histogramSI1YAmp->Write();
  histogramSI2XAmp->Write();
  histogramSI2YAmp->Write();
  histogramGM1XAmp->Write();
  histogramGM1YAmp->Write();
  histogramGM2XAmp->Write();
  histogramGM2YAmp->Write();

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

  histogramSI1XclNoise->Write();
  histogramSI1YclNoise->Write();
  histogramSI2XclNoise->Write();
  histogramSI2YclNoise->Write();

  histogramGM1XclNoise->Write();
  histogramGM1YclNoise->Write();
  histogramGM2XclNoise->Write();
  histogramGM2YclNoise->Write();

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
  
  histogramSI1XresiVu2d->Write(); 
  histogramSI1YresiVu2d->Write(); 
  histogramSI2XresiVu2d->Write(); 
  histogramSI2YresiVu2d->Write(); 
  histogramGM1XresiVu2d->Write(); 
  histogramGM1YresiVu2d->Write(); 
  histogramGM2XresiVu2d->Write(); 
  histogramGM2YresiVu2d->Write(); 

  histogramGM1XunBiResi->Write();
  histogramGM1YunBiResi->Write();
  histogramSI1XunBiResi->Write();
  histogramSI1YunBiResi->Write();
  histogramSI2XunBiResi->Write();
  histogramSI2YunBiResi->Write();
  
  histogramSI1XunBiResiVu2d->Write(); 
  histogramSI1YunBiResiVu2d->Write(); 
  histogramSI2XunBiResiVu2d->Write(); 
  histogramSI2YunBiResiVu2d->Write(); 
  histogramGM1XunBiResiVu2d->Write(); 
  histogramGM1YunBiResiVu2d->Write(); 


  histogramSI1XU->Write();
  histogramSI1YU->Write();
  histogramSI2XU->Write();
  histogramSI2YU->Write();
  histogramGM1XU->Write();
  histogramGM1YU->Write();
  histogramGM2XU->Write();
  histogramGM2YU->Write();

  histogramSI1XhitpointBbest->Write();
  histogramSI1YhitpointBbest->Write();
  histogramSI2XhitpointBbest->Write();
  histogramSI2YhitpointBbest->Write();
  histogramGM1XhitpointBbest->Write();
  histogramGM1YhitpointBbest->Write();
  histogramGM2XhitpointBbest->Write();
  histogramGM2YhitpointBbest->Write();

  histogramSI1XUbBest->Write();
  histogramSI1YUbBest->Write();
  histogramSI2XUbBest->Write();
  histogramSI2YUbBest->Write();
  histogramGM1XUbBest->Write();
  histogramGM1YUbBest->Write();
  histogramGM2XUbBest->Write();
  histogramGM2YUbBest->Write();

  histogramSI1Xerr->Write();
  histogramSI1Yerr->Write();
  histogramSI2Xerr->Write();
  histogramSI2Yerr->Write();
  histogramGM1Xerr->Write();
  histogramGM1Yerr->Write();
  histogramGM2Xerr->Write();
  histogramGM2Yerr->Write();

  histogramSI1XtrackErr->Write();
  histogramSI1YtrackErr->Write();
  histogramSI2XtrackErr->Write();
  histogramSI2YtrackErr->Write();
  histogramGM1XtrackErr->Write();
  histogramGM1YtrackErr->Write();
  histogramGM2XtrackErr->Write();
  histogramGM2YtrackErr->Write();

  histogramSI1XtrackErrVu2d->Write();
  histogramSI1YtrackErrVu2d->Write();
  histogramSI2XtrackErrVu2d->Write();
  histogramSI2YtrackErrVu2d->Write();
  histogramGM1XtrackErrVu2d->Write();
  histogramGM1YtrackErrVu2d->Write();
  histogramGM2XtrackErrVu2d->Write();
  histogramGM2YtrackErrVu2d->Write();

  histogramSI1XampRatio->Write();
  histogramSI1YampRatio->Write();
  histogramSI2XampRatio->Write();
  histogramSI2YampRatio->Write();
  histogramGM1XampRatio->Write();
  histogramGM1YampRatio->Write();
  histogramGM2XampRatio->Write();
  histogramGM2YampRatio->Write();

  file->Close();
  delete file;

}

HistContainer::~HistContainer(){
  delete hitmapGM1;
  delete hitmapGM2;
  delete hitmapSI1;
  delete hitmapSI2;
  
  delete histogramTrackErrMatrix;
  delete histogramSI1Xerr;
  delete histogramSI1Yerr;
  delete histogramSI2Xerr;
  delete histogramSI2Yerr;
  delete histogramGM1Xerr;
  delete histogramGM1Yerr;
  delete histogramGM2Xerr;
  delete histogramGM2Yerr;

  delete histogramSI1XAmp;
  delete histogramSI1YAmp;
  delete histogramSI2XAmp;
  delete histogramSI2YAmp;
  delete histogramGM1XAmp;
  delete histogramGM1YAmp;
  delete histogramGM2XAmp;
  delete histogramGM2YAmp;

  delete histogramSI1XhitpointBbest;
  delete histogramSI1YhitpointBbest;
  delete histogramSI2XhitpointBbest;
  delete histogramSI2YhitpointBbest;
  delete histogramGM1XhitpointBbest;
  delete histogramGM1YhitpointBbest;
  delete histogramGM2XhitpointBbest;
  delete histogramGM2YhitpointBbest;

  delete histogramSI1XUbBest;
  delete histogramSI1YUbBest;
  delete histogramSI2XUbBest;
  delete histogramSI2YUbBest;
  delete histogramGM1XUbBest;
  delete histogramGM1YUbBest;
  delete histogramGM2XUbBest;
  delete histogramGM2YUbBest;
  
  delete histogramSI1XU;
  delete histogramSI1YU;
  delete histogramSI2XU;
  delete histogramSI2YU;
  delete histogramGM1XU;
  delete histogramGM1YU;
  delete histogramGM2XU;
  delete histogramGM2YU;
 
  delete histogramSI1XresiVu2d; 
  delete histogramSI1YresiVu2d; 
  delete histogramSI2XresiVu2d; 
  delete histogramSI2YresiVu2d; 
  delete histogramGM1XresiVu2d; 
  delete histogramGM1YresiVu2d; 
  delete histogramGM2XresiVu2d; 
  delete histogramGM2YresiVu2d; 
  
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

  delete histogramSI1XclNoise;
  delete histogramSI1YclNoise;
  delete histogramSI2XclNoise;
  delete histogramSI2YclNoise;
  delete histogramGM1XclNoise;
  delete histogramGM1YclNoise;
  delete histogramGM2XclNoise;
  delete histogramGM2YclNoise;

  delete histogramSI1Xhitpoint;
  delete histogramSI1Yhitpoint;
  delete histogramSI2Xhitpoint;
  delete histogramSI2Yhitpoint;
  delete histogramGM1Xhitpoint;
  delete histogramGM1Yhitpoint;
  delete histogramGM2Xhitpoint;
  delete histogramGM2Yhitpoint;

  delete histogramSI1XunBiResi;
  delete histogramSI1YunBiResi;
  delete histogramSI2XunBiResi;
  delete histogramSI2YunBiResi;
  delete histogramGM1XunBiResi;
  delete histogramGM1YunBiResi;
 
  delete histogramSI1XunBiResiVu2d; 
  delete histogramSI1YunBiResiVu2d; 
  delete histogramSI2XunBiResiVu2d; 
  delete histogramSI2YunBiResiVu2d; 
  delete histogramGM1XunBiResiVu2d; 
  delete histogramGM1YunBiResiVu2d; 
  delete histogramGM2XunBiResiVu2d; 
  delete histogramGM2YunBiResiVu2d; 

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

  delete histogramSI1XtrackErr;
  delete histogramSI1YtrackErr;
  delete histogramSI2XtrackErr;
  delete histogramSI2YtrackErr;
  delete histogramGM1XtrackErr;
  delete histogramGM1YtrackErr;
  delete histogramGM2XtrackErr;
  delete histogramGM2YtrackErr;

  delete histogramSI1XtrackErrVu2d;
  delete histogramSI1YtrackErrVu2d;
  delete histogramSI2XtrackErrVu2d;
  delete histogramSI2YtrackErrVu2d;
  delete histogramGM1XtrackErrVu2d;
  delete histogramGM1YtrackErrVu2d;
  delete histogramGM2XtrackErrVu2d;
  delete histogramGM2YtrackErrVu2d;

  delete histogramSI1XampRatio;
  delete histogramSI1YampRatio;
  delete histogramSI2XampRatio;
  delete histogramSI2YampRatio;
  delete histogramGM1XampRatio;
  delete histogramGM1YampRatio;
  delete histogramGM2XampRatio;
  delete histogramGM2YampRatio;
}
