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
#include <TStyle.h>
#include <Math/Polynomial.h>
#include <TMultiGraph.h>

#include "../../../src/TCcluster.h"
#include "../../../src/TCalign.h"
#include "../../../src/TCtrack.h"
#include "../../../src/TCevent.h"
#include "tracks.h"
#include "helpers.h"

static bool controlC=false;
static int  controlCs=0;

void signalHandler1(int signal) {
  if (signal==SIGINT) {
    std::cout<<"breaking and storing"<<std::endl;
    controlC = true;
    if (controlCs!=0) {
      std::cout<<"aborting"<<std::endl;
      exit(1);
    }
    controlCs++;
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
  
  signal(SIGINT, signalHandler1);

  if(!(argc==4||argc==5)){
    cerr<<"Wrong number of arguments, "<<argc<<endl
        <<"Syntax should be ./tracker infile alignmentfile mode histfile"<<endl;
    throw;
  }
  TApplication theApp("theApp",NULL,NULL);
  HistContainer* histContainer = new HistContainer();
  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("at2");
  string alignmentFile = argv[2];
  string outHist = "clusterMultipCut.root";
  if(argc==5){
    outHist=argv[4];
  }
  bool disp=false;
  bool fit=true;
  bool brute=false;
  bool hist=false;
  bool twoPoint=false;
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
  }else if(mode == "twoPoint"){
    cout<<mode<<endl;
    disp = false;
    fit = true;
    twoPoint = true;
    brute = false;
  }

  TCevent *EVENT=new TCevent();
  TCevent *eventStore=new TCevent();
  TBranch *branchEvent =tree->GetBranch("event");  
  
  branchEvent->SetAddress(&EVENT);

  int nEvents=tree->GetEntries();
  TFile* trackOutFile = new TFile("tracks.root","RECREATE");
  TTree* eventTreeOut = new TTree("at2","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&EVENT,32000,99);
  /*
    Creating an instance of TCallign to assure wich alignmentfile is used
  */
  cout<<"before alignment"<<endl;
  TCalign* a = TCalign::getInstance(alignmentFile);
  a->clear();
  a->read(alignmentFile);

  int n_tracks = 0;
  int nToMany=0;
  int nZero=0;
  int noiseCut=0;
  int ratioCut=0;
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    if(i_ev%250==0){
      cout<<i_ev<<endl;
    }
    tree->GetEntry(i_ev);
    for(unsigned int i=0;i<EVENT->nTracks();++i){
      TCtrack * track=EVENT->getTrack(i);
      track->fit(1,2,3,4,5,6,7,8);
      histContainer->histogramChi2->Fill(track->getChi2()/track->getNDF());
      histContainer->histogramChi2rough->Fill(track->getChi2()/track->getNDF());
      histContainer->histogramNDF->Fill(track->getNDF());
      histContainer->fillRes(track,alignmentFile);
    }
    eventTreeOut->Fill();
    if(controlC){ 
      break;
    }
  }
  std::cout<<nEvents<<" , nTrackTries: "<<n_tracks<<", To Many cl: "
	   <<nToMany<< ", Empty: "<<nZero<<", noiseCut: "<<noiseCut
	   <<", ratioCut: "<<ratioCut<<"********************************************************************"<<std::endl;
  
  eventTreeOut->Write();

  trackOutFile->Close();
  delete trackOutFile;
  delete tree;
  delete EVENT;
  histContainer->write(outHist);
  delete histContainer;
  return 0;
} 



