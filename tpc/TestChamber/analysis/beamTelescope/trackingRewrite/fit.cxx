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
#include "histContainers.h"

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
  //  TCanvas * c=NULL;
  string infilePath;
  string outFilePath;
  string alignmentFilePath;
  string histFilePath;
  string mode;
  trackHistContainer* histCont =new trackHistContainer();
  ConfigFile cf( argv[1] );
  
  bool disp=false;


  if(!(cf.readInto(infilePath , "inFile") )) failedConf("infile");
  if(!(cf.readInto(outFilePath , "outFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(alignmentFilePath , "alignmentFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(histFilePath , "histFile") )) failedConf("histFile");
  if(!(cf.readInto(mode , "mode") )) failedConf("mode");
  if(mode=="disp"){
    disp=true;
  }

  TFile::Open(infilePath.c_str());
  TTree *inTree = (TTree*)gROOT->FindObject("at_pr");
  TCevent *inEvent = new TCevent();
  TCevent *outEvent = new TCevent();
  TBranch *branchEvent=inTree->GetBranch("event");
  branchEvent->SetAddress(&inEvent);
  int nEvents=inTree->GetEntries();

  TFile* outFile = new TFile(outFilePath.c_str(),"RECREATE");
  TTree* eventTreeOut = new TTree("at_tr","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&outEvent,32000,99);
  int totClusters=0;

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);

  
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    //cout<<"bla"<<endl;
    inTree->GetEntry(i_ev);
    outEvent->clear();   
    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%250==0){
      cout<<i_ev<<" n clusters "<<totClusters<<endl;
    }
    for(unsigned int iCl=0;iCl<inEvent->nTracks();++iCl) {
      TCtrack* track = inEvent->getTrack(iCl);
      TCtrack* outTrack=new TCtrack;//cout<<track->nClFit()<<endl;
     
      for(unsigned int i=0;i<track->nCl();++i){
	TCcluster cl=track->getCl(i);
	outTrack->addCluster(cl);
      }
      if(outTrack->nClFit()>5&&outTrack->fit(1,2,3,4,5,6,7,8)){
	histCont->fillRes(outTrack);
	outEvent->addTrack(outTrack);
	//	cout<<"test"<<endl;
      }else{
	delete outTrack;
      }
    }
    eventTreeOut->Fill();

  }
  eventTreeOut->Write();
  outFile->Close();
  histCont->write(histFilePath);
  delete histCont;
}
