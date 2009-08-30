//STL and C headers
#include <list>
#include <iostream>
#include <sstream> 
#include <string>
#include <signal.h>
//boost headers
#include <boost/tokenizer.hpp>
//ROOT headers
#include <TChain.h>
#include <TFile.h>
#include <TApplication.h>
#include <TVector3.h>
#include <TRandom3.h>
//normal includes
#include "../Hits.h"
#include "ConfigFile.h"
#include "TCevent.h"
using namespace std;

static bool controlC=false;
static int controlCs=0;

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
  TTree *inTree = (TTree*)gROOT->FindObject("Hits");

  vector<CsGEMPlane*> detectorPlanes;
  vector<double> noiseThreshold; //in units of sigma
  vector<double> a1a2Threshold;
  vector<double> a0a2Threshold;
  vector<double> cutMaxU;
  vector<double> cutMinU;
  for(int i=0;i<8;++i){
    detectorPlanes.push_back(new CsGEMPlane());
    /*
      Reading cuts from configfile
     */
    string s = "";
    ostringstream oss;
    oss<<(i+1);
    string name="cut"+oss.str();
    if(!(cf.readInto(s, name.c_str()) )) failedConf(name.c_str());
    istringstream iss(s);
    double noise,a0a2,a1a2,minU,maxU;
    iss>>noise>>a1a2>>a0a2>>minU>>maxU;
    cout<<i<<" noise "<<noise
	<<" a0a2 "<<a0a2
	<<" a1a2 "<<a1a2
	<<" minU "<<minU
	<<" maxU "<<maxU<<endl;
    noiseThreshold.push_back(noise);
    a1a2Threshold.push_back(a1a2);
    a0a2Threshold.push_back(a0a2);
    cutMinU.push_back(minU);
    cutMaxU.push_back(maxU);
  }
  
  TBranch *branchGM01X1 =inTree->GetBranch("GM01X1__.");
  TBranch *branchGM01Y1 =inTree->GetBranch("GM01Y1__.");
  TBranch *branchGM02X1 =inTree->GetBranch("GM02X1__.");
  TBranch *branchGM02Y1 =inTree->GetBranch("GM02Y1__.");
  TBranch *branchSI01X1 =inTree->GetBranch("SI01X1__.");
  TBranch *branchSI01Y1 =inTree->GetBranch("SI01Y1__.");
  TBranch *branchSI02X1 =inTree->GetBranch("SI02X1__.");
  TBranch *branchSI02Y1 =inTree->GetBranch("SI02Y1__.");  
  
  branchGM01X1->SetAddress(&(detectorPlanes.at(0)));
  branchGM01Y1->SetAddress(&(detectorPlanes.at(1)));
  branchSI01X1->SetAddress(&(detectorPlanes.at(2)));
  branchSI01Y1->SetAddress(&(detectorPlanes.at(3)));
  branchSI02X1->SetAddress(&(detectorPlanes.at(4)));
  branchSI02Y1->SetAddress(&(detectorPlanes.at(5)));
  branchGM02X1->SetAddress(&(detectorPlanes.at(6)));
  branchGM02Y1->SetAddress(&(detectorPlanes.at(7)));
  

  int nEvents=inTree->GetEntries();

  TCevent* outEvent = new TCevent();
  /*
    outTree will contain events with clusters, no track candidates.
  */
  TFile* outFile = new TFile(outFilePath.c_str(),"RECREATE");
  TTree* outTree = new TTree("at_cl","testbench elsa analysis tree");
  outTree->Branch("event","TCevent",&outEvent,32000,99);

  TCalign* a = TCalign::getInstance(alignmentFilePath);
  a->clear();
  a->read(alignmentFilePath);
  
  vector<int> nClusters;
  vector<int> nCuts;
  int totClusters=0;
  int totCut=0;
  int posCut=0;
  for(int i=0;i<8;++i){
    nClusters.push_back(0);
    nCuts.push_back(0);
  }
  TRandom3 rand(0);
  for(int i_ev=0;i_ev<nEvents;i_ev++) {

    if(controlC){ 
      cout<<"jumping out of for loop"<<endl;
      break;
    }
    if(i_ev%500==0){
      cout<<i_ev<<" n clusters "<<totClusters<<" totCut "<<totCut<<" pos cut "<<posCut<<endl;
    }
    
    inTree->GetEntry(i_ev);
    outEvent->Clear();
    vector<TCcluster> eventClusters;
    for(unsigned int i=0;i<detectorPlanes.size();++i){
      double pitch=a->getPitch(i+1);
      double res=a->getRes(i+1);
      //cout<<detectorPlanes.at(i)->GetName()<<endl;
      for(list<CsGEMCluster*>::const_iterator it = (detectorPlanes.at(i)->GetClusters()).begin();it!=(detectorPlanes.at(i)->GetClusters()).end();++it){
	//cout<<"test"<<endl;
	vector<float> amps =(*it)->GetAmp();
	double amp2=amps.at(2);
	double amp1=amps.at(1);
	double amp0=amps.at(0);
	double ratioA1A2=(amp1+rand.Uniform(-0.5,0.5))/(amp2);
	double ratioA0A2=(amp0+rand.Uniform(-0.5,0.5))/(amp2);
	double noise=(*it)->GetNoise();
	if((ratioA0A2<a0a2Threshold.at(i))&&(ratioA1A2<a1a2Threshold.at(i))&&(amp2>noiseThreshold.at(i)*noise)){
	  double x=((*it)->GetPosition())*pitch;
	 
	  if(x>=cutMinU.at(i)&&x<=cutMaxU.at(i)){
	    TVector3 pos(x,0,0);
	    TVector3 err(0,0,0);
	    if(i>1&&i<7){
	      err=TVector3(res,0.5,0.1);
	    }else{
	      err=TVector3(((*it)->GetPositionErr())*pitch,0.5,0.1);
	    }
	    TCcluster _c(pos,err,amp2,(i+1));
	    eventClusters.push_back(_c);
	    nClusters.at(i)++;
	    totClusters++;
	    //	    cout<<i+1<<" x "<<_c.posXYZ().x()<<" y "<<_c.posXYZ().y()<<endl;
	  }else{
	    posCut++;
	  }
	  
	}else{
	  nCuts.at(i)++;
	  totCut++;
	}
      }//end looping over clusters
    }//end looping over detector planes
    /*
    string tmp;
    cout<<"bla enter"<<endl;
    cin>>tmp;
    */
    outEvent->addClusters(eventClusters);
    outTree->Fill();
  }//eventloop end
  outEvent->Clear();
  outFile->Write();
  outFile->Close();
  for(unsigned int i=0;i<detectorPlanes.size();++i){
    delete detectorPlanes.at(i);
  }
  detectorPlanes.clear();
  a->clear();
}

