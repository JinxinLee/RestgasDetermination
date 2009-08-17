#include <list>
#include <iostream>
#include <string>
#include <cassert>
#include <signal.h>
#include <TApplication.h>
#include <TROOT.h>
#include <TSystem.h>

#include <TObject.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1I.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TGraph2D.h>
#include <TStyle.h>
#include <Math/Polynomial.h>
#include <TMultiGraph.h>

#include "../../../src/TChough2.h"
#include "../../../src/TCcluster.h"
#include "../../../src/TCalign.h"
#include "../../../src/TCtrack.h"
#include "../../../src/TCevent.h"
#include "../Hits.h"
#include "helpers.h"

static bool controlC=false;
static int  controlCs=0;
static bool disp=false;
static bool next_event=false;

void signalHandler2(int signal) {
  if (signal==SIGINT) {
    if(!disp){
      std::cout<<"CTRL + C pressed saving and exiting, press again to abort"<<std::endl;
      controlC = true;
      if (controlCs!=0) {
	std::cout<<"aborting"<<std::endl;
	exit(1);
      }
      controlCs++;
    }else{
      std::cout<<"disp terminate"<<std::endl;
      std::cout<<"CTRL + C pressed saving and exiting, press again to abort"<<std::endl;
      next_event=true;
      if (controlCs!=0) {
	std::cout<<"aborting"<<std::endl;
	exit(1);
      }
      controlCs++;
    }
  }
}

int main(int argc,char **argv){
  using std::vector;
  using std::cout;
  using std::cerr;
  using std::cin;
  using std::endl;
  using std::list;
  using std::string;
  
  signal(SIGINT, signalHandler2);

  if(!(argc==4||argc==5)){
    cerr<<"Wrong number of arguments, "<<argc<<endl
        <<"Syntax should be ./tracker infile alignmentfile mode histfile"<<endl;
    throw;
  }
  TApplication theApp("theApp",NULL,NULL);
  HistContainer* histContainer = new HistContainer();
  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("Hits");
  string alignmentFile = argv[2];
  string outHist = "clusterMultipCut.root";
  if(argc==5){
    outHist=argv[4];
  }
  bool fit=true;
  bool brute=false;
  bool hist=false;
  bool twoPoint=false;
  bool bruteMax=false;
  string mode(argv[3]);
  if(mode == "disp"){
    disp = true;
    fit = true;
    brute = true;
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
  }else if(mode == "twoPoint"){
    cout<<mode<<endl;
    disp = false;
    fit = true;
    twoPoint = true;
    brute = false;
  }else if(mode == "bruteMax"){
    bruteMax=true;
    fit=true;
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


  int nEvents=tree->GetEntries();
  TCevent* event = new TCevent();
  TFile* trackOutFile = new TFile("tracks.root","RECREATE");
  TTree* eventTreeOut = new TTree("at2","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&event,32000,99);
  /*
    Creating an instance of TCallign to assure wich alignmentfile is used
  */
  cout<<"before alignment"<<endl;
  TCalign* a = TCalign::getInstance(alignmentFile);
  a->clear();
  a->read(alignmentFile);
    
  TVector3 X(1.,0.,0.);
  TVector3 Y(0.,1.,0.);
  TVector3 Z(0.,0.,1.);
  TChough2* houghYZ = new TChough2( Y,Z,500,500,
				    0.15,0.02,
				    -0.5,0.5,
				    -5,5);
  TChough2* houghXZ = new TChough2(-X,Z,500,500,
				   0.15,0.02,
				   -0.5,0.5,-5,5);
  int n_tracks = 0;
  int nToMany=0;
  int nZero=0;
  int noiseCut=0;
  int ratioCut=0;
  int nClusters=0;
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%250==0){
      std::cout<<i_ev<<" n clusters "<<nClusters<<", nTrackTries: "<<n_tracks<<", To Many cl: "<<nToMany<< ", Empty: "<<nZero<<", noiseCut: "<<noiseCut<<", ratioCut: "<<ratioCut<<"********************************************************************"<<std::endl;
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
    //cout<<"befoore cuts"<<endl;
    ampRatioNoiseCut(SI01X1->GetClusters(), clSI1X, 
		     1.1,0.8,7,
		     3,alignmentFile, 
		     ratioCut,histContainer,0.3,1.65);

    ampRatioNoiseCut(SI01Y1->GetClusters(), clSI1Y, 
		     1.1,0.8,7,
		     4,alignmentFile, 
		     ratioCut,histContainer,0.3,1.5);

    ampRatioNoiseCut(SI02X1->GetClusters(), clSI2X, 
		     1.1,0.9,7,
		     5,alignmentFile, 
		     ratioCut,histContainer,0.2,1.55);
    ampRatioNoiseCut(SI02Y1->GetClusters(), clSI2Y, 
		     1.1,0.9,7,
		     6,alignmentFile, 
		     ratioCut,histContainer,0.4,1.55);
    ampRatioNoiseCut(GM01X1->GetClusters(), clGM1X, 
		     0.9,0.55,7,
		     1,alignmentFile, 
		     ratioCut,histContainer,4.2,5.65);
    ampRatioNoiseCut(GM01Y1->GetClusters(), clGM1Y, 
		     0.9,0.45,7,
		     2,alignmentFile, 
		     ratioCut,histContainer,5.5,6.9);
    ampRatioNoiseCut(GM02X1->GetClusters(), clGM2X,
		     1.1,0.9,7,
		     7,alignmentFile, 
		     ratioCut,histContainer,2.1,3.6);
    ampRatioNoiseCut(GM02Y1->GetClusters(), clGM2Y,
		     1.1,0.9,7,
		     8,alignmentFile, 
		     ratioCut,histContainer,5.55,6.85);
    //    cout<<"after cuts"<<endl;
    /*
    ampDiffCut(clSI1X, 5,3,alignmentFile, noiseCut,histContainer,1.2,2);
    ampDiffCut(clSI1Y, 5,4,alignmentFile, noiseCut,histContainer);
    ampDiffCut(clSI2X, 5,5,alignmentFile, noiseCut,histContainer,0.,0.8);
    ampDiffCut(clSI2Y, 5,6,alignmentFile, noiseCut,histContainer);
    ampDiffCut(clGM1X, 5,1,alignmentFile, ratioCut,histContainer,5,7.5);
    ampDiffCut(clGM1Y, 5,2,alignmentFile, ratioCut,histContainer,4.8,7.3);
    ampDiffCut(clGM2X, 5,7,alignmentFile, ratioCut,histContainer);
    ampDiffCut( clGM2Y, 5,8,alignmentFile, ratioCut,histContainer);
    */   

    if(hist){
      histContainer->histogramSI1X->Fill(clSI1X.size());
      histContainer->histogramSI1Y->Fill(clSI1Y.size());    
      histContainer->histogramSI2X->Fill(clSI2X.size());
      histContainer->histogramSI2Y->Fill(clSI2Y.size());    
      histContainer->histogramGM1X->Fill(clGM2X.size());
      histContainer->histogramGM1Y->Fill(clGM2Y.size());   
      histContainer->histogramGM2X->Fill(clGM1X.size());
      histContainer->histogramGM2Y->Fill(clGM1Y.size());

      histContainer->histogramNhitsSI1X->Fill(SI01X1->GetNhit());
      histContainer->histogramNhitsSI1Y->Fill(SI01Y1->GetNhit());    
      histContainer->histogramNhitsSI2X->Fill(SI02X1->GetNhit());
      histContainer->histogramNhitsSI2Y->Fill(SI02Y1->GetNhit());    
      histContainer->histogramNhitsGM1X->Fill(GM02X1->GetNhit());
      histContainer->histogramNhitsGM1Y->Fill(GM02Y1->GetNhit());   
      histContainer->histogramNhitsGM2X->Fill(GM01X1->GetNhit());
      histContainer->histogramNhitsGM2Y->Fill(GM01Y1->GetNhit());

      histContainer->histogramNclustersSI1X->Fill(SI01X1->GetNcluster());
      histContainer->histogramNclustersSI1Y->Fill(SI01Y1->GetNcluster());    
      histContainer->histogramNclustersSI2X->Fill(SI02X1->GetNcluster());
      histContainer->histogramNclustersSI2Y->Fill(SI02Y1->GetNcluster());    
      histContainer->histogramNclustersGM1X->Fill(GM02X1->GetNcluster());
      histContainer->histogramNclustersGM1Y->Fill(GM02Y1->GetNcluster());   
      histContainer->histogramNclustersGM2X->Fill(GM01X1->GetNcluster());
      histContainer->histogramNclustersGM2Y->Fill(GM01Y1->GetNcluster());

      nClusters=nClusters+clSI1X.size()+clSI1Y.size()+clSI2X.size()+clSI2Y.size()+clGM2X.size()+clGM2Y.size()+clGM1X.size()+clGM1Y.size();
      histContainer->histogramEvent->Fill(event->nClusters());
    }
    vector<TCcluster> startClusters;
    vector<TCcluster> endClusters;
    for(unsigned int i=0;i<clGM1X.size();++i){
      for(unsigned int j=0;j<clGM1Y.size();++j){
	double ratio=clGM1X.at(i).getAmp()/clGM1Y.at(j).getAmp();
        double ratio2=0;
        if(hist){
          histContainer->histogramGM1XYratio->Fill(ratio);
          histContainer->histogramGM1YXratio->Fill(1/ratio);
        }
        double x = clGM1X.at(i).getAmp()*1.05;    double y = clGM1Y.at(j).getAmp();
        if(x>y){
          ratio2=y/x;
        }else{
          ratio2=x/y;
        }
        if(hist){
          histContainer->histogramGM1SmallBigScaledRatio->Fill(ratio2);
          histContainer->histogramGM1YXScaledRatio->Fill(y/x);
          histContainer->histogramGM1XYScaledRatio->Fill(x/y);
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
          histContainer->histogramGM2XYratio->Fill(ratio);
          histContainer->histogramGM2YXratio->Fill(1/ratio);
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
          histContainer->histogramGM2SmallBigScaledRatio->Fill(ratio2);
          histContainer->histogramGM2YXScaledRatio->Fill(y/x);
          histContainer->histogramGM2XYScaledRatio->Fill(x/y);
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
      /*
      if(!hist){
        continue;
      }
      */
    }
    if(clSI1X.size()==0||clSI1Y.size()==0||clSI2X.size()==0||clSI2Y.size()==0||startClusters.size()==0||endClusters.size()==0){
      nZero++;
      if(!hist){
        continue;
      }
    }
    histContainer->histogramStart->Fill(startClusters.size()/2);
    histContainer->histogramEnd->Fill(endClusters.size()/2);
    int n_points=0;//temporary index for filling TGraph2D
    TGraph2D* x_eventAmp =new TGraph2D();
    TGraph2D* y_eventAmp =new TGraph2D();
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();
    vector<TCcluster> clXZ;
    vector<TCcluster> clYZ;
    //X cluster ***************************************************************************************
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i;
      histContainer->hitmapGM1->Fill(startClusters.at(id).posUVW().x(),
				     startClusters.at(id+1).posUVW().x());
      clusterFiller(startClusters.at(id),clXZ,histContainer->histogramGM1Xhitpoint,
		    histContainer->histogramGM1XU, histContainer->histogramGM1Xerr,
		    x_eventAmp, x_event, n_points, true);
    }
    for(unsigned int i=0;i<clSI1X.size();++i){
      clusterFiller(clSI1X.at(i),clXZ,histContainer->histogramSI1Xhitpoint,
		    histContainer->histogramSI1XU, histContainer->histogramSI1Xerr,
		    x_eventAmp, x_event,  n_points, true);
      if(hist){
	for(unsigned int j=0;j<clSI1Y.size();++j){
	  histContainer->hitmapSI1->Fill(clSI1X.at(i).posUVW().x(),clSI1Y.at(j).posUVW().x());
	}
      }
    }
  
    for(unsigned int i=0;i<clSI2X.size();++i){
      clusterFiller(clSI2X.at(i),clXZ, histContainer->histogramSI2Xhitpoint,
		    histContainer->histogramSI2XU, histContainer->histogramSI2Xerr,
		    x_eventAmp, x_event,  n_points, true);
      if(hist){
	for(unsigned int j=0;j<clSI2Y.size();++j){
	  histContainer->hitmapSI2->Fill(clSI2X.at(i).posUVW().x(),clSI2Y.at(j).posUVW().x());
	}
      }
    }
   for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i;
      histContainer->hitmapGM2->Fill(endClusters.at(id).posUVW().x(),
				     endClusters.at(id+1).posUVW().x());
      clusterFiller(endClusters.at(id),clXZ,histContainer->histogramGM2Xhitpoint,
                    histContainer->histogramGM2XU, histContainer->histogramGM2Xerr,
		    x_eventAmp, x_event, n_points, true);
    }
   //Y clusters **********************************************************************************************
    n_points=0;
    for(unsigned int i=0;i<startClusters.size()/2;i++){
      int id =2*i+1;
      clusterFiller(startClusters.at(id),clYZ,histContainer->histogramGM1Yhitpoint,
                    histContainer->histogramGM1YU, histContainer->histogramGM1Yerr,
		    y_eventAmp, y_event,n_points, false);
    }
    for(unsigned int j=0;j<clSI1Y.size();++j){
      clusterFiller(clSI1Y.at(j),clYZ,histContainer->histogramSI1Yhitpoint,
			histContainer->histogramSI1YU, histContainer->histogramSI1Yerr,
			y_eventAmp, y_event,n_points, false);
    } 
    for(unsigned int j=0;j<clSI2Y.size();++j){
      clusterFiller(clSI2Y.at(j),clYZ,histContainer->histogramSI2Yhitpoint,
		    histContainer->histogramSI2YU, histContainer->histogramSI2Yerr,
		    y_eventAmp, y_event,n_points, false);
    }
 
    for(unsigned int i=0;i<endClusters.size()/2;i++){
      int id =2*i+1;
      clusterFiller(endClusters.at(id),clYZ,histContainer->histogramGM2Yhitpoint,
                    histContainer->histogramGM2YU, histContainer->histogramGM2Yerr,
		    y_eventAmp, y_event,n_points, false);
    }
    
    if(fit&&!brute&&!twoPoint&&!bruteMax){
      cout<<"befor1"<<endl;
      houghYZ->make(clYZ,2);
      cout<<"befor2"<<endl;
      houghXZ->make(clXZ,2);
      cout<<"befor3"<<endl;


    }


 if(fit&&bruteMax) {
      
   double maxAmpSI1x=-1;
   double maxAmpSI1y=-1;
   double maxAmpSI2x=-1;
   double maxAmpSI2y=-1;
   double maxAmpGM1x=-1;
   double maxAmpGM1y=-1;
   double maxAmpGM2x=-1;
   double maxAmpGM2y=-1;

   int maxPosSI1x=-1;
   int maxPosSI1y=-1;
   int maxPosSI2x=-1;
   int maxPosSI2y=-1;
   int maxPosGM1x=-1;
   int maxPosGM1y=-1;
   int maxPosGM2x=-1;
   int maxPosGM2y=-1;
   for(unsigned int endI=0;endI<endClusters.size()/2;++endI){
	if(next_event){
	  continue;
	}
	for(unsigned int i=0;i<startClusters.size()/2;i++){
	  if(next_event){
	    continue;
	  }
	  for(unsigned int j=0;j<clSI1X.size();++j){
	    if(next_event){
	      continue;
	    }
	    for(unsigned int k=0;k<clSI2X.size();++k){
	      if(next_event){
		continue;
	      }
              for(unsigned int l=0;l<clSI1Y.size();++l){
		if(next_event){
		  continue;
		}
                for(unsigned int m=0;m<clSI2Y.size();++m){
		  if(next_event){
		    continue;
		  }
                  int id1X =2*i;
                  int id1Y =2*i+1;
                  int id2X =2*endI;
                  int id2Y =2*endI+1;
		  if(startClusters.at(id1X).getAmp()>maxAmpGM1x){
		    maxAmpGM1x=startClusters.at(id1X).getAmp();
		    maxPosGM1x=id1X;
		  }
		  if(startClusters.at(id1Y).getAmp()>maxAmpGM1y){
		    maxAmpGM1y=startClusters.at(id1Y).getAmp();
		    maxPosGM1y=id1Y;
		  }
		  if(endClusters.at(id2X).getAmp()>maxAmpGM2x){
		    maxAmpGM2x=endClusters.at(id2X).getAmp();
		    maxPosGM2x=id2X;
		  }
		  if(endClusters.at(id2Y).getAmp()>maxAmpGM2y){
		    maxAmpGM2y=endClusters.at(id2Y).getAmp();
		    maxPosGM2y=id2Y;
		  }
		  if(clSI1X.at(j).getAmp()>maxAmpSI1x){
		    maxAmpSI1x=clSI1X.at(j).getAmp();
		    maxPosSI1x=j;
		  }
		  if(clSI2X.at(k).getAmp()>maxAmpSI2x){
		    maxAmpSI2x=clSI2X.at(k).getAmp();
		    maxPosSI2x=k;
		  }
		  if(clSI1Y.at(l).getAmp()>maxAmpSI1y){
		    maxAmpSI1y=clSI1Y.at(l).getAmp();
		    maxPosSI1y=l;
		  }
		  if(clSI2Y.at(m).getAmp()>maxAmpSI2y){
		    maxAmpSI2y=clSI2Y.at(m).getAmp();
		    maxPosSI2y=m;
		  }


		} //end clSI2Y m
	      } //end clSI1Y l
	    } // end clSI2X k
	  } //end clSI1X j
	} // end startClusters i 2i and 2i+1
   }//end end clusters
   TCtrack* track=new TCtrack();//clean up memory mess
   vector<TCcluster> trackCl;
   trackCl.push_back(startClusters.at(maxPosGM1x));
   trackCl.push_back(clSI1X.at(maxPosSI1x));
   trackCl.push_back(clSI2X.at(maxPosSI2x));
   trackCl.push_back(endClusters.at(maxPosGM2x));
   trackCl.push_back(startClusters.at(maxPosGM1y));
   trackCl.push_back(clSI1Y.at(maxPosSI1y));
   trackCl.push_back(clSI2Y.at(maxPosSI2y));
   trackCl.push_back(endClusters.at(maxPosGM2y));
   track->addClusters(trackCl);
   if(track->fit(1,2,3,4,5,6,7,8)){
     event->addTrack(track);
     histContainer->histogramChi2->Fill(track->getChi2()/track->getNDF());
     histContainer->histogramChi2rough->Fill(track->getChi2()/track->getNDF());
     histContainer->histogramNDF->Fill(track->getNDF());
     histContainer->fillRes(track,alignmentFile);
   }
  
 }
 









    if(fit&&brute) {
      
      double chiStore=99999999;
      for(unsigned int endI=0;endI<endClusters.size()/2;++endI){
	if(next_event){
	  continue;
	}
	for(unsigned int i=0;i<startClusters.size()/2;i++){
	  if(next_event){
	    continue;
	  }
	  for(unsigned int j=0;j<clSI1X.size();++j){
	    if(next_event){
	      continue;
	    }
	    for(unsigned int k=0;k<clSI2X.size();++k){
	      if(next_event){
		continue;
	      }
              for(unsigned int l=0;l<clSI1Y.size();++l){
		if(next_event){
		  continue;
		}
                for(unsigned int m=0;m<clSI2Y.size();++m){
		  if(next_event){
		    continue;
		  }
                  int id1X =2*i;
                  int id1Y =2*i+1;
                  int id2X =2*endI;
                  int id2Y =2*endI+1;
                  TCtrack* track=new TCtrack();//clean up memory mess
                  vector<TCcluster> trackCl;
                  trackCl.push_back(startClusters.at(id1X));
                  trackCl.push_back(clSI1X.at(j));
                  trackCl.push_back(clSI2X.at(k));
		  trackCl.push_back(endClusters.at(id2X));
                  trackCl.push_back(startClusters.at(id1Y));
                  trackCl.push_back(clSI1Y.at(l));
                  trackCl.push_back(clSI2Y.at(m));
		  trackCl.push_back(endClusters.at(id2Y));
                  n_tracks++;
                  track->addClusters(trackCl);
                  if(track->fit(1,2,3,4,5,6,7,8)&&track->getChi2()/track->getNDF()<chiStore){

		    if(disp){
		      cout<<i_ev<<endl;
		      gROOT->SetStyle("Plain");
		      gStyle->SetPalette(1);
		      TCanvas * c = new TCanvas("Event display","Event Display",1,1,1200,480);
		      c->Divide(2,2);
		      
		      TGraph *xTrackStartEndPoint = new TGraph;
		      TGraph *yTrackStartEndPoint = new TGraph;
		      double ax1=track->getAx();// ax = tx
		      double bx1=track->getBx();
		      double ay1=track->getAy();// ay = ty
		      double by1=track->getBy();
		      char bufFormula[50];
		      
		      sprintf(bufFormula,"%f*x+%f",ax1,bx1);
		      TF1 *tf = new TF1("track1x",bufFormula,-10, 200);
		      tf->SetLineWidth(1.);
		      sprintf(bufFormula,"%f*x+%f",ay1,by1);
		      TF1 *tf2 = new TF1("track1y",bufFormula,-10, 200);
		      tf2->SetLineWidth(1.);
		      
		      cout<<"ax1 "<<ax1<<" bx1 "<<bx1<<endl;
		      cout<<"ay1 "<<ay1<<" by1 "<<by1<<endl;
		      //TMultiGraph *xGraph= new TMultiGraph();
		      //TMultiGraph *yGraph= new TMultiGraph();

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
		      


		      (c->cd(1))->Clear();
		      x_event->Draw("AP*");
		      xTrackStartEndPoint->Draw("*");
		      tf->Draw("LSAME");

		      (c->cd(2))->Clear();
		      y_event->Draw("AP*");
		      yTrackStartEndPoint->Draw("*");
		      tf2->Draw("LSAME");
		      
		      (c->cd(3))->Clear();
		      x_eventAmp->SetMarkerStyle(20);
		      x_eventAmp->Draw("pcol");                     
		      		      
		      (c->cd(4))->Clear();
		      y_eventAmp->SetMarkerStyle(20);
		      y_eventAmp->Draw("pcol");


		      c->Update();
		      c->Modified();
		      
		      gApplication->SetReturnFromRun(kTRUE);
		      gSystem->Run();
		      gROOT->Reset();
		      cout<<"after"<<endl;
		      
		      delete c;
		   
		      /*
		      delete xGraph;
		      delete yGraph;
		      delete tf;
		      delete tf2;
		      delete xTrackStartEndPoint;
		      delete yTrackStartEndPoint;
		      */
		    }
                    store=track;
                    chiStore=track->getChi2()/track->getNDF();
		    /* event->addTrack(store);
		    histContainer->histogramChi2->Fill(store->getChi2()/store->getNDF());
		    histContainer->histogramChi2rough->Fill(store->getChi2()/store->getNDF());
		    histContainer->histogramNDF->Fill(store->getNDF());
		    histContainer->fillRes(store,alignmentFile);*/

                  } else{
		    delete track;
		  }
                } //end clSI2Y m
              } //end clSI1Y l
	    } // end clSI2X k
	  } //end clSI1X j
	} // end startClusters i 2i and 2i+1
      }//end end clusters
      
      if(store!=NULL/*&&(store->getChi2()/store->getNDF())<15*/){
	event->addTrack(store);
	histContainer->histogramChi2->Fill(store->getChi2()/store->getNDF());
	histContainer->histogramChi2rough->Fill(store->getChi2()/store->getNDF());
	histContainer->histogramNDF->Fill(store->getNDF());
	histContainer->fillRes(store,alignmentFile);
      }
      if(next_event){
	controlCs=0;
	next_event=false;
	continue;
      }

    }
    if(fit&&twoPoint){
      double chiStore=99999999;
      for(unsigned int i=0;i<startClusters.size()/2;i++){
        for(unsigned int j=0;j<clSI1X.size();++j){
          for(unsigned int k=0;k<clSI2X.size();++k){
            for(unsigned int l=0;l<clSI1Y.size();++l){
              for(unsigned int m=0;m<clSI2Y.size();++m){                
                int id1X =2*i;
                int id1Y =2*i+1;
                n_tracks++;
                double ax1,ax2,ax3,bx1,bx2,bx3;
                double ay1,ay2,ay3,by1,by2,by3;
                TVector3 gm1xV=startClusters.at(id1X).posXYZ();
                TVector3 si1xV=clSI1X.at(j).posXYZ();
                TVector3 si2xV=clSI2X.at(k).posXYZ();
                
                TVector3 gm1yV=startClusters.at(id1Y).posXYZ();
                TVector3 si1yV=clSI1Y.at(l).posXYZ();
                TVector3 si2yV=clSI2Y.at(m).posXYZ();
                
                ax1=(si2xV.x() - si1xV.x())/(si2xV.z() - si1xV.z());
                ax2=(si2xV.x() - gm1xV.x())/(si2xV.z() - gm1xV.z());
                ax3=(si1xV.x() - gm1xV.x())/(si1xV.z() - gm1xV.z());
                
                ay1=(si2yV.y() - si1yV.y())/(si2yV.z() - si1yV.z());
                ay2=(si2yV.y() - gm1yV.y())/(si2yV.z() - gm1yV.z());
                ay3=(si1yV.y() - gm1yV.y())/(si1yV.z() - gm1yV.z());
                
                bx1=si1xV.x()-ax1*si1xV.z();
                bx2=gm1xV.x()-ax2*gm1xV.z();
                bx3=si1xV.x()-ax3*si1xV.z();
                
                by1=si1yV.y()-ay1*si1yV.z();
                by2=gm1yV.y()-ay2*gm1yV.z();
                by3=si1yV.y()-ay3*si1yV.z();
                
                  
                histContainer->histogramGM1XunBiResi->Fill(gm1xV.x()-ax1*gm1xV.z()-bx1);
                histContainer->histogramGM1YunBiResi->Fill(gm1yV.y()-ay1*gm1yV.z()-by1);;
                histContainer->histogramSI1XunBiResi->Fill(si1xV.x()-ax2*si1xV.z()-bx2);;
                histContainer->histogramSI1YunBiResi->Fill(si1yV.y()-ay2*si1yV.z()-by2);;
                histContainer->histogramSI2XunBiResi->Fill(si2xV.x()-ax3*si2xV.z()-bx3);;
                histContainer->histogramSI2YunBiResi->Fill(si2yV.y()-ay3*si2yV.z()-by3);;
                
                histContainer->histogramGM1XunBiResiVu2d->Fill(gm1xV.x(),gm1xV.x()-ax1*gm1xV.z()-bx1);
                histContainer->histogramGM1YunBiResiVu2d->Fill(gm1yV.y(),gm1yV.y()-ay1*gm1yV.z()-by1);
                histContainer->histogramSI2XunBiResiVu2d->Fill(si2xV.x(),si2xV.x()-ax3*si2xV.z()-bx3);
                histContainer->histogramSI2YunBiResiVu2d->Fill(si2yV.y(),si2yV.y()-ay3*si2yV.z()-by3);
                histContainer->histogramSI1XunBiResiVu2d->Fill(si1xV.x(),si1xV.x()-ax2*si1xV.z()-bx2);
                histContainer->histogramSI1YunBiResiVu2d->Fill(si1yV.y(),si1yV.y()-ay2*si1yV.z()-by2);
                
                } //end clSI2Y m
            } //end clSI1Y l
          } // end clSI2X k
        } //end clSI1X j
      }// end startClusters i 2i and 2i+1
    }
    
    /*
      TMatrixT<double> covar = store->getErrMatrix();
      double dax=store->getDax();
      double day=store->getDay();
      double dbx=store->getDbx();
      double dby=store->getDby();
    */
    
      /*cout<<"dax "<<dax*dax<<" dbx "<<dbx*dbx<<" day "<<day*day<<" dby "<<dby*dby<<endl;
        covar.Print();*/
      

    
    if(fit&&!brute&&!bruteMax){
      vector<TCcluster> clTrack1;
      vector<TCcluster> clTrack2;
      TGraph *clFit_x1 = new TGraph;//first x track
      TGraph *clFit_y1 = new TGraph;//first y track
      TGraph *clFit_x2 = new TGraph;
      TGraph *clFit_y2 = new TGraph;
      n_points=0;
      int n_points2=0;//for filling TGraph, used for drawing
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
      TCtrack* track1 = new TCtrack;
      TCtrack* track2 = new TCtrack;
      track1->addClusters(clTrack1);
      track2->addClusters(clTrack2);
      if(track1->fit(1,2,3,4,5,6,7,8)&&track2->fit(1,2,3,4,5,6,7,8)){
        if(disp){
          cout<<"track fit converged with chi2 "<<track1->getChi2()/track1->getNDF()<<endl;
        }
        histContainer->histogramChi2->Fill(track1->getChi2()/track1->getNDF());
        histContainer->histogramChi2rough->Fill(track1->getChi2()/track1->getNDF());
        histContainer->histogramNDF->Fill(track1->getNDF());
        n_tracks++;
        //        event->addTrack(track1);
        //        event->addClusters(clTrack1);

        histContainer->histogramChi2->Fill(track2->getChi2()/track2->getNDF());
        histContainer->histogramChi2rough->Fill(track2->getChi2()/track2->getNDF());
        histContainer->histogramNDF->Fill(track2->getNDF());
        n_tracks++;
        //        event->addTrack(track2);
        //        event->addClusters(clTrack2);
        histContainer->fillRes(track1,alignmentFile);
        histContainer->fillRes(track2,alignmentFile);
        if(disp){
	  //	  dispDraw(track1,track2,clFit_x1,clFit_y1,clFit_x2,clFit_y2,x_event,y_event);
	  houghXZ->draw(false,1,512,600,480,event);
	  houghYZ->draw(false,640,512,600,480,event);
	  gApplication->SetReturnFromRun(kTRUE);
	  gSystem->Run();
	  gROOT->Reset();
        }//end event display
      }//end if fit true
      delete track1;
      delete track2;
      delete clFit_x1;
      delete clFit_x2;
      delete clFit_y1;
      delete clFit_y2;
      
    }//end fit
    delete x_event;
    delete y_event;
    
    if(event->nTracks()==0){
      continue;
    }
        //cout<<"before fill"<<endl;
    eventTreeOut->Fill();
    
    if(controlC){ 
      break;
    }
    //    cout<<"before fill"<<endl;
  }//end looping of eventTree
  event->clear();
  // histogramTrack->Fill(event->nTracks());  
  //  

  std::cout<<nEvents<<" , nTrackTries: "<<n_tracks<<", To Many cl: "
	   <<nToMany<< ", Empty: "<<nZero<<", noiseCut: "<<noiseCut
	   <<", ratioCut: "<<ratioCut<<"********************************************************************"<<std::endl;
  
  eventTreeOut->Write();

  trackOutFile->Close();
  delete trackOutFile;
  delete tree;
  delete GM01X1;
  delete GM01Y1;
  delete GM02X1;
  delete GM02Y1;
  delete SI01X1;
  delete SI01Y1;;
  delete SI02X1;
  delete SI02Y1;
  histContainer->write(outHist);
  delete histContainer;
  return 0;
} 



