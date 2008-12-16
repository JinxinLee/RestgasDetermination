#include<iostream>
#include<cassert>
#include"tpcEvent.h"
#include"TApplication.h"
#include"TGraph.h"
#include"TCanvas.h"
#include"TPolyMarker3D.h"
#include"TSystem.h"

#include "SignalAnalysis.h"
#include "../Mapping.h"
#include "LcardMapping.h"

int main(int argc,char **argv){
  assert(argc==2);

  TApplication theApp("theApp",NULL,NULL);

  TFile::Open(argv[1]);
  TTree *tree = (TTree*)gROOT->FindObject("datatree");

  tpcEvent e(tree);

  Mapping hexMap;
  LcardMapping lMap;
  hexMap.read("../gem_mapping2.txt");
  lMap.read("igor2.txt");

  //  void map(unsigned int conn, unsigned int pin, double& x, double& y){
  // bool map(unsigned int chip,unsigned int ch,unsigned int& pin){

  while(e.loadNext()){

    UInt_t amps[76][255];
    std::vector<pulse_t*> pulses[76];
    int counter[76];
    for(int i=0;i<76;++i) counter[i]=0;
    int nPulses=0;
    int nChannels=0;
    for(int i=0;i<e.tNumberOfSamples;++i){
      int channelId = e.tChannelId[i];
      amps[channelId][counter[channelId]++] = 1024.-e.tAmp[i];
    }

    for(int i=0;i<76;++i) {
      pulses[i] = psa(amps[i],254);
      for(int j=0;j<pulses[i].size();++j){
	pulses[i].at(j)->ch = i;
      }
      nPulses+=pulses[i].size();
      if(pulses[i].size()>0)nChannels+=1;
    }
    double x[nPulses];
    double y[nPulses];
    int pulsCounter = 0;

    for(int j=0;j<76;++j){
      for(int k=0;k<pulses[j].size();++k){
	x[pulsCounter] = pulses[j].at(k)->startTime;
	y[pulsCounter++] = j;
      }
    }

    if(nPulses>0){
      static int rootCounter = 0;
      char buf2[50];
      sprintf(buf2,"graph%d",rootCounter++);
      TCanvas *c = new TCanvas();
      c->SetName(buf2);
      TGraph *g = new TGraph(nPulses,x,y);
      char buf[50];
      static int eventCounter(0);
      sprintf(buf,"event %d",eventCounter++);
      sprintf(buf2,"graph%d",rootCounter++);
      g->SetName(buf2);
      g->SetTitle(buf);
      g->Draw("A*");
      gApplication->SetReturnFromRun(kTRUE);
      gSystem->Run();
      gROOT->Reset();
      delete g;
      delete c;
    }
    std::cout << nPulses << " " << nChannels <<  std::endl;
    
    std::vector<double> X;
    std::vector<double> Y;
    std::vector<double> T;
    
    for(int i=0;i<76;++i){
      for(int j=0;j<pulses[i].size();++j){
	
	//  void hex map(unsigned int conn, unsigned int pin, double& x, double& y){
	//  bool l map(unsigned int chip,unsigned int ch,unsigned int& pin){
	
	int conn = 1;
	int chip = 1;
	
	unsigned int pin;
	if(!lMap.map(chip,pulses[i].at(j)->ch,pin)) continue;
	std::cout << chip << " " << pulses[i].at(j)->ch << " " << pin << std::endl;
	double x,y;
	hexMap.map(conn,pin,x,y);
	X.push_back(x);
	Y.push_back(y);
	T.push_back(pulses[i].at(j)->startTime);
	
      }
      
    }
    
    int nPoints = X.size();
    TPolyMarker3D* pm = new TPolyMarker3D(nPoints);
    for(int i=0;i<nPoints;++i){
      pm->SetPoint(i,X.at(i),Y.at(i),T.at(i));
    }
    pm->Draw();
  }//end of event loop
}
