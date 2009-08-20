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
  if(!(argc==2)){
    cerr<<"Wrong number of arguments, "<<argc<<endl
        <<"Syntax should be ./tracker configFile"<<endl;
    throw;
  }
  TApplication theApp("theApp",NULL,NULL);
  //reading configFile

  string infilePath;
  string outFilePath;
  string alignmentFilePath;
  string histFilePath;

   ConfigFile cf( argv[1] );
  
  if(!(cf.readInto(infilePath , "inFile") )) failedConf("infile");
  if(!(cf.readInto(outFilePath , "outFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(alignmentFilePath , "alignmentFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(histFilePath , "histFile") )) failedConf("histFile");
  
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

  //  TFile* outFile = new TFile(outFilePath.c_str(),"RECREATE");
  TTree* eventTreeOut = new TTree("at_pr","testBench analysis tree");
  eventTreeOut->Branch("event","TCevent",&outEvent,32000,99);
  int totClusters=0;
  TVector3 x(1,0,0);
  TVector3 y(0,1,0);
  TVector3 z(0,0,1);
  TCfast2DHough* houghXZ = new TCfast2DHough(z,x);
  TCfast2DHough* houghYZ = new TCfast2DHough(z,y);

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);


  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    inTree->GetEntry(i_ev);
    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%250==0){
      cout<<i_ev<<" n clusters "<<totClusters<<endl;
    }
    TGraph* x_event =new TGraph();
    TGraph* y_event =new TGraph();

    vector<TCcluster> y_clusters;
    vector<TCcluster> x_clusters;
    int x_count=0;
    int y_count=0;
    cout<<"nr cluster"<<inEvent->nClusters()<<endl;
    for(unsigned int iCl=0;iCl<inEvent->nClusters();++iCl) {
      TCcluster cl=inEvent->getCluster(iCl);
      int detID=cl.getId();
      if(detID%2==0){
	y_clusters.push_back(cl);
	y_count++;
	y_event->SetPoint(y_count, cl.posXYZ().z(), cl.posXYZ().y());
	cout<<"z "<<cl.posXYZ().z()<<"   y "<<cl.posXYZ().y()<<endl;
      }else{
	x_clusters.push_back(cl);
	x_event->SetPoint(x_count, cl.posXYZ().z(), cl.posXYZ().x());
	x_count++;
	cout<<"z "<<cl.posXYZ().z()<<"   x "<<cl.posXYZ().x()<<endl;
      }
    }
    
    cout<<"hough XZ make on "<<x_count<<" clusters"<<endl;
    houghXZ->make(x_clusters);
    houghXZ->draw(false);
    cout<<"hough YZ  on "<<y_count<<" clusters"<<endl;
    houghYZ->make(y_clusters);    
    houghYZ->draw(false,800);

    TCanvas * c = new TCanvas("Event display","Event Display",1280,1,960,480);
    c->Divide(2,1);
    (c->cd(1))->Clear();
    x_event->Draw("AP*");
    
    (c->cd(2))->Clear();
    y_event->Draw("AP*");
    
    gApplication->SetReturnFromRun(kTRUE);
    gSystem->Run();
    gROOT->Reset();
  }
}
