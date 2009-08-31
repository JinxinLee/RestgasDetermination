//STL and C includes
#include <iostream>
#include <string>
#include <signal.h>

//ROOT headers
#include <TGraph.h>
#include <TChain.h>
#include <TFile.h>
#include <TApplication.h>
#include <TVector3.h>
#include <TRandom3.h>
#include <TSystem.h>
#include <TROOT.h>
//TestChamber includes

#include "TCcluster.h"
#include "TCalign.h"
#include "TCtrack.h"
#include "TCevent.h"
#include "ConfigFile.h"


#include "TCfast2Dhough.h"

static bool controlC=false;
static int controlCs=0;
using std::cout;
using std::endl;
void signalHandler2(int signal) {
  if (signal==SIGINT) {
    cout<<"CTRL + C pressed saving and exiting, press again to abort"<<endl;
    controlC = true;
    if (controlCs!=0) {
      cout<<"aborting"<<endl;
      exit(1);
    }
    controlCs++;
    
  }
}

void failedConf(std::string var){
  std::cerr << "Reading parameter " << var << " from conf file failed ->abort"
	    << std::endl;
  throw;
}

int main(int argc,char **argv){
  using namespace std;
  signal(SIGINT, signalHandler2);  
  if(!(argc==2)){
    cerr<<"Wrong number of arguments, "<<argc<<endl
        <<"Syntax should be ./tracker configFile"<<endl;
    throw;
  }
  TApplication theApp("theApp",NULL,NULL);
  //reading configFile
  TCanvas * c=NULL;
  string infilePath;
  string outFilePath;
  string alignmentFilePath;
  string histFilePath;
  string mode;
  ConfigFile cf( argv[1] );
  bool hough = true;
  bool maxAmp=false;
  bool disp=false;
  int houghThreshold;
  int houghDepth;
  bool debug =true;
  if(!(cf.readInto(infilePath , "inFile") )) failedConf("infile");
  if(!(cf.readInto(outFilePath , "outFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(alignmentFilePath , "alignmentFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(histFilePath , "histFile") )) failedConf("histFile");
  if(!(cf.readInto(mode , "mode") )) failedConf("mode");
  if(mode=="hough"){
    hough=true;
    maxAmp=false;
    disp=false;
    if(!(cf.readInto(houghThreshold , "houghThreshold") )) failedConf("houghThreshold");
    if(!(cf.readInto(houghDepth , "houghDepth") )) failedConf("houghDepth");
  }else if(mode=="maxAmp"){
    hough=false;
    maxAmp=true;
    disp=false;
    houghDepth=0;
    houghThreshold=0;
  }else if(mode=="eventDisp"){
    hough=true;
    maxAmp=false;
    disp=true;
    if(!(cf.readInto(houghThreshold , "houghThreshold") )) failedConf("houghThreshold");
    if(!(cf.readInto(houghDepth , "houghDepth") )) failedConf("houghDepth");
  }else{
    std::cerr << "Unknown mode  " << mode << " from conf file ->abort"
	      << std::endl;
    throw;
  }
  
  TFile::Open(infilePath.c_str());
  TTree *inTree = (TTree*)gROOT->FindObject("at_cl");
  TCevent *inEvent = new TCevent();
  TCevent *outEvent = new TCevent();
  TBranch *branchEvent=inTree->GetBranch("event");
  branchEvent->SetAddress(&inEvent);
  int nEvents=inTree->GetEntries();
  /*
    outTree wil contain events with clusters, and track candidates.
  */

  TFile* outFile = new TFile(outFilePath.c_str(),"RECREATE");
  TTree* eventTreeOut = new TTree("at_pr","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&outEvent,32000,99);
  int totClusters=0;

  TVector3 x(1,0,0);
  TVector3 y(0,1,0);
  TVector3 z(0,0,1);
  TCfast2DHough* houghXZ = new TCfast2DHough(-x,z,true);
  TCfast2DHough* houghYZ = new TCfast2DHough(y,z,true);
  cout<<"thresholds "<<endl<<houghThreshold<<endl<<houghDepth<<endl;
  houghXZ->setThreshold(houghThreshold);
  houghXZ->setDepth(houghDepth);
  houghYZ->setThreshold(houghThreshold);
  houghYZ->setDepth(houghDepth);

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);

  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    inTree->GetEntry(i_ev);
    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%1==0){
      cout<<i_ev<<" n clusters "<<totClusters<<endl;
    }
    outEvent->clear();
    
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();

    vector<TCcluster> y_clusters;
    vector<TCcluster> x_clusters;
    int x_count=0;
    int y_count=0;
    //cout<<"nr cluster"<<inEvent->nClusters()<<endl;
      
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
   vector<TCcluster> outClusters;
   vector<TCtrack*> outTracks;
   for(unsigned int iCl=0;iCl<inEvent->nClusters();++iCl) {
     TCcluster cl=inEvent->getCluster(iCl);
     outClusters.push_back(cl);
     int detID=cl.getId();
      if(detID%2==0){
	y_clusters.push_back(cl);
	y_count++;
	y_event->SetPoint(y_count, cl.posXYZ().z(), cl.posXYZ().y());
	if(disp)
	  cout<<"z "<<cl.posXYZ().z()<<"   y "<<cl.posXYZ().y()<<endl;
      }else{
	x_clusters.push_back(cl);
	x_event->SetPoint(x_count, cl.posXYZ().z(), cl.posXYZ().x());
	x_count++;
	if(disp)
	  cout<<"z "<<cl.posXYZ().z()<<"   x "<<cl.posXYZ().x()<<endl;
      }
      if(maxAmp){
	switch(detID){
	case 1:
	  if(cl.getAmp()>maxAmpGM1x){
	    maxAmpGM1x=cl.getAmp();
	    maxPosGM1x=iCl;
	  }
	  break;
	case 2:
	  if(cl.getAmp()>maxAmpGM1y){
	    maxAmpGM1y=cl.getAmp();
	    maxPosGM1y=iCl;
	  }
	  break;
	case 3:
	  if(cl.getAmp()>maxAmpSI1x){
	    maxAmpSI1x=cl.getAmp();
	    maxPosSI1x=iCl;
	  }
	  break;
	case 4:
	  if(cl.getAmp()>maxAmpSI1y){
	    maxAmpSI1y=cl.getAmp();
	    maxPosSI1y=iCl;
	  }
	  break;
	case 5:
	  if(cl.getAmp()>maxAmpSI2x){
	    maxAmpSI2x=cl.getAmp();
	    maxPosSI2x=iCl;
	  }
	  break;
	case 6:
	  if(cl.getAmp()>maxAmpSI2y){
	    maxAmpSI2y=cl.getAmp();
	    maxPosSI2y=iCl;
	  }
	  break;
	case 7:
	  if(cl.getAmp()>maxAmpGM2x){
	    maxAmpGM2x=cl.getAmp();
	    maxPosGM2x=iCl;
	  }
	  break;
	case 8:
	  if(cl.getAmp()>maxAmpGM2y){
	    maxAmpGM2y=cl.getAmp();
	    maxPosGM2y=iCl;
	  }
	  break;
	default:
	  break;
	}
      }
    }
    if(maxAmp){
      vector<TCcluster> clTrack;
      if(maxPosGM1x>-1){
	inEvent->getCluster(maxPosGM1x).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosGM1x));
	++totClusters;
      }
      if(maxPosGM1y>-1){
	inEvent->getCluster(maxPosGM1y).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosGM1y));
	++totClusters;
      }
      if(maxPosSI1x>-1){
	inEvent->getCluster(maxPosSI1x).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosSI1x));
	++totClusters;
      }
      if(maxPosSI1y>-1){
	inEvent->getCluster(maxPosSI1y).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosSI1y));
	++totClusters;
      }
      if(maxPosSI2x>-1){
	inEvent->getCluster(maxPosSI2x).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosSI2x));
	++totClusters;
      }
      if(maxPosSI2y>-1){
	inEvent->getCluster(maxPosSI2y).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosSI2y));
	++totClusters;
      }
      if(maxPosGM2x>-1){
	inEvent->getCluster(maxPosGM2x).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosGM2x));
	++totClusters;
      }
      if(maxPosGM2y>-1){
	inEvent->getCluster(maxPosGM2y).setFit();
	clTrack.push_back(inEvent->getCluster(maxPosGM2y));
	++totClusters;
      }
      outTracks.push_back(new TCtrack());
      for(unsigned int i=0;i<clTrack.size();++i){
	clTrack.at(i).setFit();
      }
      (outTracks.back())->addClusters(clTrack);
      //cout<<outTracks.back()->nClFit()<<endl;
    }
    if(hough){
      outTracks.push_back(new TCtrack());
      vector<TCcluster> clTrack;
      if(disp||debug){
	houghXZ->setDebug(true);
	houghYZ->setDebug(true);
      }
      if(disp||debug){
	cout<<"hough XZ make on "<<x_count<<" clusters *************************************************"<<endl;
      }
      houghXZ->make(x_clusters);
      if(disp||debug){
	cout<<"hough YZ  on "<<y_count<<" clusters*******************************************************"<<endl;
      }
      houghYZ->make(y_clusters);
      if(houghXZ->getNmax()>0&&houghYZ->getNmax()){
	for(unsigned int i=0;i<x_clusters.size();++i){
	  if(houghXZ->hot(i)){
	    x_clusters.at(i).setFit();
	  }
	  clTrack.push_back(x_clusters.at(i));
	}
	for(unsigned int i=0;i<y_clusters.size();++i){
	  if(houghYZ->hot(i)){
	    y_clusters.at(i).setFit();
	  }
	  clTrack.push_back(y_clusters.at(i));
	}
	(outTracks.back())->addClusters(clTrack);
      }
    }
    if(disp){
      houghXZ->draw(false );    
      houghYZ->draw(false,800);
      if(c==NULL){
	delete c;
      }
      c = new TCanvas("Event display","Event Display",1280,1,960,480);
      c->Divide(2,1);
      (c->cd(1))->Clear();
      x_event->Draw("AP*");
      (c->cd(2))->Clear();
      y_event->Draw("AP*");
      gApplication->SetReturnFromRun(kTRUE);
      gSystem->Run();
      gROOT->Reset();
    }
    for(unsigned int i = 0; i<outTracks.size();++i){
      outEvent->addTrack(outTracks.at(i));
    }
    if(outTracks.size()>0){
      eventTreeOut->Fill();
    }
  }//end event loop
  eventTreeOut->Write();
  outFile->Close();
}
