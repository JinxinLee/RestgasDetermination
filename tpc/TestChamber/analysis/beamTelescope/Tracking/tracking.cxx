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

  TH1D *histogramSI1XU = new TH1D("U SI1X", "U SI1X", 400, 0,2);
  TH1D *histogramSI1YU = new TH1D("U SI1Y", "U SI1Y", 400, 0,2);
  TH1D *histogramSI2XU = new TH1D("U SI2X", "U SI2X", 400, 0,2);
  TH1D *histogramSI2YU = new TH1D("U SI2Y", "U SI2Y", 400, 0,2);
  TH1D *histogramGM1XU = new TH1D("U GM1X", "U GM1X", 400, 0,10.3);
  TH1D *histogramGM1YU = new TH1D("U GM1Y", "U GM1Y", 400, 0,10.3);
  TH1D *histogramGM2XU = new TH1D("U GM2X", "U GM2X", 400, 0,10.3);
  TH1D *histogramGM2YU = new TH1D("U GM2Y", "U GM2Y", 400, 0,10.3);

  TH1D *histogramSI1Xerr = new TH1D("err SI1X", "err SI1X", 400, 0,0.01);
  TH1D *histogramSI1Yerr = new TH1D("err SI1Y", "err SI1Y", 400, 0,0.01);
  TH1D *histogramSI2Xerr = new TH1D("err SI2X", "err SI2X", 400, 0,0.01);
  TH1D *histogramSI2Yerr = new TH1D("err SI2Y", "err SI2Y", 400, 0,0.01);
  TH1D *histogramGM1Xerr = new TH1D("err GM1X", "err GM1X", 400, 0,0.05);
  TH1D *histogramGM1Yerr = new TH1D("err GM1Y", "err GM1Y", 400, 0,0.05);
  TH1D *histogramGM2Xerr = new TH1D("err GM2X", "err GM2X", 400, 0,0.5);
  TH1D *histogramGM2Yerr = new TH1D("err GM2Y", "err GM2Y", 400, 0,0.5);

  TH1D *histogramSI1XhitpointBbest = new TH1D("brute best hitpoint SI1X", "brute best hitpoint SI1X", 200, 10,20);
  TH1D *histogramSI1YhitpointBbest = new TH1D("brute best hitpoint SI1Y", "brute best hitpoint SI1Y", 200, 20,30);
  TH1D *histogramSI2XhitpointBbest = new TH1D("brute best hitpoint SI2X", "brute best hitpoint SI2X", 200, 10,20);
  TH1D *histogramSI2YhitpointBbest = new TH1D("brute best hitpoint SI2Y", "brute best hitpoint SI2Y", 200, 20,30);
  TH1D *histogramGM1XhitpointBbest = new TH1D("brute best hitpoint GM1X", "brute best hitpoint GM1X", 200, 5, 25);
  TH1D *histogramGM1YhitpointBbest = new TH1D("brute best hitpoint GM1Y", "brute best hitpoint GM1Y", 200, 15,30);
  TH1D *histogramGM2XhitpointBbest = new TH1D("brute best hitpoint GM2X", "brute best hitpoint GM2X", 200, 5, 25);
  TH1D *histogramGM2YhitpointBbest = new TH1D("brute best hitpoint GM2Y", "brute best hitpoint GM2Y", 200, 15,30);

  TH1D *histogramSI1XUbBest = new TH1D("brute best U SI1X", "brute best U SI1X", 400, 0,2);
  TH1D *histogramSI1YUbBest = new TH1D("brute best U SI1Y", "brute best U SI1Y", 400, 0,2);
  TH1D *histogramSI2XUbBest = new TH1D("brute best U SI2X", "brute best U SI2X", 400, 0,2);
  TH1D *histogramSI2YUbBest = new TH1D("brute best U SI2Y", "brute best U SI2Y", 400, 0,2);
  TH1D *histogramGM1XUbBest = new TH1D("brute best U GM1X", "brute best U GM1X", 400, 0,10.3);
  TH1D *histogramGM1YUbBest = new TH1D("brute best U GM1Y", "brute best U GM1Y", 400, 0,10.3);
  TH1D *histogramGM2XUbBest = new TH1D("brute best U GM2X", "brute best U GM2X", 400, 0,10.3);
  TH1D *histogramGM2YUbBest = new TH1D("brute best U GM2Y", "brute best U GM2Y", 400, 0,10.3);
  
  TH1D *histogramSI1Xresi = new TH1D("resSI1X", "resSI1X", 400, -0.3, 0.3);
  TH1D *histogramSI1Yresi = new TH1D("resSI1Y", "resSI1Y", 400, -0.3,0.3);
  TH1D *histogramSI2Xresi = new TH1D("resSI2X", "resSI2X", 400, -0.3,0.3);
  TH1D *histogramSI2Yresi = new TH1D("resSI2Y", "resSI2Y", 400, -0.3,0.3);
  TH1D *histogramGM1Xresi = new TH1D("resGM1X", "resGM1X", 400, -3, 3);
  TH1D *histogramGM1Yresi = new TH1D("resGM1Y", "resGM1Y", 400, -3, 3);
  TH1D *histogramGM2Xresi = new TH1D("resGM2X", "resGM2X", 400, -3, 3);
  TH1D *histogramGM2Yresi = new TH1D("resGM2Y", "resGM2Y", 400, -3, 3);

  TProfile *histogramSI1XresiVu = new TProfile("resVuSI1X", "resVuSI1X", 400, 0,2,-10,10);
  TProfile *histogramSI1YresiVu = new TProfile("resVuSI1Y", "resVuSI1Y", 400, 0,2,-10,10);
  TProfile *histogramSI2XresiVu = new TProfile("resVuSI2X", "resVuSI2X", 400, 0,2,-10,10);
  TProfile *histogramSI2YresiVu = new TProfile("resVuSI2Y", "resVuSI2Y", 400, 0,2,-10,10);
  TProfile *histogramGM1XresiVu = new TProfile("resVuGM1X", "resVuGM1X", 1000, 0,10,-10,10);
  TProfile *histogramGM1YresiVu = new TProfile("resVuGM1Y", "resVuGM1Y", 1000, 0,10,-10,10);
  TProfile *histogramGM2XresiVu = new TProfile("resVuGM2X", "resVuGM2X", 1000, 0,10,-10,10);
  TProfile *histogramGM2YresiVu = new TProfile("resVuGM2Y", "resVuGM2Y", 1000, 0,10,-10,10);

  TH2D *histogramSI1XresiVu2d = new TH2D("resVu2dSI1X", "resVu2dSI1X", 200, 0,2,1000, -0.2,0.2);
  TH2D *histogramSI1YresiVu2d = new TH2D("resVu2dSI1Y", "resVu2dSI1Y", 200, 0,2,1000, -0.2,0.2);
  TH2D *histogramSI2XresiVu2d = new TH2D("resVu2dSI2X", "resVu2dSI2X", 200, 0,2,1000, -0.2,0.2);
  TH2D *histogramSI2YresiVu2d = new TH2D("resVu2dSI2Y", "resVu2dSI2Y", 200, 0,2,1000, -0.2,0.2);
  TH2D *histogramGM1XresiVu2d = new TH2D("resVu2dGM1X", "resVu2dGM1X", 1000, 0,10,1000, -10,10);
  TH2D *histogramGM1YresiVu2d = new TH2D("resVu2dGM1Y", "resVu2dGM1Y", 1000, 0,10,1000, -10,10);
  TH2D *histogramGM2XresiVu2d = new TH2D("resVu2dGM2X", "resVu2dGM2X", 1000, 0,10,1000, -10,10);
  TH2D *histogramGM2YresiVu2d = new TH2D("resVu2dGM2Y", "resVu2dGM2Y", 1000, 0,10,1000, -10,10);
  
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
    if(i_ev%250==0){
      std::cout<<i_ev<<", nTrackTries: "<<n_tracks<<", To Many cl: "<<nToMany<< ", Empty: "<<nZero<<", noiseCut: "<<noiseCut<<", ratioCut: "<<ratioCut<<"********************************************************************"<<std::endl;
    }
    tree->GetEntry(i_ev);
    event->clear();
    TCtrack* store=NULL;
    std::vector<TCcluster> clSI1X;
    std::vector<TCcluster> clSI1Y;
    std::vector<TCcluster> clSI2X;
    std::vector<TCcluster> clSI2Y;
    std::vector<TCcluster> clGM1X;
    std::vector<TCcluster> clGM1Y;
    std::vector<TCcluster> clGM2X;
    std::vector<TCcluster> clGM2Y;
    ampDiffCut(SI01X1->GetClusters(), clSI1X, 5,3,alignmentFile, noiseCut,1.2,2);
    ampDiffCut(SI01Y1->GetClusters(), clSI1Y, 5,4,alignmentFile, noiseCut);
    ampDiffCut(SI02X1->GetClusters(), clSI2X, 5,5,alignmentFile, noiseCut,0.,0.8);
    ampDiffCut(SI02Y1->GetClusters(), clSI2Y, 5,6,alignmentFile, noiseCut);
    ampRatioCut(GM01X1->GetClusters(), clGM1X, 0.9,1.1,1,alignmentFile, ratioCut,5,7.5);
    ampRatioCut(GM01Y1->GetClusters(), clGM1Y, 1.1,1.1,2,alignmentFile, ratioCut,4.8,7.3);
    ampRatioCut(GM02X1->GetClusters(), clGM2X, 0.2,0.9,7,alignmentFile, ratioCut);
    ampRatioCut(GM02Y1->GetClusters(), clGM2Y, 0.3,0.9,8,alignmentFile, ratioCut);
    if(hist){
      histogramSI1X->Fill(clSI1X.size());
      histogramSI1Y->Fill(clSI1Y.size());    
      histogramSI2X->Fill(clSI2X.size());
      histogramSI2Y->Fill(clSI2Y.size());    
      histogramGM1X->Fill(clGM2X.size());
      histogramGM1Y->Fill(clGM2Y.size());   
      histogramGM2X->Fill(clGM1X.size());
      histogramGM2Y->Fill(clGM1Y.size());
      histogramEvent->Fill(event->nClusters());
    }
    vector<TCcluster> startClusters;
    vector<TCcluster> endClusters;
    for(unsigned int i=0;i<clGM1X.size();++i){
      for(unsigned int j=0;j<clGM1Y.size();++j){
	double ratio=clGM1X.at(i).getAmp()/clGM1Y.at(j).getAmp();
        double ratio2=0;
        if(hist){
          histogramGM1XYratio->Fill(ratio);
          histogramGM1YXratio->Fill(1/ratio);
        }
        double x = clGM1X.at(i).getAmp();    double y = clGM1Y.at(j).getAmp()*1.05;
        if(x>y){
          ratio2=y/x;
        }else{
          ratio2=x/y;
        }
        if(hist){
          histogramGM1SmallBigScaledRatio->Fill(ratio2);
          histogramGM1YXScaledRatio->Fill(y/x);
          histogramGM1XYScaledRatio->Fill(x/y);
        }
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
        if(hist){
          histogramGM2XYratio->Fill(ratio);
          histogramGM2YXratio->Fill(1/ratio);
        }
        double x = clGM2X.at(i).getAmp();
        double y = clGM2Y.at(j).getAmp()*1.1;
        double ratio2=0;
        if(x>y){
          ratio2=y/x;
        }else{
          ratio2=x/y;
        }
        if(hist){
          histogramGM2SmallBigScaledRatio->Fill(ratio2);
          histogramGM2YXScaledRatio->Fill(y/x);
          histogramGM2XYScaledRatio->Fill(x/y);
        }
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

    if(!brute&&(clSI1X.size()>5||clSI1Y.size()>5||clSI2X.size()>5||clSI2Y.size()>5||startClusters.size()>10)){
      nToMany++;
      if(!hist){
        continue;
      }
    }
    if(clSI1X.size()==0|| clSI1Y.size()==0||clSI2X.size()==0||clSI2Y.size()==0||startClusters.size()==0){
      nZero++;
      if(!hist){
        continue;
      }
    }
    histogramStart->Fill(startClusters.size()/2);
    histogramEnd->Fill(endClusters.size()/2);
    int n_points=0;//temporary index for filling TGraph
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();
    vector<TCcluster> clXZ;
    vector<TCcluster> clYZ;
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i;
      clusterFiller(startClusters.at(id),clXZ,histogramGM1Xhitpoint,histogramGM1XU, histogramGM1Xerr,x_event,n_points, true);
    }
    for(unsigned int i=0;i<clSI1X.size();++i){
      clusterFiller(clSI1X.at(i),clXZ,histogramSI1Xhitpoint,histogramSI1XU, histogramSI1Xerr,x_event,n_points, true);
    }
    for(unsigned int i=0;i<clSI2X.size();++i){
      clusterFiller(clSI2X.at(i),clXZ, histogramSI2Xhitpoint,histogramSI2XU, histogramSI2Xerr,x_event,n_points, true);
    }
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i;
      clusterFiller(endClusters.at(id),clXZ,histogramGM2Xhitpoint,histogramGM2XU, histogramGM2Xerr,x_event,n_points, true);
    }
    n_points=0;
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i+1;
      clusterFiller(startClusters.at(id),clYZ,histogramGM1Yhitpoint,histogramGM1YU, histogramGM1Yerr,y_event,n_points, false);
    }
    for(unsigned int i=0;i<clSI1Y.size();++i){
      clusterFiller(clSI1Y.at(i),clYZ,histogramSI1Yhitpoint,histogramSI1YU, histogramSI1Yerr,y_event,n_points, false);
    }
    for(unsigned int i=0;i<clSI2Y.size();++i){
      clusterFiller(clSI2Y.at(i),clYZ,histogramSI2Yhitpoint,histogramSI2YU, histogramSI2Yerr,y_event,n_points, false);
    }
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i+1;
      clusterFiller(endClusters.at(id),clYZ,histogramGM2Yhitpoint,histogramGM2YU, histogramGM2Yerr,y_event,n_points, false);
    }
    if(fit&&!brute){
      cout<<"befor"<<endl;
      houghYZ->make(clYZ,3);
      houghXZ->make(clXZ,3);
      cout<<"after"<<endl;
    }
    if(fit&&brute){
      double chiStore=99999999;
      for(unsigned int i=0;i<startClusters.size()/2;i++){
        for(unsigned int j=0;j<clSI1X.size();++j){
           for(unsigned int k=0;k<clSI2X.size();++k){
              for(unsigned int l=0;l<clSI1Y.size();++l){
                for(unsigned int m=0;m<clSI2Y.size();++m){
                  /*
                    //taking gm2 into account better without
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
                       track.fit(1,2,3,4,5,6);
                       if(track.getChi2()/track.getNDF()<chiStore){
                         store=track;
                         chiStore=track.getChi2()/track.getNDF();
                       }
                     }//end endClusters l 2n and 2n+1
                   }else{
                  */

                  int id1X =2*i;
                  int id1Y =2*i+1;
                  TCtrack* track=new TCtrack();
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
                  track->addClusters(trackCl);
                  track->fit(1,2,3,4,5,6);
                  if(track->getChi2()/track->getNDF()<chiStore){
                    store=track;
                    chiStore=track->getChi2()/track->getNDF();
                  } 
                 
                } //end clSI2Y m
              } //end clSI1Y l
           } // end clSI2X k
        } //end clSI1X j
      } // end startClusters i 2i and 2i+1
      if(store!=NULL){
        //        store->Print();
        event->addTrack(store);
      }
      histogramChi2->Fill(store->getChi2()/store->getNDF());
      histogramChi2rough->Fill(store->getChi2()/store->getNDF());
      histogramNDF->Fill(store->getNDF());
      for(unsigned int i=0;i<store->nCl();++i){
        TCcluster tmpcl = store->getCl(i);
        double uCl=tmpcl.posUVW().x();                   
        TVector3 resid=tmpcl.getRes();
        if(tmpcl.getFit()){
            switch(tmpcl.getId()){
            case 1:
              histogramGM1Xresi->Fill(resid.x());
              histogramGM1XresiVu->Fill(uCl,resid.x());
              histogramGM1XresiVu2d->Fill(uCl,resid.x());
              histogramGM1XUbBest->Fill(uCl);
              histogramGM1XhitpointBbest->Fill(tmpcl.posXYZ().x());
              break;
            case 2:
              histogramGM1Yresi->Fill(resid.x());
              histogramGM1YresiVu->Fill(uCl,resid.x());
              histogramGM1YresiVu2d->Fill(uCl,resid.x());
              histogramGM1YUbBest->Fill(uCl);
              histogramGM1YhitpointBbest->Fill(tmpcl.posXYZ().y());
              break;
            case 3:
              histogramSI1Xresi->Fill(resid.x());
              histogramSI1XresiVu->Fill(uCl,resid.x());
              histogramSI1XresiVu2d->Fill(uCl,resid.x());
              histogramSI1XUbBest->Fill(uCl);
              histogramSI1XhitpointBbest->Fill(tmpcl.posXYZ().x());
              break;
            case 4:                         
              histogramSI1Yresi->Fill(resid.x());
              histogramSI1YresiVu->Fill(uCl,resid.x());
              histogramSI1YresiVu2d->Fill(uCl,resid.x());
              histogramSI1YUbBest->Fill(uCl);
              histogramSI1YhitpointBbest->Fill(tmpcl.posXYZ().y());
              break;
            case 5:
              histogramSI2Xresi->Fill(resid.x());
              histogramSI2XresiVu->Fill(uCl,resid.x());
              histogramSI2XresiVu2d->Fill(uCl,resid.x());
              histogramSI2XUbBest->Fill(uCl);
              histogramSI2XhitpointBbest->Fill(tmpcl.posXYZ().x());
              break;
            case 6:
              histogramSI2Yresi->Fill(resid.x());
              histogramSI2YresiVu->Fill(uCl,resid.x());
              histogramSI2YresiVu2d->Fill(uCl,resid.x());
              histogramSI2YUbBest->Fill(uCl);
              histogramSI2YhitpointBbest->Fill(tmpcl.posXYZ().y());
              break;
            case 7:
              histogramGM2Xresi->Fill(resid.x());
              histogramGM2XresiVu->Fill(uCl,resid.x());
              histogramGM2XresiVu2d->Fill(uCl,resid.x());
              histogramGM2XUbBest->Fill(uCl);
              histogramGM2XhitpointBbest->Fill(tmpcl.posXYZ().x());
              break;
            case 8:
              histogramGM2Yresi->Fill(resid.x());
              histogramGM2YresiVu->Fill(uCl,resid.x());
              histogramGM2YresiVu2d->Fill(uCl,resid.x());
              histogramGM2YUbBest->Fill(uCl);
              histogramGM2YhitpointBbest->Fill(tmpcl.posXYZ().y());
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
        //        event->addTrack(track1);
        //        event->addClusters(clTrack1);

        histogramChi2->Fill(track2->getChi2()/track2->getNDF());
        histogramChi2rough->Fill(track2->getChi2()/track2->getNDF());
        histogramNDF->Fill(track2->getNDF());
        n_tracks++;
        //        event->addTrack(track2);
        //        event->addClusters(clTrack2);
        
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
    if(event->nTracks()==0){
       continue;
    }
    //cout<<"before fill"<<endl;
    eventTreeOut->Fill();
    //    cout<<"before fill"<<endl;
  }//end looping of eventTree

  // histogramTrack->Fill(event->nTracks());  
  //  
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

  histogramSI1XresiVu2d->Write(); 
  histogramSI1YresiVu2d->Write(); 
  histogramSI2XresiVu2d->Write(); 
  histogramSI2YresiVu2d->Write(); 
  histogramGM1XresiVu2d->Write(); 
  histogramGM1YresiVu2d->Write(); 
  histogramGM2XresiVu2d->Write(); 
  histogramGM2YresiVu2d->Write(); 

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

  file->Close();

  delete histogramSI1Xerr;
  delete histogramSI1Yerr;
  delete histogramSI2Xerr;
  delete histogramSI2Yerr;
  delete histogramGM1Xerr;
  delete histogramGM1Yerr;
  delete histogramGM2Xerr;
  delete histogramGM2Yerr;

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



