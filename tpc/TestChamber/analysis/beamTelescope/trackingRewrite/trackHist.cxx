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
#include <TH1D.h>
//TestChamber includes

#include "TCcluster.h"
#include "TCalign.h"
#include "TCtrack.h"
#include "TCevent.h"
#include "ConfigFile.h"


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
  ConfigFile cf( argv[1] );
  


  if(!(cf.readInto(infilePath , "inFile") )) failedConf("infile");
  if(!(cf.readInto(alignmentFilePath , "alignmentFile") )) failedConf("alignmentFile");
  if(!(cf.readInto(histFilePath , "histFile") )) failedConf("histFile");

  TFile::Open(infilePath.c_str());
  TTree *inTree = (TTree*)gROOT->FindObject("at_tr");
  TCevent *inEvent = new TCevent();
  TBranch *branchEvent=inTree->GetBranch("event");
  branchEvent->SetAddress(&inEvent);
  int nEvents=inTree->GetEntries();
  int totClusters=0;

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);

  vector<TH1D*> clusterErr;

  for(int i=0;i<8;++i){
    char buf[10];
    sprintf(buf,"%i",(i+1));
    string name1="clusterErr";
    name1+=buf;
    clusterErr.push_back(new TH1D(name1.c_str(),name1.c_str(),10000,0,0.1));
  }
  for(int i_ev=0;i_ev<nEvents;i_ev++) {
    if(controlC){
      break;
    }
    //Cout<<"bla"<<endl;
    inTree->GetEntry(i_ev);
    for(unsigned int i_tr=0;i_tr<inEvent->nTracks();++i_tr){
      TCtrack* track=inEvent->getTrack(i_tr);
      for(unsigned int i_cl=0;i_cl<track->nCl();++i_cl){
        TCcluster cl=track->getCl(i_cl);
        clusterErr.at(cl.getId()-1)->Fill(cl.getErr().x());
      }
      if(track->getAy()*100+track->getBy()<1){
        //        std::cout<<inEvent->getEventNumber()<<std::endl;
      }
    }

    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%250==0){
      //   cout<<i_ev<<" n clusters "<<totClusters<<endl;
    }
  
  }
  
  TFile* histFile=new TFile(histFilePath.c_str(),"RECREATE");

  for(unsigned int i=0;i<8;++i){
    clusterErr.at(i)->Write();
  }

  histFile->Close();
  delete histFile;
}
